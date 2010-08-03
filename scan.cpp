#include "database.h"
#include "sys.h"

void scan_all(database* db)
{
    db_inject_library_directories(db);
}

void scan_directory(database* db, const wchar* dir_name)
{
    sys_dir_file* dir = sys_first_file(dir_name);

    if(dir)
    {
        do
        {
            db_add_local_file(db, sys_file_name(dir), sys_mod_time(dir));
        }
        while(sys_next_file(dir));
    }
    sys_close_dir(dir);
}
