#include <sys/stat.h>
#include <cstdio>
#include <time.h>
#include "sys.h"

int wmain(int argc, wchar_t** argv)
{
    if(argc == 2)
    {
        sys_dir_file* dir = sys_first_file(argv[1]);
        
        if(dir)
        {
            do
            {
                wprintf(L"File %s Modified on: %I64u\n", sys_file_name(dir), sys_mod_time(dir));
            }
            while(sys_next_file(dir));
        }
        sys_close_dir(dir);
    }
    return 0;
}
