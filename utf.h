typedef wchar_t wchar;

// Is this right?
const wchar char_termination = L'\0';

wchar* char_next(wchar* str);
size_t char_length(const wchar* str);
void char_copy(wchar* dest, const wchar* src);
bool char_eos(const wchar* str);
void char_inc(const wchar** str);
bool char_is_alpha(const wchar* c);
bool char_is_upper(const wchar* c);
bool char_is_lower(const wchar* c);
