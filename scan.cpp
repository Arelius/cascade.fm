#include "database.h"
#include "sys.h"
#include "utf.h"
#include "ext/sha2.h"

#include "scan.h"

#include <cstdio>

// Load from DB.
const wchar* scan_file_extensions[] =
{
    L"mp3",
    L"ogg",
    L"flac",
    L"aac",
    L"m4a",
    NULL
};

const wchar* get_extension(const wchar* file_name)
{
    const wchar* ext = NULL;
    const wchar* tmp = file_name;

    while(!char_eos(tmp))
    {
        if(*tmp == L'.')
            ext = char_next(tmp);

        char_inc(&tmp);
    }

    return ext;
}

bool scan_check_file_extension(const wchar* file_name)
{
    const wchar* ext = get_extension(file_name);

    if(ext == NULL)
        return false;

    const wchar** test_ext = scan_file_extensions;

    while(*test_ext != NULL)
    {
        if(str_icompare(ext, *test_ext) == 0)
            return true;

        test_ext++;
    }

    return false;
}

void scan_directory(database* db, const wchar* dir_name)
{
    sys_dir_file* dir = sys_first_file(dir_name);

    if(dir)
    {
        do
        {
            if(sys_is_directory(dir))
                db_add_local_dir(db, sys_file_name(dir));
            else if(scan_check_file_extension(sys_file_name(dir)))
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
        delete [] curr_dir;
    }
}

bool hash_file(wchar* filename, unsigned char* buffer)
{
    SHA256_CTX sha;
    SHA256_Init(&sha);
    size_t len = 0;
    FILE* file = _wfopen(filename, L"rb");

    if(!file)
        return false;
    
    while((len = fread(buffer, 1, Hash_Buffer_Len, file)) > 0)
    {
        SHA256_Update(&sha, buffer, len);
    }
    
    fclose(file);
    
    SHA256_End(&sha, (char*)buffer);

    return true;
}

void hash_all(database* db)
{
    SHA256_CTX sha;
    wchar* curr_file;
    unsigned char buffer[Hash_Buffer_Len];

    while((curr_file = db_get_local_file_copy(db)))
    {
        SHA256_Init(&sha);
        size_t len = 0;
        FILE* file = _wfopen(curr_file, L"rb");

        while((len = fread(buffer, 1, Hash_Buffer_Len, file)) > 0)
        {
            SHA256_Update(&sha, buffer, len);
        }
        
        fclose(file);

        SHA256_End(&sha, (char*)buffer);
        wprintf(L"Sha2 (%s):", curr_file);
        printf("%s\n", buffer);

        db_add_local_file_hash(db, curr_file, (char*)buffer);
        
        delete [] curr_file;
    }
}
