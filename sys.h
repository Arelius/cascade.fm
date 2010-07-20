#include <Windows.h>

struct sys_dir_file;

typedef unsigned long long int uint64;
typedef uint64 time64;

sys_dir_file* sys_first_file(const wchar_t* dir);
void sys_close_dir(sys_dir_file* dir);
bool sys_next_file(sys_dir_file* dir);
time64 sys_mod_time(sys_dir_file* dir);
// Not valid after sys_next_file.
const wchar_t* sys_file_name(sys_dir_file* dir);
bool sys_file_exists(const wchar_t* file);
bool sys_make_path(const wchar_t* path);
