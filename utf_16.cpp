#include "utf.h"
#include <Windows.h>
#include <cstdio>

wchar* char_next(wchar* str)
{
    return CharNextW(str);
}

size_t char_length(const wchar* str)
{
    return CharNextW(str) - str;
}

void char_copy(wchar* dest, const wchar* src)
{
    memcpy(dest, src, char_length(src));
}

bool char_eos(const wchar* str)
{
    return CharNextW(str) == str;
}

void char_inc(const wchar** str)
{
    *str = CharNextW(*str);
}

bool char_is_alpha(const wchar* c)
{
    return IsCharAlpha(*c);
}

bool char_is_upper(const wchar* c)
{
    return IsCharUpper(*c);
}

bool char_is_lower(const wchar* c)
{
    return IsCharLower(*c);
}
