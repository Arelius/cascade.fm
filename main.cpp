#include <sys/stat.h>
#include <cstdio>
#include <time.h>

int main(int argc, char** argv)
{
    if(argc == 2)
    {
        struct stat file_stat;
        stat(argv[1], &file_stat);

        // file_stat.st_mtime seems to be what we want to use!
        // for directories st_mtime gets updated when files get added,
        // not when files are updated.

        printf("File times\n");
        printf("Access:%d,", file_stat.st_atime);
        printf("%s", ctime(&file_stat.st_atime));
        printf("Modification:%d,", file_stat.st_mtime);
        printf("%s", ctime(&file_stat.st_mtime));
        printf("Status:%d,", file_stat.st_ctime);
        printf("%s", ctime(&file_stat.st_ctime));
    }
    return 0;
}
