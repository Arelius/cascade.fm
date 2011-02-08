#ifndef _UTF_H_
#define _UTF_H_

typedef wchar_t wchar;

// Is this right?
const wchar char_termination = L'\0';
const size_t char_term_len = 1;

wchar* char_next(wchar* str);
const wchar* char_next(const wchar* str);
size_t char_length(const wchar* str);
size_t str_length(const wchar* str);
size_t str_byte_length(const wchar* str);
wchar* str_copy(wchar* dest, const wchar* src);
int str_compare(const wchar* str1, const wchar* str2);
int str_icompare(const wchar* str1, const wchar* str2);
void char_copy(wchar* dest, const wchar* src);
bool char_eos(const wchar* str);
void char_inc(const wchar** str);
bool char_is_alpha(const wchar* c);
bool char_is_upper(const wchar* c);
bool char_is_lower(const wchar* c);

size_t utf16_to_utf8(char* dest, size_t buf_len, const wchar* src);
#endif //_UTF_H_
