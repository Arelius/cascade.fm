// Copyright (c) 2011, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
typedef wchar_t wchar;

bool audiobox_check_exists_hash(const char* hash, const wchar* userpass);
bool audiobox_upload_file(const wchar* file_name, const wchar* userpass);
