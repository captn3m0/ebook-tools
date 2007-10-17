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

// Compare 2 root structs by mediatype field
int _list_cmp_root_by_mediatype(struct root *root1, struct root *root2) {

  //  printf("comparing %s %s\n", root1->mediatype, root2->mediatype);
  return strcmp(root1->mediatype, root2->mediatype);
}

// Print root 
void _list_dump_root(struct root *root) {
  printf("root -- full path: %s media-type: %s\n", 
         root->fullpath, root->mediatype);
}

void _list_dump_string(char *string) {
  printf("%s\n", string);
}
void _list_dump_creator(struct creator *data) {
  if (data->role)
    printf("%s", data->role);
  else
    printf("writer");

  printf(": %s (%s)\n", data->name, 
         ((data->fileAs)?data->fileAs:data->name)); 
}
