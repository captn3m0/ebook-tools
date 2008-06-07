#ifndef _LIBEPUB_VERSION_H_
#define _LIBEPUB_VERSION_H_

#define LIBEPUB_VERSION_STRING "0.1.1"
#define LIBEPUB_VERSION_MAJOR 0
#define LIBEPUB_VERSION_MINOR 1
#define LIBEPUB_VERSION_RELEASE 1
#define LIBEPUB_MAKE_VERSION( a,b,c ) (((a) << 16) | ((b) << 8) | (c))

#define LIBEPUB_VERSION \
  LIBEPUB_MAKE_VERSION(LIBEPUB_VERSION_MAJOR,LIBEPUB_VERSION_MINOR,LIBEPUB_VERSION_RELEASE)

#define LIBEPUB_IS_VERSION(a,b,c) ( LIBEPUB_VERSION >= LIBEPUB_MAKE_VERSION(a,b,c) )

#endif
