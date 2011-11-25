#include "xalloc.h"

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define HEADER_MAGIC_GUARD 0xAABB
#define MEM_HEADER(size, num_children, children, parent)                \
  (struct mem_header){ HEADER_MAGIC_GUARD, size, num_children, children, parent }
#define POINTER_TO_ENTRY(ptr) (struct mem_entry*)(((uint8_t*)ptr) - sizeof(struct mem_entry))

struct mem_entry;

/*! \brief Memory header */
struct mem_header {
  const uint16_t guard;          /*!< Memory guard to identify a sane input. */

  uintptr_t ptr_size;            /*!< Size of data pointer. */

  // NOTE: Use uint16 instead? Overhead for 2**32 children would be 160GiB by itself
  uint32_t num_children;         /*!< Number of children this pointer holds */ 
  struct mem_entry** children;   /*!< Children. */

  struct mem_entry* parent;      /*!< Parent context of this pointer. */
 };

/*! \brief Memory context used by xalloc functions */
struct mem_entry {
  /*! Memory header, contains everything needed to manage
      pointers.
   */
  struct mem_header header; 
  /*! Data pointer. This is what is actually returned to the user. */
  uint8_t data[];
};

static void xalloc_add_child(struct mem_entry* parent, struct mem_entry* child) {
  parent->header.children  = realloc(parent->header.children,
                                         sizeof(struct mem_entry*)
                                         * ++parent->header.num_children);

  parent->header.children[parent->header.num_children - 1] = child;
}

static struct mem_entry* xalloc_new_entry(const void *ptr, size_t size) {
  struct mem_entry* ctx;

  const size_t alloc_size = sizeof(struct mem_entry) + size;

  /* no parent, create new root entry */
  if(ptr == NULL) {
    ctx = malloc(alloc_size);
    memset(ctx, 0, alloc_size);

    ctx->header = MEM_HEADER(size, 0, NULL, NULL);

    return ctx;
  } else {
    struct mem_entry* parent = POINTER_TO_ENTRY(ptr);
    assert(parent->header.guard == HEADER_MAGIC_GUARD);

    ctx = malloc(alloc_size);
    memset(ctx, 0, alloc_size);

    ctx->header = MEM_HEADER(size, 0, NULL, parent);

    xalloc_add_child(parent, ctx);

    return ctx;
  }
}

void* xalloc_new(const void* ptr, size_t size) {
  struct mem_entry* ctx = xalloc_new_entry(ptr, size);
  return ctx->data;
}

static void xalloc_free_entry(struct mem_entry* ctx) {
  if(ctx == NULL) return;
  if(ctx->header.num_children != 0) {

    for(unsigned i = 0; i < ctx->header.num_children; ++i) {
      xalloc_free_entry(ctx->header.children[i]);
    }

    free(ctx->header.children);
  }

  free(ctx);
}

void xalloc_free(const void* ptr) {
  if(ptr == NULL) return;

  struct mem_entry* ctx = POINTER_TO_ENTRY(ptr);
  assert(ctx->header.guard == HEADER_MAGIC_GUARD);

  xalloc_free_entry(ctx);
}

void* xalloc_steal(const void* new, const void* ptr) {
  struct mem_entry *ctx, *new_parent;

  if(ptr == NULL) return NULL;

  ctx = POINTER_TO_ENTRY(ptr);
  new_parent = POINTER_TO_ENTRY(new);

  assert(ctx->header.guard == HEADER_MAGIC_GUARD);
  assert(new_parent->header.guard == HEADER_MAGIC_GUARD);

  struct mem_entry *old_parent = ctx->header.parent;

  if(old_parent == new_parent) return ctx->data;

  xalloc_add_child(new_parent, ctx);

  if(old_parent == NULL)  return ctx->data;

  /* locate the child pointer, clean up array */
  for(unsigned i = 0; i < old_parent->header.num_children; ++i) {

    if(old_parent->header.children[i] == ctx) {
      for(unsigned j = i; j < old_parent->header.num_children - 1; ++i) {
        old_parent->header.children[i] = old_parent->header.children[i + 1];
      }
      return ctx->data;
    }
  }

  assert(0 && "Entry is not a child of parent");
}

char* xalloc_strndup(const void* ptr, const char* str, size_t sz) {
  struct mem_entry *ctx = xalloc_new_entry(ptr, sz + 1);
  memcpy(ctx->data, str, sz);

  return (char*)(ctx->data);
}

char* xalloc_asprintf(const void* ptr, const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int length = vsnprintf(NULL, 0, fmt, ap);

  va_end(ap);

  char *string = xalloc_new(ptr, length + 1);

  va_start(ap, fmt);
  vsnprintf(string, length + 1, fmt, ap);
  va_end(ap);

  return string;
}

