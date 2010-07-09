#include "quick_string.h"
#include "sys.h"
#include <assert.h>

struct sys_dir_file
{
    HANDLE hand;
    WIN32_FIND_DATAW find;
};

sys_dir_file* sys_first_file(wchar_t* dir)
{

    QuickString<MAX_PATH> tmp_dir;
    tmp_dir.append_string(dir);
    tmp_dir.append_string(L"\\*");

    sys_dir_file* ret = new sys_dir_file();
    ret->hand = FindFirstFileExW(tmp_dir.utf_16(),
                                 FindExInfoStandard, // FindExInfoBasic on windows 7 to not return cAlternateFileName
                                 &ret->find,
                                 FindExSearchNameMatch,
                                 NULL,
                                 0); // FIND_FIRST_EX_LARGE_FETCH on windows 7 for speed boost?

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
        delete dir;
    }
}

bool sys_next_file(sys_dir_file* dir)
{
    assert(dir);
    return FindNextFileW(dir->hand, &dir->find);
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
    return dir->find.cFileName;
}
