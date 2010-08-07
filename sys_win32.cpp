#include "quick_string.h"
#include "sys.h"

#include <assert.h>
#include <shlobj.h>

struct sys_dir_file
{
    HANDLE hand;
    WIN32_FIND_DATAW find;
    wchar* dir;
    wchar* full_path;
};

void sys_fill_path(sys_dir_file* dir)
{
    if(dir->full_path)
        delete dir->full_path;
    dir->full_path = new wchar[4096];
    DWORD success = GetFullPathNameW(dir->find.cFileName, 4096, dir->full_path, NULL);
    assert(success); // Not sure what could cause this.
}

sys_dir_file* sys_first_file(const wchar_t* dir)
{
    QuickString<MAX_PATH> tmp_dir;
    tmp_dir.append_string(dir);
    tmp_dir.append_string(L"\\*");

    sys_dir_file* ret = new sys_dir_file();
    
    ret->dir = new wchar[str_byte_length(dir) + char_term_len];
    str_copy(ret->dir, dir);
    ret->full_path = NULL;

    ret->hand = FindFirstFileExW(tmp_dir.utf_16(),
                                 FindExInfoStandard, // FindExInfoBasic on windows 7 to not return cAlternateFileName
                                 &ret->find,
                                 FindExSearchNameMatch,
                                 NULL,
                                 0); // FIND_FIRST_EX_LARGE_FETCH on windows 7 for speed boost?

    sys_fill_path(ret);

    if(ret->hand == INVALID_HANDLE_VALUE)
    {
        delete ret;
        ret = NULL;
    }

    return ret;
}

void sys_close_dir(sys_dir_file* dir)
{
    if(dir)
    {
        FindClose(dir->hand);
        delete dir->dir;
        delete dir->full_path;
        delete dir;
    }
}

bool sys_next_file(sys_dir_file* dir)
{
    assert(dir);
    bool ret = FindNextFileW(dir->hand, &dir->find);
    sys_fill_path(dir);
    return ret;
}

time64 sys_mod_time(sys_dir_file* dir)
{
    assert(dir);
    ULARGE_INTEGER ret;
    
    ret.LowPart = dir->find.ftLastWriteTime.dwLowDateTime;
    ret.HighPart = dir->find.ftLastWriteTime.dwHighDateTime;

    return ret.QuadPart;
}

const wchar_t* sys_file_name(sys_dir_file* dir)
{
    assert(dir);
    return dir->full_path;
}

bool sys_is_directory(sys_dir_file* dir)
{
    assert(dir);
    return dir->find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

bool sys_file_exists(const wchar_t* file)
{
    return GetFileAttributesW(file) != 0xFFFFFFFF;
}

bool sys_make_path(const wchar_t* path)
{
    return SHCreateDirectoryExW(NULL, path, NULL) == ERROR_SUCCESS;
}
