#ifndef EPUB_H
#define EPUB_H 1

#include <epub_shared.h>
/** \struct epub is a private struct containting information about the epub file */
struct epub;

/** \struct eiterator is a private iterator struct */
struct eiterator;

#ifdef __cplusplus
extern "C" {
#endif // c++

  /** 
      This function accepts an epub filename. It then parses its information and
      returns it as an epub struct.
      
      @param filename the name of the file to open
      @param debug is the debug level (0=none, 1=errors, 2=warnings, 3=info)
      @return epub struct with the information of the file or NULL on error
      
  */
  struct epub *epub_open(const char *filename, int debug);
  
  /**
     This function sets the debug level to the given level.
     
     @param debug is the debug level (0=none, 1=errors, 2=warnings, 3=info)
  */
  void epub_set_debug(struct epub *epub, int debug);
  
  int epub_get_ocf_file(struct epub *epub, const char *filename, char **data);
  
  void epub_free_iterator(struct eiterator *it);

  /** 
      This function closes a given epub. It also frees the epub struct.
      So you can use it after calling this function.
      
      @param epub the struct of the epub to close.
  */
  int epub_close(struct epub *epub);
  
  /** 
      Debugging function dumping various file information.
      
      @param epub the struct of the epub to close.
  */
  void epub_dump(struct epub *epub);
  
  unsigned char **epub_get_metadata(struct epub *epub, enum epub_metadata type,
                                    int *size);

  struct eiterator *epub_get_iterator(struct epub *epub, 
                                      enum eiterator_type type, int opt);

  char *epub_it_get_next(struct eiterator *it);

  char *epub_it_get_curr(struct eiterator *it);

  /**
     Cleans up after the library. Call this when you are done with the library. 
  */
  void epub_cleanup();

#ifdef __cplusplus
}
#endif // c++

#endif // EPUB_H
