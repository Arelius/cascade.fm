#include "mem_tools.h"

#include <assert.h>
#include <cstring>

#undef malloc
#undef free

#define mem_check(cond, header) assert(cond)

const char untouched_signature = 0xFF;
const char freed_signature = 0xEE;
const unsigned int untouched_prefix = 32;
const unsigned int untouched_suffix = 32;
const unsigned int header_signature = 0x11EADE17;

const size_t prefix_size = untouched_prefix * sizeof(untouched_signature);
const size_t suffix_size = untouched_suffix * sizeof(untouched_signature);

struct mem_header
{
    unsigned int sig;
    size_t size;
    bool has_been_freed;
    const char* src_file;
    int src_line;
};

struct alloc_list_elem
{
    char* ptr;
};

struct alloc_list_head
{
    size_t alloc_elem_count;
    alloc_list_head* next;
    // Just for convienience, exists right after the structure.
    alloc_list_elem* elems;
};

alloc_list_head * alloc_buffer_head = NULL;

const size_t default_alloc_elem_count = 128;

void alloc_buffer(alloc_list_head** head)
{
    size_t elem_count = default_alloc_elem_count;
    size_t size = 
        sizeof(alloc_list_head) +
        sizeof(alloc_list_elem) *
        elem_count;

    *head = (alloc_list_head*)malloc(size);
    memset(*head, 0, size);

    (*head)->alloc_elem_count = elem_count;
    (*head)->elems = (alloc_list_elem*)((*head) + 1);
}

void add_alloc(char* ptr)
{
    alloc_list_head** head = &alloc_buffer_head;
    
    while(true)
    {
        if((*head) == NULL)
        {
            alloc_buffer(head);
        }
    
        for(unsigned int i=0; i<(*head)->alloc_elem_count; i++)
        {
            if((*head)->elems[i].ptr == NULL)
            {
                (*head)->elems[i].ptr = ptr;
                return;
                //return (*head)->elems[i];
            }
        }
        head = &(*head)->next;
    }
}

typedef void (*allocfn)(alloc_list_elem* elem);

void map_alloc(allocfn fn)
{
    alloc_list_head* head = alloc_buffer_head;

    while(head)
    {
        for(unsigned int i=0; i<head->alloc_elem_count; i++)
        {
            if(head->elems[i].ptr)
                fn(&head->elems[i]);
            else
                break;
        }
        head = head->next;
    }
}

void check_alloc_fn(alloc_list_elem* elem)
{
    check_alloc(elem->ptr);
}

void check_allocs()
{
    map_alloc(check_alloc_fn);
}

void* mem_malloc(size_t size, const char* file, int line)
{
    check_allocs();
    char* ptr = (char*)malloc(size + 
        sizeof(mem_header) +
        prefix_size +
        suffix_size);

    mem_header* h = (mem_header*)ptr;

    h->sig = header_signature;
    h->size = size;
    h->has_been_freed = false;
    h->src_file = file;
    h->src_line = line;

    memset(ptr + sizeof(mem_header),
           untouched_signature,
           untouched_prefix);
    memset(ptr +
           sizeof(mem_header) +
           size +
           prefix_size,
           untouched_signature,
           untouched_suffix);

    char* ret = ptr +
        sizeof(mem_header) +
        prefix_size;

    add_alloc(ret);
    return ret;
}

void mem_free(void* p)
{
    check_allocs();
    char* ptr = (char*)p -
        sizeof(mem_header) -
        prefix_size;

    mem_header* h = (mem_header*)ptr;

    mem_check(h->sig == header_signature, NULL);

    h->has_been_freed = true;

    memset(ptr + sizeof(mem_header),
           freed_signature,
           h->size);

#if 0
    // If we want to run this in an actual program that uses serious memory, we should probally actually release the memory.
    free(ptr);
    // Remove from alloc list.
#endif
}

void check_alloc(void* p)
{
    char* ptr = (char*)p -
        sizeof(mem_header) -
        prefix_size;

    mem_header* h = (mem_header*)ptr;

    mem_check(h->sig == header_signature, NULL);

    char* n = ptr + sizeof(mem_header);
    
    for(char* m = n;
        m < n + prefix_size;
        m++)
    {
        mem_check(*m == untouched_signature, h);
    }

    n += prefix_size;

    if(h->has_been_freed)
    {
        for(char* m = n;
            m < n + h->size;
            m++)
        {
            mem_check(*m == freed_signature, h);
        }
    }

    n += h->size;

    for(char* m = n;
        m < n + suffix_size;
        m++)
    {
        mem_check(*m == untouched_signature, h);
    }
}
