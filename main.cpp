#include "sys.h"
#include "command_line.h"
#include "database.h"
#include "scan.h"
#include "sync.h"

#include <sys/stat.h>
#include <cstdio>
#include <time.h>
#include <assert.h>

#define MAX_COMMANDS 7

struct command_line
{
    int min_args;
    int max_args;
    int (*cmd_proc)(int argc, wchar_t** argv);
    const wchar* command;
    const wchar* short_desc;
    const wchar* usage;
};

command_line cmd_lst[MAX_COMMANDS];
int num_commands;

void set_cmd_info(const wchar* command,
                  int (*cmd_proc)(int, wchar_t**),
                  int min_args,
                  int max_args,
                  const wchar* short_desc,
                  const wchar* usage,
                  int i)
{
    assert(i <= MAX_COMMANDS);
    cmd_lst[i].command = command;
    cmd_lst[i].min_args = min_args;
    cmd_lst[i].max_args = max_args;
    cmd_lst[i].cmd_proc = cmd_proc;
    cmd_lst[i].short_desc = short_desc;
    cmd_lst[i].usage = usage;
}

#define COMMAND_INFO(cmd, min, max, short, desc) \
    void cmd_##cmd##_info(int i) \
    { \
    set_cmd_info( \
        L#cmd, \
        cmd_##cmd, \
        min, \
        max, \
        short, \
        desc, \
        i); \
    }

int cmd_init(int argc, wchar_t** argv)
{
    database* db = db_open(NULL);
    db_init(db);
    db_close(db);
    return 0;
}

COMMAND_INFO(init, 0, 1,
             L"Initializes local databases.",
             L"init\n Takes no params.")

void lib_print(const wchar* dir, void* UP)
{
    int* i = (int*)UP;
    // Was goint to allow delete via index, this seems dangerous!
    wprintf(L" %s\n", dir);
}

int cmd_show_library(int argc, wchar_t** argv)
{
    database* db = db_open();

    wprintf(L"Directories in Library:\n");

    int i=1; // 1 easier on many users.
    db_print_search_dir(db, lib_print, &i);

    db_close(db);
    return 0;
}

COMMAND_INFO(show_library, 0, 0,
             L"Shows directories in library.",
             L"show_library")

int cmd_add_path(int argc, wchar_t** argv)
{
    //TODO: Verify dir!
    wchar* dir = argv[1];

    database* db = db_open();
    db_add_search_dir(db, dir);
    wprintf(L"Added dir '%s' to library.\n", dir);
    db_close(db);
    return 0;
}

COMMAND_INFO(add_path, 1, 1,
             L"Adds directories to Library.",
             L"add_path [dir]\n\tdir - directory to add to path")

int cmd_rm_path(int argc, wchar_t** argv)
{
    //TODO: Add support for multiple arguments
    database* db = db_open();
    int deleted = db_rm_search_dir(db, argv[1]);
    wprintf(L"Removed %d directories from library.\n", deleted);
    db_close(db);
    return 0;
}

COMMAND_INFO(rm_path, 1, 1,
             L"Removes directories from Library.",
             L"rm_path [dir]\n\tdir - directory to remove from path, supports wildcards")

int cmd_scan(int argc, wchar_t** argv)
{
    database* db = db_open();

    scan_all(db);
    hash_all(db);

    db_close(db);
    return 0;
}

COMMAND_INFO(scan, 0, 0,
             L"Begins scan of files in Library.",
             L"scan\n")

int cmd_sync(int argc, wchar_t** argv)
{
    database* db = db_open();

    sync_all(db);

    db_close(db);
    return 0;
}

COMMAND_INFO(sync, 0, 0,
             L"Begins upload of local scanned files in Library.",
             L"sync\n");

int cmd_test(int argc, wchar_t** argv)
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
    return 0;
}

COMMAND_INFO(test, 1, 1,
             L"Test directory info.",
             L"test [file]\n\tfile - file/directory to get info on");

void initialize_commands()
{
    int i=0;
    cmd_init_info(i++);
    cmd_test_info(i++);
    cmd_show_library_info(i++);
    cmd_add_path_info(i++);
    cmd_rm_path_info(i++);
    cmd_scan_info(i++);
    cmd_sync_info(i++);
    num_commands = i;

    assert(num_commands <= MAX_COMMANDS);
}

int wmain(int argc, wchar_t** argv)
{
    initialize_commands();
    int num_args = arg_count(argc, argv);
    if(num_args >= 2)
    {
        int index = arg_get_real_idx(1, argc, argv);
        wchar* command = argv[index];
        for(int i=0; i < num_commands; i++)
        {
            if(str_compare(command, cmd_lst[i].command) == 0)
            {
                if(num_args - 2 >= cmd_lst[i].min_args &&
                   num_args - 2 <= cmd_lst[i].max_args)
                    return (cmd_lst[i].cmd_proc)(argc - (index + 1), argv + index);
                else
                {
                    wprintf(L"Command '%s' does not expect %d (%d-%d) arguments.",
                            command,
                            num_args - 2,
                            cmd_lst[i].min_args,
                            cmd_lst[i].max_args);
                    return 1;
                }
            }
        }
        wprintf(L"Command '%s' not found.", command);
        return 1;
    }
    else
    {
        wprintf(L"Not enough arguments.");
        return 1;
    }
}
