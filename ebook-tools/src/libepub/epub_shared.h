#ifndef EPUB_SHARED_H
#define EPUB_SHARED_H 1

enum epub_metadata {
  EPUB_ID,
  EPUB_TITLE,
  EPUB_CREATOR,
  EPUB_CONTRIB,
  EPUB_SUBJECT,
  EPUB_PUBLISHER,
  EPUB_DESCRIPTION,
  EPUB_DATE,
  EPUB_TYPE,
  EPUB_FORMAT,
  EPUB_SOURCE,
  EPUB_LANG,
  EPUB_RELATION,
  EPUB_COVERAGE,
  EPUB_RIGHTS,
  EPUB_META,
};

enum eiterator_type {
  EITERATOR_SPINE,
  EITERATOR_LINEAR,
  EITERATOR_NONLINEAR,
  //  EITERATOR_TOUR,
};

enum titerator_type {
  TITERATOR_TOC,
  TITERATOR_GUIDE,
  TITERATOR_PAGES,
  TITERATOR_SPINE,
};

#endif
