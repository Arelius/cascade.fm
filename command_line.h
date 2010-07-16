// Gets args by index, should skip switches.
#include "utf.h"
int arg_get_real_idx(int index, int argc, wchar** argv);
wchar* arg_get_arg(int index, int argc, wchar** argv);
int arg_count(int argc, wchar** argv);
