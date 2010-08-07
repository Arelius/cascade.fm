#include "database.h"
#include "sys.h"

void scan_directory(database* db, const wchar* dir_name)
{
    sys_dir_file* dir = sys_first_file(dir_name);

    if(dir)
    {
        do
        {
            if(sys_is_directory(dir))
                db_add_local_dir(db, sys_file_name(dir));
            else
                db_add_local_file(db, sys_file_name(dir), sys_mod_time(dir));
        }
        while(sys_next_file(dir));
    }
    sys_close_dir(dir);
}

void scan_all(database* db)
{
    db_inject_library_directories(db);

    wchar* curr_dir;

    while((curr_dir = db_get_local_dir_copy(db)))
    {
        scan_directory(db, curr_dir);
        db_remove_local_dir(db, curr_dir);
        free(curr_dir);
    }
}
