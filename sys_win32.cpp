#include "sys.h"

struct sys_dir_file
{
    HANDLE hand;
    WIN32_FIND_DATAW find;
};

sys_dir_file* sys_first_file(wchar_t* dir)
{
    sys_dir_file* ret = new sys_dir_file();
    ret->hand = FindFirstFileW(dir, &ret->find);
    return ret;
}

void sys_close_dir(sys_dir_file* dir)
{
    FindClose(dir->hand);
    delete dir;
}

bool sys_next_file(sys_dir_file* dir)
{
    return FindNextFileW(dir->hand, &dir->find);
}

time64 sys_mod_time(sys_dir_file* dir)
{
    ULARGE_INTEGER ret;
    
    ret.LowPart = dir->find.ftLastWriteTime.dwLowDateTime;
    ret.HighPart = dir->find.ftLastWriteTime.dwHighDateTime;

    return ret.QuadPart;
}

const wchar_t* sys_file_name(sys_dir_file* dir)
{
    return dir->find.cFileName;
}
