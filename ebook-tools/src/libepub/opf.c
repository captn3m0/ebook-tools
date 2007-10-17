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
      const xmlChar *name = xmlTextReaderConstName(reader);
      if (xmlStrcmp(name, (xmlChar *)"metadata") == 0)
        _opf_parse_metadata(opf, reader);
      else 
      if (xmlStrcmp(name, (xmlChar *)"manifest") == 0)
        _opf_parse_manifest(opf, reader);
      else 
      if (xmlStrcmp(name, (xmlChar *)"spine") == 0)
        _opf_parse_spine(opf, reader);
      else 
      if (xmlStrcmp(name, (xmlChar *)"guide") == 0)
        _opf_parse_guide(opf, reader);
      else 
      if (xmlStrcmp(name, (xmlChar *)"tours") == 0)
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
void _opf_init_metadata(struct opf *opf) {
  struct metadata *meta = malloc(sizeof(struct metadata));

  meta->id = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);  
  meta->title = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->creator = NewListAlloc(LIST, NULL, NULL, NULL);
  meta->contrib = NewListAlloc(LIST, NULL, NULL, NULL);
  meta->subject = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->publisher = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->description = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->date = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->type = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->format = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->source = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->lang = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->relation = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->coverage = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->rights = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);


  opf->metadata = meta;
}

void _opf_free_metadata(struct metadata *meta) {
  FreeList(meta->id, free);
  FreeList(meta->title, free);
  FreeList(meta->creator, (ListFreeFunc)_list_free_creator);
  FreeList(meta->contrib, (ListFreeFunc)_list_free_creator);
  FreeList(meta->subject, free);
  FreeList(meta->publisher, free);
  FreeList(meta->description, free);
  FreeList(meta->date, free);
  FreeList(meta->type, free);
  FreeList(meta->format, free);
  FreeList(meta->source, free);
  FreeList(meta->lang, free);
  FreeList(meta->relation, free);
  FreeList(meta->coverage, free);
  FreeList(meta->rights, free);
  free(meta);
}

void _opf_parse_metadata(struct opf *opf, xmlTextReaderPtr reader) {
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing metadata");
  int ret;

  // must have title, identifier and language
  _opf_init_metadata(opf);
  struct metadata *meta = opf->metadata;

  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcmp(xmlTextReaderConstName(reader),(xmlChar *)"metadata")) {

    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }
    
    const xmlChar *local = xmlTextReaderConstLocalName(reader);
    xmlChar *string = (xmlChar *)xmlTextReaderReadString(reader);

    if (xmlStrcmp(local, (xmlChar *)"identifier") == 0) {
        AddNode(meta->id, NewListNode(meta->id, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "identifier is %s", string); 

    } else if (xmlStrcmp(local, (xmlChar *)"title") == 0) {
        AddNode(meta->title, NewListNode(meta->title, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "title is %s", string);
        
    } else if (xmlStrcmp(local, (xmlChar *)"creator") == 0) {
        struct creator *new = malloc(sizeof(struct creator));
        new->name = string;
        new->fileAs = 
            xmlTextReaderGetAttributeNs(reader, (xmlChar *)"file-as", (xmlChar *)"opf");
        new->role = 
            xmlTextReaderGetAttributeNs(reader, (xmlChar *)"role", (xmlChar *)"opf");
        AddNode(meta->creator, NewListNode(meta->creator, new));       
        _epub_print_debug(opf->epub, DEBUG_INFO, "creator - %s: %s (%s)", 
                          new->role, new->name, new->fileAs);
        
    } else if (xmlStrcmp(local, (xmlChar *)"contributor") == 0) {
      struct creator *new = malloc(sizeof(struct creator));
      new->name = string;
      new->fileAs = 
          xmlTextReaderGetAttributeNs(reader, (xmlChar *)"file-as", (xmlChar *)"opf");
      new->role = 
          xmlTextReaderGetAttributeNs(reader, (xmlChar *)"role", (xmlChar *)"opf");
      AddNode(meta->contrib, NewListNode(meta->contrib, new));     
      _epub_print_debug(opf->epub, DEBUG_INFO, "contributor - %s: %s (%s)", 
                        new->role, new->name, new->fileAs);
      
    } else if (xmlStrcmp(local, (xmlChar *)"subject") == 0) {
        AddNode(meta->subject, NewListNode(meta->subject, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "subject is %s", string);
        
    } else if (xmlStrcmp(local, (xmlChar *)"publisher") == 0) {
        AddNode(meta->publisher, NewListNode(meta->publisher, string)); 
        _epub_print_debug(opf->epub, DEBUG_INFO, "publisher is %s", string); 
        
    } else if (xmlStrcmp(local, (xmlChar *)"description") == 0) {
        AddNode(meta->description, NewListNode(meta->description, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "description is %s", string);
        
    } else if (xmlStrcmp(local, (xmlChar *)"date") == 0) {
        AddNode(meta->date, NewListNode(meta->date, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "date is %s", string); 
        
    } else if (xmlStrcmp(local, (xmlChar *)"type") == 0) {
        AddNode(meta->type, NewListNode(meta->type, string));       
        _epub_print_debug(opf->epub, DEBUG_INFO, "type is %s", string);
        
    } else if (xmlStrcmp(local, (xmlChar *)"format") == 0) {
        AddNode(meta->format, NewListNode(meta->format, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "format is %s", string); 

    } else if (xmlStrcmp(local, (xmlChar *)"source") == 0) {
        AddNode(meta->source, NewListNode(meta->source, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "source is %s", string); 

    } else if (xmlStrcmp(local, (xmlChar *)"language") == 0) {
        AddNode(meta->lang, NewListNode(meta->lang, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "language is %s", string); 
      
    } else if (xmlStrcmp(local, (xmlChar *)"relation") == 0) {
        AddNode(meta->relation, NewListNode(meta->relation, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "relation is %s", string); 

    } else if (xmlStrcmp(local, (xmlChar *)"coverage") == 0) {
        AddNode(meta->coverage, NewListNode(meta->coverage, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "coverage is %s", string); 
    } else if (xmlStrcmp(local, (xmlChar *)"rights") == 0) {
        AddNode(meta->rights, NewListNode(meta->rights, string));
        _epub_print_debug(opf->epub, DEBUG_INFO, "rights is %s", string);
    } else if (string) {
        if (xmlStrcmp(local, (xmlChar *)"dc-metadata") != 0)
            _epub_print_debug(opf->epub, DEBUG_INFO,
                              "unsupported local %s: %s", local, string); 
        free(string);
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
    _epub_print_debug(opf->epub, DEBUG_WARNING, "toc not found (-)"); 
  else {
    _epub_print_debug(opf->epub, DEBUG_INFO, "toc is %s", toc); 
    free(toc);
}	
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
    if (idref)
	free(idref);

    if(linear)
	free(linear);

	
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
                      "manifest item %s href %s media-type %s", id, href, type);

    if (id)
        free(id);
    if (href)
        free(href);
    if (type)
        free(type);
    if (fallback)
        free(fallback);
    if (fbStyle)
        free(fbStyle);
            
            
            
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
  printf("Title(s): ");
  DumpList(opf->metadata->title, (ListDumpFunc)_list_dump_string);
  printf("Creator(s): ");
  DumpList(opf->metadata->creator, (ListDumpFunc)_list_dump_creator);
  printf("Identifier(s): ");
  DumpList(opf->metadata->id, (ListDumpFunc)_list_dump_string);
}

void _opf_close(struct opf *opf) {
  if (opf->metadata)
    _opf_free_metadata(opf->metadata);
  free(opf);
}
