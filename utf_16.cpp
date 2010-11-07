#include "utf.h"
#include <Windows.h>
#include <cstdio>

wchar* char_next(wchar* str)
{
    return CharNextW(str);
}

const wchar* char_next(const wchar* str)
{
    return CharNextW(str);
}

size_t str_length(const wchar* str)
{
    return wcslen(str);
}

size_t str_byte_length(const wchar* str)
{
    return strlen(str);
}

// returns a pointer to the null termination character.
wchar* str_copy(wchar* dest, const wchar* src)
{
    wcscpy(dest, src);
    return dest + wcslen(src);
}

int str_compare(const wchar* str1, const wchar* str2)
{
    return wcscmp(str1, str2);
}

int str_icompare(const wchar* str1, const wchar* str2)
{
    return wcsicmp(str1, str2);
}

size_t char_length(const wchar* str)
{
    return CharNextW(str) - str;
}

void char_copy(wchar* dest, const wchar* src)
{
    lstrcpynW(dest, src, 2);
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
