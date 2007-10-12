#include "epublib.h"
#include <stdarg.h>

struct epub *epub_open(char *filename, int debug) {
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

int epub_close(struct epub *epub) {
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

void epub_dump(struct epub *epub) {
  _ocf_dump(epub->ocf);

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

