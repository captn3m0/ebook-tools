#include "epublib.h"

// Free root struct
void _list_free_root(struct root *data) {
  if (data->mediatype)
    free(data->mediatype);
  if (data->mediatype)
    free(data->fullpath);
  free(data);
}

void _list_free_creator(struct creator *data) {
  if (data->name)
    free(data->name);
  if (data->fileAs)
    free(data->fileAs);
  if (data->role)
    free(data->role);
  free(data);
}

void _list_free_spine(struct spine *spine) {
  if (spine->idref)
    free(spine->idref);
  free(spine);
}

void _list_free_guide(struct guide *guide) {
  if (guide->href)
    free(guide->href);
  if (guide->type)
    free(guide->type);
  if (guide->title)
    free(guide->title);
  free(guide);
}

void _list_free_site(struct site *site) {
  if (site->title)
    free(site->title);
  if (site->href)
    free(site->href);
  free(site);
}

void _list_free_tours(struct tour *tour) {
  if (tour->id)
    free(tour->id);
  if (tour->title)
    free(tour->title);
      
  FreeList(tour->sites, (ListFreeFunc)_list_free_site);
  free(tour);
}

void _list_free_manifest(struct manifest *manifest) {

  if (manifest->namespace)
    free(manifest->namespace);
  if (manifest->modules)
    free(manifest->modules);
  if (manifest->id)
    free(manifest->id);
  if (manifest->href)
    free(manifest->href);
  if (manifest->type)
    free(manifest->type);
  if (manifest->fallback)
    free(manifest->fallback);
  if (manifest->fbStyle)
    free(manifest->fbStyle);

  free(manifest);
} 

// Compare 2 root structs by mediatype field
int _list_cmp_root_by_mediatype(struct root *root1, struct root *root2) {

  //  printf("comparing %s %s\n", root1->mediatype, root2->mediatype);
  return strcmp(root1->mediatype, root2->mediatype);
}

// Print root 
void _list_dump_root(struct root *root) {
  printf("   %s (%s)\n", 
         root->fullpath, root->mediatype);
}
void _list_dump_spine(struct spine *spine) {
  printf("%s", spine->idref);
  if (spine->linear) 
    printf("(L)");
  printf(" ");
}

void _list_dump_spine_linear(struct spine *spine) {
  if (spine->linear) 
    _list_dump_spine(spine);
}

void _list_dump_string(char *string) {
  printf("%s\n", string);
}
 
void _list_dump_creator(struct creator *data) {
  if (data->role)
    printf("%s", data->role);
  else
    printf("Author");
  
  printf(": %s (%s)\n", data->name, 
         ((data->fileAs)?data->fileAs:data->name)); 
}

void _list_dump_guide(struct guide *guide) {
  printf("%s: %s(%s)\n", guide->title, guide->href, guide->type);
}

void _list_dump_site(struct site *site) {
  printf("site: %s(%s)\n", site->title, site->href);
}

void _list_dump_tour(struct tour *tour) {
  printf("Tour %s(%s):\n", tour->title, tour->id);
  DumpList(tour->sites, (ListDumpFunc)_list_dump_site);
}
