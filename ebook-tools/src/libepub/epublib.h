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

// An OCF root 
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

struct creator {
  xmlChar *name;
  xmlChar *fileAs;
  xmlChar *role;
};

struct metadata {
  listPtr id;
  listPtr title;
  listPtr creator;
  listPtr contrib;
  listPtr subject;
  listPtr publisher;
  listPtr description;
  listPtr date;
  listPtr type;
  listPtr format;
  listPtr source;
  listPtr lang;
  listPtr relation;
  listPtr coverage;
  listPtr rights;
};

struct manifest {
  xmlChar *namespace; 
  xmlChar *modules; 
  xmlChar *id;
  xmlChar *href;
  xmlChar *type;
  xmlChar *fallback;
  xmlChar *fbStyle;

};
    
struct guide {
  xmlChar *type;
  xmlChar *title;
  xmlChar *href;
};

struct site {
  xmlChar *title;
  xmlChar *href;
};

struct tour {
  xmlChar *id;
  xmlChar *title;
  listPtr sites;
};

struct toc {
  xmlChar id;
};

struct spine {
  xmlChar *idref;
  int linear; //bool
};

struct opf {
  char *name;
  xmlChar *tocName;
  struct epub *epub;
  struct metadata *metadata;
  struct toc *toc; // must in opf 2.0
  listPtr manifest;
  listPtr spine;
  int linearCount;
    
  // might be NULL
  listPtr guide;
  listPtr tours;
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
void _list_free_root(struct root *data);
void _list_free_creator(struct creator *data);
void _list_free_spine(struct spine *spine);
void _list_free_manifest(struct manifest *manifest);
void _list_free_guide(struct guide *guide);
void _list_free_tours(struct tour *tour);

int _list_cmp_root_by_mediatype(struct root *root1, struct root *root2);

void _list_dump_root(struct root *root);
void _list_dump_string(char *string);
void _list_dump_creator(struct creator *data);
void _list_dump_spine(struct spine *spine);
void _list_dump_guide(struct guide *guide);
void _list_dump_tour(struct tour *tour);

#endif /* epublib_h */ 
