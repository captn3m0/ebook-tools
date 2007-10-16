#ifndef EPUBLIB_H
#define EPUBLIB_H 1
// generally needed includes
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// For opening the zip file
#include <zip.h>
#include <zlib.h>

// For parsing xml
#include <libxml/xmlreader.h>

// For list stuff
#include "linklist.h"
 
///////////////////////////////////////////////////////////
// OCF definions
///////////////////////////////////////////////////////////
#define METAINFO_DIR "META-INF"
#define MIMETYPE_FILENAME "mimetype"
#define CONTAINER_FILENAME "container.xml"
#define MANIFEST_FILENAME  "manifest.xml"
#define METADATA_FILENAME  "metadata.xml"
#define SIGNATURES_FILENAME "signatures.xml"
#define ENCRYPTION_FILENAME "encryption.xml"
#define RIGHTS_FILENAME "rights.xml"

// A linked list of OCF roots 
struct root {
  xmlChar *mediatype; // media type (mime)
  xmlChar *fullpath; // full path to the root
};


struct ocf {
  char *filename; // The ebook filename
  struct zip *arch; // The epub zip
  char *mimetype; // For debugging 
  listPtr roots; // list of OCF roots
  struct epub *epub; // back pointer
};

//struct metadata {
//};

struct opf {
  struct epub *epub;
  //  struct metadata *meta;
  char *name;

};

struct epuberr {
  char lastStr[1025];
  int len;
}; 

// general structs
struct epub {
  struct ocf *ocf;
  struct opf *opf;
  struct epuberr *error;
  int debug;

};

enum {
  DEBUG_NONE,
  DEBUG_ERROR,
  DEBUG_WARNING,
  DEBUG_INFO,
  DEBUG_VERBOSE
};


// Ocf functions
struct ocf *_ocf_parse(struct epub *epub, char *filename);
void _ocf_dump(struct ocf *ocf);
void _ocf_close(struct ocf *ocf);
struct zip *_ocf_open(struct ocf *ocf, char *fileName);
int _ocf_get_file(struct ocf *ocf, const char *filename, char **fileStr);
int _ocf_check_file(struct ocf *ocf, const char *filename);
char *_ocf_root_by_type(struct ocf *ocf, char *type);

// Parsing ocf
int _ocf_parse_container(struct ocf *ocf);
int _ocf_parse_mimetype(struct ocf *ocf);

// parsing opf
struct opf *_opf_parse(struct epub *epub, char *opfStr);
void _opf_dump(struct opf *opf);
void _opf_close(struct opf *opf);

void _opf_parse_metadata(struct opf *opf, xmlTextReaderPtr reader);
void _opf_parse_spine(struct opf *opf, xmlTextReaderPtr reader);
void _opf_parse_manifest(struct opf *opf, xmlTextReaderPtr reader);
void _opf_parse_guide(struct opf *opf, xmlTextReaderPtr reader);
void _opf_parse_tours(struct opf *opf, xmlTextReaderPtr reader);

// epub functions
struct epub *epub_open(char *filename, int debug);
void _epub_print_debug(struct epub *epub, int debug, char *format, ...);
char *epub_last_errStr(struct epub *epub);
void _epub_print_debug(struct epub *epub, int debug, char *format, ...);

// List operations
void list_free_root(struct root *data);
int list_cmp_root_by_mediatype(struct root *root1, struct root *root2);
void list_dump_root(struct root *root);

#endif /* epublib_h */ 
