#pragma once

#ifndef XALLOC_H_
#define XALLOC_H_ 1
#include <string.h>

/*! \file xalloc.h
    \brief Main header for xalloc.
*/

/*! \mainpage xalloc
  \section about About
  xalloc is a small library providing hierarchical memory management in
  the style of talloc. Does not provide a full replacement for talloc's
  feature set yet, and hasn't been thoroughly tested. 
  
  \section license License (MIT License)
  
  Copyright (c) 2011 Erik Price
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, 
  publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
  so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*! \fn void* xalloc_new(const void* ptr, size_t size)
    \brief Allocate new chunk of memory
    
    \param ptr Previously xalloc'd pointer to use as parent, or NULL.
    \param size Size of memory block to allocate.

    \see xalloc(ptr, type)
 */
void* xalloc_new(const void* ptr, size_t size);

/*! \fn void xalloc_free(const void* ptr)
    \brief Free a pointer that has been created with xalloc

    Any children of this pointer will also be freed.

    Pointer must have been created with xalloc, or xalloc_free will fail
    an assertion (this can also happen if the memory becomes corrupted).

    \param ptr A pointer returned from a call to xalloc or xalloc_new
 */
void xalloc_free(const void* ptr);

/*! \fn void* xalloc_steal(const void* new, const void* ptr)
    \brief Change a pointer's parent

    Both the new parent and the pointer must be xalloc'd.
    If the new parent is the same as the current parent of
    the pointer, nothing happens; otherwise, the function 
    searches through the pointer's parent's children to find
    itself and remove any reference to it. Note that this 
    function operates on a flat array and as a result must 
    search and remake the parent's array of children in O(n)
    time.

    \param new Pointer to set as parent.
    \param ptr Pointer to move.
 */
void* xalloc_steal(const void* new, const void* ptr);

/*! \fn char* xalloc_strndup(const void* ptr, const char* str, size_t sz)
    \brief Duplicates a string into xalloc'd memory
    
    \param ptr Pointer to set as parent of new string, or NULL.
    \param str String to duplicate.
    \param sz Number of characters to copy.
    
    \see xalloc_strdup(ptr, str)
 */
char* xalloc_strndup(const void* ptr, const char* str, size_t sz);

/*! \fn char* xalloc_asprintf(const void* ptr, const char* fmt, ...)
    \brief Prints a format to a newly xalloc'd string.
    
    Operates exactly like asprintf().

    \param ptr Pointer to use as parent for new string, or NULL.
    \param fmt Format string.
    \param ... Arguments for the format string.
 */
char* xalloc_asprintf(const void* ptr, const char* fmt, ...);

/*! \def xalloc(ptr, type)
    \brief Allocate correct amount of memory for a given type.

    \param ptr Pointer to use as parent, or NULL.
    \param type A data type to allocate space for.

    \see xalloc_new(const void* ptr, size_t sz)
 */
#define xalloc(ptr, type) xalloc_new(ptr, sizeof(type))

/*! \def xalloc_strdup(ptr, str)
    \brief A macro that automatically copies the correct number of characters for a given string

    \param ptr Pointer to use as parent for new string, or NULL.
    \param str String to duplicate.

    \see xalloc_strndup(const void* ptr, const char* str, size_t sz)
 */
#define xalloc_strdup(ptr, str) xalloc_strndup(ptr, str, strlen(str));

#endif /* XALLOC_H_ */

