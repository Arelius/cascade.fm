// Copyright (c) 2011, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#ifndef _MEM_TOOLS_H_
#define _MEM_TOOLS_H_

#include <stdlib.h>

void check_alloc(void* p);
void check_allocs();

void* mem_malloc(size_t size, const char* file, int line);
void mem_free(void* p);

#define malloc(size) mem_malloc(size, __FILE__, __LINE__)
#define free(size) mem_free(size);

#ifdef __cplusplus

inline void* operator new(size_t size, const char* file, int line)
{
    return mem_malloc(size, file, line);
}

inline void operator delete(void* p)
{
    return mem_free(p);
}

#endif

#define NEW new(__FILE__, __LINE__)
#define new NEW

#endif //_MEM_TOOLS_H_
