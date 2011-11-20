#include "xalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct A {
  int x, y, z;
  long a, b, c;
  char* str;
} A;

int main() {

  A* ptr = xalloc(NULL, sizeof(A));
  ptr->str = xalloc_asprintf(ptr, "%d%d%d", 3, 2, 1);
  xalloc_steal(ptr, ptr->str);

  ptr->x = 111;

  printf("%d => %s\n", ptr->x, ptr->str);

  xalloc_free(ptr);

  return 0;
}
