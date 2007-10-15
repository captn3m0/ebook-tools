#include "epublib.h"

struct opf *_opf_parse(struct epub *epub, char *opfStr) {
  _epub_print_debug(epub, DEBUG_INFO, "building opf struct");
  
  struct opf *opf = malloc(sizeof(struct opf));
  opf->epub = epub;
  
  
  xmlTextReaderPtr reader;
  int ret;
  
  reader = xmlReaderForMemory(opfStr, strlen(opfStr), 
                              "OPF", NULL, 0);
   if (reader != NULL) {
    ret = xmlTextReaderRead(reader);
    while (ret == 1) {
      if (xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"metadata") == 0)
        _opf_parse_metadata(opf, reader);
      else 
      if (xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"manifest") == 0)
        _opf_parse_manifest(opf, reader);
      else 
      if (xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"spine") == 0)
        _opf_parse_spine(opf, reader);
      else 
      if (xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"guide") == 0)
        _opf_parse_guide(opf, reader);
      else 
      if (xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"tours") == 0)
        _opf_parse_tours(opf, reader);
      
     ret = xmlTextReaderRead(reader);
    }

    xmlFreeTextReader(reader);
    if (ret != 0) {
      _epub_print_debug(opf->epub, DEBUG_ERROR, "failed to parse OPF");
      return NULL;
    }
   } else {
     _epub_print_debug(opf->epub, DEBUG_ERROR, "unable to open OPF");
     return NULL;
   }
   
   //FIXME: _opf_parse_toc(opf->toc);
   
   return opf;
}

void _opf_parse_metadata(struct opf *opf, xmlTextReaderPtr reader) {
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing metadata");
  int ret;
  // must have title, identifier and language
  
  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"metadata")) {

    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }
    
    const xmlChar *local = xmlTextReaderConstLocalName(reader);
    if (xmlStrcmp(local, (xmlChar *)"title") == 0) {
      _epub_print_debug(opf->epub, DEBUG_INFO, "title is %s", 
                        xmlTextReaderReadString(reader));
    } else if (xmlStrcmp(local, (xmlChar *)"identifier") == 0) {
      _epub_print_debug(opf->epub, DEBUG_INFO, "identifier is %s", 
                        xmlTextReaderReadString(reader));  
    } else if (xmlStrcmp(local, (xmlChar *)"language") == 0) {
      _epub_print_debug(opf->epub, DEBUG_INFO, "language is %s", 
                        xmlTextReaderReadString(reader));  
    }
    

    ret = xmlTextReaderRead(reader);
  }
}

void _opf_parse_spine(struct opf *opf, xmlTextReaderPtr reader) {
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing spine");

  int ret;
  xmlChar *toc, *idref, *linear;
  toc = xmlTextReaderGetAttribute(reader, (xmlChar *)"toc");
  
  if (! toc) 
    _epub_print_debug(opf->epub, DEBUG_WARNING, "toc not found (-)", 
                      xmlTextReaderReadString(reader));  
  else 
    _epub_print_debug(opf->epub, DEBUG_INFO, "toc is %s", 
                      xmlTextReaderReadString(reader));
  
  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"spine")) {
  
    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }
     idref = xmlTextReaderGetAttribute(reader, (xmlChar *)"idref");
     linear = xmlTextReaderGetAttribute(reader, (xmlChar *)"linear");
     // decide what to do with non linear items
     _epub_print_debug(opf->epub, DEBUG_INFO, "found item %s", idref);
    
    ret = xmlTextReaderRead(reader);
  }
}

void _opf_parse_manifest(struct opf *opf, xmlTextReaderPtr reader) {
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing manifest");

  int ret;
  xmlChar *id,*href,*type, *fallback,*fbStyle;
  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"manifest")) {
 
    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }
    // FIXME: what to do with required-namespace and required-modules 
    id = xmlTextReaderGetAttribute(reader, (xmlChar *)"id");
    href = xmlTextReaderGetAttribute(reader, (xmlChar *)"href");
    type = xmlTextReaderGetAttribute(reader, (xmlChar *)"media-type");
    fallback = xmlTextReaderGetAttribute(reader, (xmlChar *)"fallback");
    fbStyle = xmlTextReaderGetAttribute(reader, (xmlChar *)"fallback-style");

    _epub_print_debug(opf->epub, DEBUG_INFO, 
                      "manifest item %s href %s media-type %s", id, href, type, 
                      xmlTextReaderReadString(reader));  

    ret = xmlTextReaderRead(reader);
  }
}

void _opf_parse_guide(struct opf *opf, xmlTextReaderPtr reader) {
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing guides");

  int ret;

  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"guides")) {

    ret = xmlTextReaderRead(reader);
  }
}

void _opf_parse_tours(struct opf *opf, xmlTextReaderPtr reader) {
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing tours");

  int ret;

  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"tours")) {

    ret = xmlTextReaderRead(reader);
  }
}

void _opf_dump(struct opf *opf) {
  
}

void _opf_close(struct opf *opf) {

  free(opf);
}
