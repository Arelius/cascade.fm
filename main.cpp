#include "sys.h"
#include "command_line.h"

#include <sys/stat.h>
#include <cstdio>
#include <time.h>
#include <assert.h>

#define MAX_COMMANDS 1

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
    return 0;
}

COMMAND_INFO(init, 0, 0,
             L"Initializes local databases.",
             L"init\n Takes no params.")

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
    num_commands = i;
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
