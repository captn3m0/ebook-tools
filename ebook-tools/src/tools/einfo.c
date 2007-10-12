#include <stdlib.h>
#include <stdio.h>
#include <epub.h>


void quit(int code) {
  epub_cleanup();
  exit(code);
}


int main(int argc , char **argv) {
  epub *epub;

  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  if (! (epub = epub_open(argv[1], 3)))
    quit(1);
  
  epub_dump(epub);

  if (! epub_close(epub)) {
    quit(1);
  }

  quit(0);
  return 0;
}
