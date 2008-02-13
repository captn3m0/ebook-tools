#include "epublib.h"
#include <stdarg.h>

struct epub *epub_open(const char *filename, int debug) {
  struct epub *epub = malloc(sizeof(struct epub));
  epub->ocf = NULL;
  epub->opf = NULL;

  epub->error = malloc(sizeof(struct epuberr));
  epub->debug = debug;
  _epub_print_debug(epub, DEBUG_INFO, "opening %s", filename);
  
  LIBXML_TEST_VERSION;
  
  if (! (epub->ocf = _ocf_parse(epub, filename))) {
    epub_close(epub);
    return NULL;
  }

  char *opfStr = _ocf_root_by_type(epub->ocf, "application/oebps-package+xml");
  if (!opfStr) {
    epub_close(epub);
    return NULL;
  }
  epub->opf = _opf_parse(epub, opfStr);
  free(opfStr);

  return epub;
}

xmlChar *_getXmlStr(void *str) {

  return xmlStrdup((xmlChar *)str);
}

xmlChar *_getRoleStr(void *creator) {
  struct creator *data = (struct creator *)creator;
  xmlChar buff[10000];
  xmlStrPrintf(buff, 10000, "%s: %s(%s)", 
               ((data->role)?data->role:(xmlChar *)"Author"), 
               data->name, ((data->fileAs)?data->fileAs:data->name));
  
  return xmlStrdup(buff);
}

xmlChar **epub_get_metadata(struct epub *epub, enum epub_metadata type, 
                            int *size) {
  xmlChar **data = NULL;
  listPtr list;
  xmlChar *(*getStr)(void *);
  int i;

  switch(type) {
  case EPUB_ID:
    list = epub->opf->metadata->id;
    getStr = _getXmlStr;
    break;
  case EPUB_TITLE:
    list = epub->opf->metadata->title;
    getStr = _getXmlStr;
    break;
  case EPUB_SUBJECT:
    list = epub->opf->metadata->subject;
    getStr = _getXmlStr;
    break;
  case EPUB_PUBLISHER:
    list = epub->opf->metadata->publisher;
    getStr = _getXmlStr;
    break;
  case EPUB_DESCRIPTION:
    list = epub->opf->metadata->description;
    getStr = _getXmlStr;
    break;
  case EPUB_DATE:
    list = epub->opf->metadata->date;
    getStr = _getXmlStr;
    break;
  case EPUB_TYPE:
    list = epub->opf->metadata->type;
    getStr = _getXmlStr;
    break;
  case EPUB_FORMAT:
    list = epub->opf->metadata->format;
    getStr = _getXmlStr;
    break;
  case EPUB_SOURCE:
    list = epub->opf->metadata->source;
    getStr = _getXmlStr;
    break;
  case EPUB_LANG:
    list = epub->opf->metadata->lang;
    getStr = _getXmlStr;
    break;
  case EPUB_RELATION:
    list = epub->opf->metadata->relation;
    getStr = _getXmlStr;
    break;
  case EPUB_COVERAGE:
    list = epub->opf->metadata->coverage;
    getStr = _getXmlStr;
    break;
  case EPUB_RIGHTS:
    list = epub->opf->metadata->rights;
    getStr = _getXmlStr;
    break;

  case EPUB_CREATOR:
    list = epub->opf->metadata->creator;
    getStr = _getRoleStr;
    break;
  case EPUB_CONTRIB:
    list = epub->opf->metadata->contrib;
    getStr = _getRoleStr;
    break;
  }

  *size = list->Size;
  if (! list->Size)
    return NULL;

  data = malloc(list->Size * sizeof(xmlChar *));

  data[0] = getStr(GetNode(list));
  for (i=1;i<list->Size;i++) {
    data[i] = getStr(NextNode(list));
  }

  return data;
}

struct eiterator *epub_get_iterator(struct epub *epub, 
                                    enum eiterator_type type, int opt) {

  struct eiterator *it = malloc(sizeof(struct eiterator));
  
  it->type = type;
  it->epub = epub;
  it->opt = opt;
  it->cache = NULL;

  switch (type) {
  case EITERATOR_NONLINEAR:
    it->curr = epub->opf->spine->Head;
    break;
  case EITERATOR_LINEAR:
    
    break;
  }


  return it;
}
char *epub_it_get_curr(struct eiterator *it) {

  if (!it->curr)
    return NULL;

  if (!it->cache) {
    void *data;
       
    switch (it->type) {
      struct manifest *tmp;
      
    case EITERATOR_NONLINEAR:
      data = GetNodeData(it->curr);
      tmp = _opf_manifest_get_by_id(it->epub->opf, 
                                    ((struct spine *)data)->idref);
      _ocf_get_data_file(it->epub->ocf, tmp->href, &(it->cache));
      break;

    case EITERATOR_LINEAR:
      break;
    }
  }
  
  return it->cache;
}
char *epub_it_get_next(struct eiterator *it) {
  
  if (it->cache) {
    free(it->cache);
    it->cache = NULL;
  }

  if (!it->curr)
    return NULL;

  switch (it->type) {
   
  case EITERATOR_NONLINEAR:
    it->curr = it->curr->Next;
    break;

  case EITERATOR_LINEAR:
    break;
  }
  
  return epub_it_get_curr(it);
}

int epub_close(struct epub *epub) {
  if (epub->error) 
    free(epub->error);
 
  if (epub->ocf)
    _ocf_close(epub->ocf);

  if (epub->opf)
    _opf_close(epub->opf);

  if (epub)
    free(epub);

  
  return 1;
}

void epub_set_debug(struct epub *epub, int debug) {
  epub->debug = debug;
}

void _epub_print_debug(struct epub *epub, int debug, char *format, ...) {
  va_list ap;
  char strerr[1025];

  va_start(ap, format);

  vsnprintf(strerr, 1024, format, ap);
  strerr[1024] = 0;
  
  if (debug == DEBUG_ERROR) {
    epub->error->len = strlen(strerr);
    strcpy(epub->error->lastStr, strerr);
  }

  if (epub->debug >= debug) {
    fprintf(stderr, "libepub ");
    switch(debug) {
    case DEBUG_ERROR: 
      
      fprintf(stderr, "(EE)");
    break;
    case DEBUG_WARNING:
      fprintf(stderr, "(WW)");
      break;
    case DEBUG_INFO:
      fprintf(stderr, "(II)");
      break;
    case DEBUG_VERBOSE:
      fprintf(stderr, "(VV)");
      break;
    }
    fprintf(stderr, ": \t%s\n" , strerr);
  }
  va_end(ap);
}

int epub_get_ocf_file(struct epub *epub, char *filename, char **data) {
  return _ocf_get_file(epub->ocf, filename, data);

}

void epub_dump(struct epub *epub) {
  _ocf_dump(epub->ocf);
  _opf_dump(epub->opf);
}

void epub_cleanup() {
  xmlCleanupParser();
}

char *epub_last_errStr(struct epub *epub) {
  char *errStr = epub->error->lastStr;
  char *res = malloc(epub->error->len +1);
  strcpy(errStr, res);

  return res;
}

