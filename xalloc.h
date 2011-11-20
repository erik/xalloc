#pragma once

#ifndef XALLOC_H_
#define XALLOC_H_ 1
#define _GNU_SOURCE
#include <string.h>

void* xalloc_new(const void* ptr, unsigned size);
void xalloc_free(const void* ptr);

void* xalloc_steal(const void* new, const void* ptr);

char* xalloc_strndup(const void* ptr, const char* str, unsigned sz);

char* xalloc_asprintf(const void* ptr, const char* fmt, ...);

#define xalloc(ptr, type) xalloc_new(ptr, sizeof(type))
#define xalloc_strdup(ptr, str) xalloc_strndup(ptr, str, strlen(str));

#endif /* XALLOC_H_ */
