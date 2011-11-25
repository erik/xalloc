#include "xalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct A {
  int x, y, z;
  long a, b, c;
  char* str;
} A;

typedef struct linked_list {
  struct linked_list *next;
  struct A *value;
} linked_list;

int main() {

  linked_list *root = xalloc(NULL, sizeof(linked_list));
  
  linked_list *nxt = root;

  for(int i = 0; i < 100; ++i) {
    nxt->next = xalloc(nxt, sizeof(linked_list));
    nxt->value = xalloc(nxt, sizeof(A));

    nxt->value->x = i;
    nxt->value->str = xalloc_asprintf(nxt, "%d.", i);

    nxt = nxt->next;
  }

  nxt = root;
  for(int i = 0; i < 100; ++i) {
    printf("%d (%s), ", nxt->value->x, nxt->value->str);
    nxt = nxt->next;
  }


  xalloc_free(root);

  return 0;
}
