#include <sys/stat.h>
#include <cstdio>

int main(int argc, char** argv)
{
    if(argc == 2)
    {
        struct stat file_stat;
        stat(argv[1], &file_stat);

        printf("File times, access:%d modification:%d status:%d\n", file_stat.st_atime, file_stat.st_mtime, file_stat.st_ctime);
    }
    return 0;
}
