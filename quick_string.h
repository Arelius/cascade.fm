#include "utf.h"
#include <cstdio>

template<int initial_alloc>
class QuickString
{
    wchar* str;
    wchar* end;
    wchar* max;
    wchar buffer[initial_alloc];
public:
    QuickString()
    {
        end = str = buffer;
        max = str + initial_alloc - 1;

        // Starts unterminated.
    }

    ~QuickString()
    {
        if(str != buffer)
            delete[] str;
    }

    const wchar* utf_16() const
    {
        return str;
    };
    
    void double_alloc()
    {
        size_t new_alloc = (max-str) * 2;
        wchar* tmp = new wchar[new_alloc];
        memcpy(tmp, str, (end-str)+1);;

        max = tmp + new_alloc-1;
        end = tmp + (end - str);
    
        if(str != buffer)
            delete[] str;
        str = tmp;
    }
    
    void append_string(const wchar* src)
    {
        const wchar* next = src;
        while(!char_eos(next))
        {
            // We just double allocations so that we don't need a count pass.
            // with a larg enough initial alloc, we should rarely need to grow.
            if(end + char_length(next) >= max)
                double_alloc();
            
            char_copy(end, next);
            end = char_next(end); //char_inc(&end);
            char_inc(&next);
        }

        *end = char_termination;
    }

    void clear()
    {
        end = str;
    }
};
