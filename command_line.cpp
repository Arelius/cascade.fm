#include "command_line.h"

#include <assert.h>

// Gets args by index, should skip switches.

int arg_get_real_idx(int index, int argc, wchar** argv)
{
    assert(index < argc);
    return index;
}

wchar* arg_get_arg(int index, int argc, wchar** argv)
{
    return argv[arg_get_real_idx(index, argc, argv)];
}

int arg_count(int argc, wchar** argv)
{
    return argc;
}
