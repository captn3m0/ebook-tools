#include "epublib.h"

struct opf *_opf_parse(struct epub *epub, char *opfStr) {
  _epub_print_debug(epub, DEBUG_INFO, "building opf struct");
  
  struct opf *opf = malloc(sizeof(struct opf));
  opf->epub = epub;
  opf->guide = NULL;
  opf->tours = NULL;
  
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

   return opf;
}

xmlChar *_get_possible_namespace(xmlTextReaderPtr reader, 
                                 const xmlChar * localName, 
                                 const xmlChar * namespaceURI) 
{
  if (xmlTextReaderGetAttributeNs(reader, localName, namespaceURI))
    return xmlTextReaderGetAttributeNs(reader, localName, namespaceURI);
  else
    return xmlTextReaderGetAttribute(reader, localName);
}

void _opf_init_metadata(struct opf *opf) {
  struct metadata *meta = malloc(sizeof(struct metadata));

  meta->id = NewListAlloc(LIST, NULL, NULL, NULL);  
  meta->title = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->creator = NewListAlloc(LIST, NULL, NULL, NULL);
  meta->contrib = NewListAlloc(LIST, NULL, NULL, NULL);
  meta->subject = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->publisher = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->description = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->date = NewListAlloc(LIST, NULL, NULL, NULL);
  meta->type = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->format = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->source = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->lang = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->relation = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->coverage = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->rights = NewListAlloc(LIST, NULL, NULL, (NodeCompareFunc)StringCompare);
  meta->meta = NewListAlloc(LIST, NULL, NULL, NULL);

  opf->metadata = meta;
}

void _opf_free_metadata(struct metadata *meta) {
  FreeList(meta->id, (ListFreeFunc)_list_free_id);
  FreeList(meta->title, free);
  FreeList(meta->creator, (ListFreeFunc)_list_free_creator);
  FreeList(meta->contrib, (ListFreeFunc)_list_free_creator);
  FreeList(meta->subject, free);
  FreeList(meta->publisher, free);
  FreeList(meta->description, free);
  FreeList(meta->date, (ListFreeFunc)_list_free_date);
  FreeList(meta->type, free);
  FreeList(meta->format, free);
  FreeList(meta->source, free);
  FreeList(meta->lang, free);
  FreeList(meta->relation, free);
  FreeList(meta->coverage, free);
  FreeList(meta->rights, free);
  FreeList(meta->meta, (ListFreeFunc)_list_free_meta);
  free(meta);
}

void _opf_parse_metadata(struct opf *opf, xmlTextReaderPtr reader) {
  int ret;
  struct metadata *meta;
  
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing metadata");
  
  // must have title, identifier and language
  _opf_init_metadata(opf);
  meta = opf->metadata;
  
  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"metadata")) {

    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }
    
    const xmlChar *local = xmlTextReaderConstLocalName(reader);
    xmlChar *string = (xmlChar *)xmlTextReaderReadString(reader);

    if (xmlStrcasecmp(local, (xmlChar *)"identifier") == 0) {
      struct id *new = malloc(sizeof(struct id));
      new->string = string;
      new->scheme = _get_possible_namespace(reader, (xmlChar *)"scheme",
                                                (xmlChar *)"opf");
      new->id = xmlTextReaderGetAttribute(reader, (xmlChar *)"id");
      
      AddNode(meta->id, NewListNode(meta->id, new));
      _epub_print_debug(opf->epub, DEBUG_INFO, "identifier %s(%s) is: %s", 
                        new->id, new->scheme, new->string);
    } else if (xmlStrcasecmp(local, (xmlChar *)"title") == 0) {
      AddNode(meta->title, NewListNode(meta->title, string));
      _epub_print_debug(opf->epub, DEBUG_INFO, "title is %s", string);
        
    } else if (xmlStrcasecmp(local, (xmlChar *)"creator") == 0) {
      struct creator *new = malloc(sizeof(struct creator));
      new->name = string;
      new->fileAs = 
        _get_possible_namespace(reader, (xmlChar *)"file-as",
                                    (xmlChar *)"opf");
      new->role = 
        _get_possible_namespace(reader, (xmlChar *)"role",
                                    (xmlChar *)"opf");
      AddNode(meta->creator, NewListNode(meta->creator, new));       
      _epub_print_debug(opf->epub, DEBUG_INFO, "creator - %s: %s (%s)", 
                        new->role, new->name, new->fileAs);
        
    } else if (xmlStrcasecmp(local, (xmlChar *)"contributor") == 0) {
      struct creator *new = malloc(sizeof(struct creator));
      new->name = string;
      new->fileAs = 
        _get_possible_namespace(reader, (xmlChar *)"file-as",
                                    (xmlChar *)"opf");
      new->role = 
        _get_possible_namespace(reader, (xmlChar *)"role",
                                    (xmlChar *)"opf");
      AddNode(meta->contrib, NewListNode(meta->contrib, new));     
      _epub_print_debug(opf->epub, DEBUG_INFO, "contributor - %s: %s (%s)", 
                        new->role, new->name, new->fileAs);
      
    } else if (xmlStrcasecmp(local, (xmlChar *)"meta") == 0) {
      struct meta *new = malloc(sizeof(struct meta));
      new->name = xmlTextReaderGetAttribute(reader, (xmlChar *)"name");
      new->content = xmlTextReaderGetAttribute(reader, (xmlChar *)"content");
      
      AddNode(meta->meta, NewListNode(meta->meta, new));
      if (string)
        free(string);
      _epub_print_debug(opf->epub, DEBUG_INFO, "meta is %s: %s", 
                        new->name, new->content); 
    } else if (xmlStrcasecmp(local, (xmlChar *)"date") == 0) {
      struct date *new = malloc(sizeof(struct date));
      new->date = string;
      new->event = _get_possible_namespace(reader, (xmlChar *)"event",
                                               (xmlChar *)"opf");
      AddNode(meta->date, NewListNode(meta->date, new));
      _epub_print_debug(opf->epub, DEBUG_INFO, "date is %s: %s", 
                        new->event, new->date); 
        
    } else if (xmlStrcasecmp(local, (xmlChar *)"subject") == 0) {
      AddNode(meta->subject, NewListNode(meta->subject, string));
      _epub_print_debug(opf->epub, DEBUG_INFO, "subject is %s", string);
        
    } else if (xmlStrcasecmp(local, (xmlChar *)"publisher") == 0) {
      AddNode(meta->publisher, NewListNode(meta->publisher, string)); 
      _epub_print_debug(opf->epub, DEBUG_INFO, "publisher is %s", string); 
        
    } else if (xmlStrcasecmp(local, (xmlChar *)"description") == 0) {
      AddNode(meta->description, NewListNode(meta->description, string));
      _epub_print_debug(opf->epub, DEBUG_INFO, "description is %s", string);
        
    } else if (xmlStrcasecmp(local, (xmlChar *)"type") == 0) {
      AddNode(meta->type, NewListNode(meta->type, string));       
      _epub_print_debug(opf->epub, DEBUG_INFO, "type is %s", string);
        
    } else if (xmlStrcasecmp(local, (xmlChar *)"format") == 0) {
      AddNode(meta->format, NewListNode(meta->format, string));
      _epub_print_debug(opf->epub, DEBUG_INFO, "format is %s", string); 

    } else if (xmlStrcasecmp(local, (xmlChar *)"source") == 0) {
      AddNode(meta->source, NewListNode(meta->source, string));
      _epub_print_debug(opf->epub, DEBUG_INFO, "source is %s", string); 

    } else if (xmlStrcasecmp(local, (xmlChar *)"language") == 0) {
      AddNode(meta->lang, NewListNode(meta->lang, string));
      _epub_print_debug(opf->epub, DEBUG_INFO, "language is %s", string); 
      
    } else if (xmlStrcasecmp(local, (xmlChar *)"relation") == 0) {
      AddNode(meta->relation, NewListNode(meta->relation, string));
      _epub_print_debug(opf->epub, DEBUG_INFO, "relation is %s", string); 

    } else if (xmlStrcasecmp(local, (xmlChar *)"coverage") == 0) {
      AddNode(meta->coverage, NewListNode(meta->coverage, string));
      _epub_print_debug(opf->epub, DEBUG_INFO, "coverage is %s", string); 
    } else if (xmlStrcasecmp(local, (xmlChar *)"rights") == 0) {
      AddNode(meta->rights, NewListNode(meta->rights, string));
      _epub_print_debug(opf->epub, DEBUG_INFO, "rights is %s", string);
    } else if (string) {
      if (xmlStrcasecmp(local, (xmlChar *)"dc-metadata") != 0 &&
          xmlStrcasecmp(local, (xmlChar *)"x-metadata") != 0)
        _epub_print_debug(opf->epub, DEBUG_INFO,
                          "unsupported local %s: %s", local, string); 
      free(string);
    }

    ret = xmlTextReaderRead(reader);
  }
}

struct toc *_opf_init_toc() {
  
  struct toc *toc = malloc(sizeof(struct toc));
  
  if (! toc)
    return NULL;
  
  toc->navMap = NewListAlloc(LIST, NULL, NULL, NULL); 
  toc->pageList = NewListAlloc(LIST, NULL, NULL, NULL);
  toc->navList = NewListAlloc(LIST, NULL, NULL, NULL);;
  toc->playOrder = NewListAlloc(LIST, NULL, NULL, 
                                (NodeCompareFunc)_list_cmp_toc_by_playorder);

  return toc;
}

void _opf_free_toc(struct toc *toc) {
  
  FreeList(toc->navMap, (ListFreeFunc)_list_free_toc);
  FreeList(toc->pageList, (ListFreeFunc)_list_free_toc);
  FreeList(toc->navList, (ListFreeFunc)_list_free_toc);
  FreeList(toc->playOrder, (ListFreeFunc)_list_free_toc);

  free(toc);

}
          
void _opf_parse_navlabel(struct tocItem *item, xmlTextReaderPtr reader) {
  int ret;

  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"navLabel")) {
    if (xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"text")) {
      item->label = (xmlChar *)xmlTextReaderReadString(reader);
    }
    ret = xmlTextReaderRead(reader);
  }
}

void _opf_parse_navinfo(struct tocItem *item, xmlTextReaderPtr reader) {
  int ret;

  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"navInfo")) {
    if (xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"text")) {
      //      item->info = (xmlChar *)xmlTextReaderReadString(reader);
    }
    ret = xmlTextReaderRead(reader);
  }
}

void _opf_parse_navmap(struct opf *opf, xmlTextReaderPtr reader) {
  int ret;
  int depth = 0;
  struct tocItem *item;

  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing nav map");

  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"navMap")) {

    if (xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"navPoint")) {
      if (xmlTextReaderNodeType(reader) == 1) {
        depth++;
        item = malloc(sizeof(struct tocItem));
        item->depth = depth;
        item->id = xmlTextReaderGetAttribute(reader, (xmlChar *)"id");
        if (xmlTextReaderGetAttribute(reader, (xmlChar *)"playOrder")) 
          item->playOrder = 
            *xmlTextReaderGetAttribute(reader, (xmlChar *)"playOrder");

      } else if (xmlTextReaderNodeType(reader) == 15) {
        AddNode(opf->toc->navMap, NewListNode(opf->toc->navMap, item));
        depth--;
      }
    }
  
    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }

    if (xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"navLabel")) {
      _opf_parse_navlabel(item, reader);
    } else if 
        (xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"content")) {
      item->src = xmlTextReaderGetAttribute(reader, (xmlChar *)"src");
    }
  }
}

void _opf_parse_navlist(struct opf *opf, xmlTextReaderPtr reader) {
  int ret;
  struct tocItem *item;
  int inList;
  
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing nav list");

  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"navList")) {

    if (xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"navPoint")) {
      if (xmlTextReaderNodeType(reader) == 1) {
      } else if (xmlTextReaderNodeType(reader) == 15) {
      }
    }
  

    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }

    if (xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"navLabel")) {
      _opf_parse_navlabel(item, reader);
    }
  }
}

void _opf_parse_pagelist(struct opf *opf, xmlTextReaderPtr reader) {
  int ret;
  struct tocItem *item;

  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing page list");

  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"pageList")) {

    item = malloc(sizeof(struct tocItem));
    item->depth = 1;

    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }

    if (xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"navLabel")) {
      _opf_parse_navlabel(item, reader);
    }
  }
}

void _opf_parse_toc(struct opf *opf, char *tocStr, int size) {

  xmlTextReaderPtr reader;
  int ret;

  _epub_print_debug(opf->epub, DEBUG_INFO, "building toc");
  
  opf->toc = _opf_init_toc();
  
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing toc");
  
  reader = xmlReaderForMemory(tocStr, size, "TOC", NULL, 0);
  
  if (reader != NULL) {
    ret = xmlTextReaderRead(reader);
    
    while (ret == 1) {
      
      const xmlChar *name = xmlTextReaderConstName(reader);
      if (xmlStrcasecmp(name, (xmlChar *)"navList") == 0)
        _opf_parse_navlist(opf, reader);
      else 
      if (xmlStrcasecmp(name, (xmlChar *)"navMap") == 0)
        _opf_parse_navmap(opf, reader);
      else 
      if (xmlStrcasecmp(name, (xmlChar *)"pageList") == 0)
        _opf_parse_pagelist(opf, reader);

      ret = xmlTextReaderRead(reader);
    }

    xmlFreeTextReader(reader);
    if (ret != 0) {
      _epub_print_debug(opf->epub, DEBUG_ERROR, "failed to parse toc");
    }
  } else {
    _epub_print_debug(opf->epub, DEBUG_ERROR, "unable to open toc reader");
  }
}      

void _opf_parse_spine(struct opf *opf, xmlTextReaderPtr reader) {
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing spine");

  int ret;
  xmlChar *linear;
  
  opf->spine = NewListAlloc(LIST, NULL, NULL, NULL); 
  opf->tocName = xmlTextReaderGetAttribute(reader, (xmlChar *)"toc");
  
  if (opf->tocName) { 
    char *tocStr;
    struct manifest *item;
    int size;

    _epub_print_debug(opf->epub, DEBUG_INFO, "toc is %s", opf->tocName);
    
    item = _opf_manifest_get_by_id(opf, opf->tocName);
    size = _ocf_get_data_file(opf->epub->ocf, (char *)item->href, &tocStr);
    
    if (size <= 0) 
      _epub_print_debug(opf->epub, DEBUG_ERROR, "unable to open toc file");

    //_opf_parse_toc(opf, tocStr, size);
    opf->toc = NULL;
    free(tocStr);

  } else {
    _epub_print_debug(opf->epub, DEBUG_WARNING, "toc not found (-)"); 
    opf->toc = NULL;
  }

  
  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcasecmp(xmlTextReaderConstName(reader), (xmlChar *)"spine")) {
  
    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }

    struct spine *item = malloc(sizeof(struct spine));

    item->idref = xmlTextReaderGetAttribute(reader, (xmlChar *)"idref");
    linear = xmlTextReaderGetAttribute(reader, (xmlChar *)"linear");
    if (linear) {
        if (xmlStrcasecmp(linear, (xmlChar *)"no") == 0) {
            item->linear = 0;
        }
    } else {
        item->linear = 1;
        opf->linearCount++;
    }

    if(linear)
        free(linear);
    
     AddNode(opf->spine, NewListNode(opf->spine, item));
     
    // decide what to do with non linear items
    _epub_print_debug(opf->epub, DEBUG_INFO, "found item %s", item->idref);
    
    ret = xmlTextReaderRead(reader);
  }
}

void _opf_parse_manifest(struct opf *opf, xmlTextReaderPtr reader) {
  int ret;
  
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing manifest");

  opf->manifest = NewListAlloc(LIST, NULL, NULL, 
                               (NodeCompareFunc)_list_cmp_manifest_by_id );

  ret = xmlTextReaderRead(reader);

  while (ret == 1 && 
         xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"manifest")) {
    struct manifest *item;

    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }

    item = malloc(sizeof(struct manifest));

    item->id = xmlTextReaderGetAttribute(reader, (xmlChar *)"id");
    item->href = xmlTextReaderGetAttribute(reader, (xmlChar *)"href");
    item->type = xmlTextReaderGetAttribute(reader, (xmlChar *)"media-type");
    item->fallback = xmlTextReaderGetAttribute(reader, (xmlChar *)"fallback");
    item->fbStyle = 
      xmlTextReaderGetAttribute(reader, (xmlChar *)"fallback-style");
    item->nspace = 
      xmlTextReaderGetAttribute(reader, (xmlChar *)"required-namespace");
    item->modules = 
      xmlTextReaderGetAttribute(reader, (xmlChar *)"required-modules");
    
    _epub_print_debug(opf->epub, DEBUG_INFO, 
                      "manifest item %s href %s media-type %s", 
                      item->id, item->href, item->type);

    AddNode(opf->manifest, NewListNode(opf->manifest, item));

    ret = xmlTextReaderRead(reader);
  }
}

struct manifest *_opf_manifest_get_by_id(struct opf *opf, xmlChar* id) {
  struct manifest data;
  data.id = id;
  
  return FindNode(opf->manifest, &data);
  
}

void _opf_parse_guide(struct opf *opf, xmlTextReaderPtr reader) {
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing guides");

  int ret;
   opf->guide = NewListAlloc(LIST, NULL, NULL, NULL);

  ret = xmlTextReaderRead(reader);
  while (ret == 1 && 
         xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"guides")) {

    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }
    
    struct guide *item = malloc(sizeof(struct guide));
    item->type = xmlTextReaderGetAttribute(reader, (xmlChar *)"type");
    item->title = xmlTextReaderGetAttribute(reader, (xmlChar *)"href");
    item->href = xmlTextReaderGetAttribute(reader, (xmlChar *)"title");

    _epub_print_debug(opf->epub, DEBUG_INFO, 
                      "guide item: %s href: %s type: %s", 
                      item->title, item->href, item->type);
    AddNode(opf->guide, NewListNode(opf->guide, item));
    ret = xmlTextReaderRead(reader);
  }
}

listPtr _opf_parse_tour(struct opf *opf, xmlTextReaderPtr reader) {
  int ret;
  listPtr tour = NewListAlloc(LIST, NULL, NULL, NULL);

  ret = xmlTextReaderRead(reader);
  
  while (ret == 1 && 
         xmlStrcasecmp(xmlTextReaderConstName(reader),(xmlChar *)"tour")) {

    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }
    
    struct site *item = malloc(sizeof(struct site));
    item->title = xmlTextReaderGetAttribute(reader, (xmlChar *)"title");
    item->href = xmlTextReaderGetAttribute(reader, (xmlChar *)"href");
    AddNode(tour, NewListNode(tour, item));

    ret = xmlTextReaderRead(reader);
  }

  return tour;
}

void _opf_parse_tours(struct opf *opf, xmlTextReaderPtr reader) {
  _epub_print_debug(opf->epub, DEBUG_INFO, "parsing tours");

  int ret;
  opf->tours = NewListAlloc(LIST, NULL, NULL, NULL);

  ret = xmlTextReaderRead(reader);
  xmlChar *local = (xmlChar *)xmlTextReaderConstName(reader);
  
  while (ret == 1 && 
         xmlStrcasecmp(local,(xmlChar *)"tours")) {
    
    
    // ignore non starting tags
    if (xmlTextReaderNodeType(reader) != 1) {
      ret = xmlTextReaderRead(reader);
      continue;
    }
    
    if (xmlStrcasecmp(local, (xmlChar *)"tour") == 0) {
      struct tour *item = malloc(sizeof(struct tour));
      
      item->title = xmlTextReaderGetAttribute(reader, (xmlChar *)"title");
      item->id = xmlTextReaderGetAttribute(reader, (xmlChar *)"id");
      item->sites = _opf_parse_tour(opf, reader);

      AddNode(opf->tours, NewListNode(opf->tours, item));
    }
    
    free(local);
    local = (xmlChar *)xmlTextReaderConstName(reader);
    ret = xmlTextReaderRead(reader);
  }

  free(local);
}

void _opf_dump(struct opf *opf) {
  printf("Title(s):\n   ");
  DumpList(opf->metadata->title, (ListDumpFunc)_list_dump_string);
  printf("Creator(s):\n   ");
  DumpList(opf->metadata->creator, (ListDumpFunc)_list_dump_creator);
  printf("Identifier(s):\n   ");
  DumpList(opf->metadata->id, (ListDumpFunc)_list_dump_id);
  printf("Reading order:\n   ");
  DumpList(opf->spine, (ListDumpFunc)_list_dump_spine);
  printf("\n");
  if (opf->guide) {
    printf("Guide:\n");
    DumpList(opf->guide, (ListDumpFunc)_list_dump_guide);
  }
  if (opf->tours)
    DumpList(opf->tours, (ListDumpFunc)_list_dump_tour);
  if (opf->metadata->meta->Size != 0) {
    printf("Extra local metadata:\n");
    DumpList(opf->metadata->meta, (ListDumpFunc)_list_dump_meta);
  }
}

void _opf_close(struct opf *opf) {
  if (opf->metadata)
    _opf_free_metadata(opf->metadata);
  if (opf->toc)
    _opf_free_toc(opf->toc);
  if (opf->spine)
    FreeList(opf->spine, (ListFreeFunc)_list_free_spine);
  if (opf->tocName)
    free(opf->tocName);
  if (opf->manifest)
    FreeList(opf->manifest, (ListFreeFunc)_list_free_manifest);
  if (opf->guide)
    FreeList(opf->guide, (ListFreeFunc)_list_free_guide);
  if (opf->tours)
    FreeList(opf->tours, (ListFreeFunc)_list_free_tours);
  free(opf);
}
