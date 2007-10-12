#include "epublib.h"

int _ocf_parse_mimetype(struct ocf *ocf) {

  _epub_print_debug(ocf->epub, DEBUG_INFO, "looking for mime type");
  // Figure out mime type
  if (_ocf_get_file(ocf, MIMETYPE_FILENAME, &ocf->mimetype) == -1) {
    _epub_print_debug(ocf->epub, DEBUG_WARNING, 
                      "Can't get mimetype, assuming application/epub+zip (-)");
    ocf->mimetype = malloc(sizeof(char) * strlen("application/epub+zip")+1);
    strcpy(ocf->mimetype, "application/epub+zip");
  } else {
    _epub_print_debug(ocf->epub, DEBUG_INFO, "mimetype found %s", ocf->mimetype);
  }

  return 1;
}

int _ocf_parse_container(struct ocf *ocf) {

  _epub_print_debug(ocf->epub, DEBUG_INFO, "parsing container file %s", 
                    METAINFO_DIR "/" CONTAINER_FILENAME);

  char *containerXml;
  char *name = CONTAINER_FILENAME;
  if (! _ocf_get_file(ocf, METAINFO_DIR "/" CONTAINER_FILENAME, &containerXml))
    return 0;
  
  xmlTextReaderPtr reader;
  int ret;

  reader = xmlReaderForMemory(containerXml, strlen(containerXml), 
                              name, NULL, 0);
  if (reader != NULL) {
    ret = xmlTextReaderRead(reader);
    while (ret == 1) {

      if (xmlStrcmp(xmlTextReaderConstName(reader), 
                    (xmlChar *)"rootfile") == 0) {
        struct root *newroot = malloc(sizeof(struct root));
        newroot->next = NULL;
        newroot->mediatype = 
          xmlTextReaderGetAttribute(reader, (xmlChar *)"media-type");
        newroot->fullpath =
          xmlTextReaderGetAttribute(reader, (xmlChar *)"full-path");
        _epub_print_debug(ocf->epub, DEBUG_INFO, "found root in %s media-type is %s",
                          newroot->fullpath, newroot->mediatype);
        if (! ocf->roots) {
          ocf->roots = newroot;
        } else {
          ocf->roots->next = newroot;
        }
      }

      ret = xmlTextReaderRead(reader);
    }

    xmlFreeTextReader(reader);
    if (ret != 0) {
      _epub_print_debug(ocf->epub, DEBUG_ERROR, "failed to parse %s\n", name);
      return 0;
    }
  } else {
    _epub_print_debug(ocf->epub, DEBUG_ERROR, "unable to open %s\n", name);
    return 0;
  }
  
  
  return 1;
}

void _ocf_dump(struct ocf *ocf) {  
  printf("filename:\t %s\n", ocf->filename);
  printf("mimetype:\t %s\n", ocf->mimetype);
  
  struct root *curr = ocf->roots;

  while (curr) {
    printf("root:\n full path:\t %s\t\n media type:\t %s\n", 
           curr->fullpath, curr->mediatype);
    curr = curr->next;
  }

}

struct zip *_ocf_open(struct ocf *ocf, char *filename) {

  int err;
  char errStr[8192];
  struct zip *arch = NULL;

  if (! (arch = zip_open(filename, 0, &err))) {
    zip_error_to_str(errStr, sizeof(errStr), err, errno);
    _epub_print_debug(ocf->epub, DEBUG_ERROR, "%s - %s", filename, errStr); 
  }
  
  return arch;
}

void _ocf_close(struct ocf *ocf) {

  if (ocf->arch) {
    if (zip_close(ocf->arch) == -1) {
      _epub_print_debug(ocf->epub, DEBUG_ERROR, "%s - %s\n", 
                        ocf->filename, zip_strerror(ocf->arch));
    }
  }
  
  struct root *curr = ocf->roots;
  struct root *prev;
  
  while (curr) {
    free(curr->fullpath);
    free(curr->mediatype);
    prev = curr;
    curr = curr->next;
    free(prev);
  }

  free(ocf->filename);
  if (ocf->mimetype)
    free(ocf->mimetype);
  free(ocf);
}

// returns index if file exists else -1
int _ocf_check_file(struct ocf *ocf, const char *filename) {
  return zip_name_locate(ocf->arch, filename, 0);
}

// Get the file named filename from epub zip and pub it in fileStr
// Returns the size of the file or -1 on failure
int _ocf_get_file(struct ocf *ocf, const char *filename, char **fileStr) {
  
  struct epub *epub = ocf->epub;
  struct zip *arch = ocf->arch;

  struct zip_file *file = NULL;
  struct zip_stat *fileStat = malloc(sizeof(struct zip_stat));

  if (zip_stat(arch, filename, ZIP_FL_UNCHANGED, fileStat) == -1) {
    _epub_print_debug(epub, DEBUG_INFO, "%s - %s\n", 
                      filename, zip_strerror(arch));
    return -1;
  }
  
  if (! (file = zip_fopen_index(arch, fileStat->index, ZIP_FL_NODIR))) {
    _epub_print_debug(epub, DEBUG_INFO, "%s - %s\n", 
                      filename, zip_strerror(arch));
    return -1;
  }
  
  *fileStr = (char *)malloc((fileStat->size+1)* sizeof(char));
  
  int size;
  if ((size = zip_fread(file, *fileStr, fileStat->size)) == -1) {
    _epub_print_debug(epub, DEBUG_INFO, "%s - %s\n", 
                      filename, zip_strerror(arch));
  } else {
    (*fileStr)[size] = 0;
  }
  
  free(fileStat);

  if (zip_fclose(file) == -1) {
    _epub_print_debug(epub, DEBUG_INFO, "%s - %s\n", 
                      filename, zip_strerror(arch));
    return -1;
  }

  return size;
}

void _ocf_not_supported(struct ocf *ocf, char *filename) {
  if (_ocf_check_file(ocf, filename) > -1) 
    _epub_print_debug(ocf->epub, DEBUG_WARNING, 
                      "file %s exists but is not supported by this version", filename);
}

struct ocf *_ocf_parse(struct epub *epub, char *filename) {
  _epub_print_debug(epub, DEBUG_INFO, "building ocf struct");
  
  struct ocf *ocf = malloc(sizeof(struct ocf));
  ocf->epub = epub;
  ocf->roots = NULL;

  ocf->filename = malloc(sizeof(char)*(strlen(filename)+1));
  strcpy(ocf->filename, filename);
  if (! (ocf->arch = _ocf_open(ocf, ocf->filename)))
    return NULL;
  
  // Find the mime type
  _ocf_parse_mimetype(ocf);

  // Parse the container for roots
  _ocf_parse_container(ocf);
    
  // Unsupported files
   _ocf_not_supported(ocf, METAINFO_DIR "/" MANIFEST_FILENAME);
   _ocf_not_supported(ocf, METAINFO_DIR "/" METADATA_FILENAME);
   _ocf_not_supported(ocf, METAINFO_DIR "/" SIGNATURES_FILENAME);
   _ocf_not_supported(ocf, METAINFO_DIR "/" ENCRYPTION_FILENAME);
   _ocf_not_supported(ocf, METAINFO_DIR "/" RIGHTS_FILENAME);

  return ocf;
}

char *_ocf_root_by_type(struct ocf *ocf, char *type) {
  struct root *curr = ocf->roots;
  char *rootXml;

  while (curr) {
    if (strcmp(curr->mediatype, type) == 0) {
      _ocf_get_file(ocf, curr->fullpath, &rootXml);
      return rootXml;
    }
    
    curr = curr->next;
  }
  _epub_print_debug(ocf->epub, DEBUG_WARNING, "type %s for root not found", type);

  return NULL;
}
