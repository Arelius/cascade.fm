#include "sync.h"
#include "utf.h"
#include "database.h"
#include "audiobox.h"

#include <cstdio>

void sync_all(database* db, const wchar* userpass)
{
    wchar* file;
    char* hash;

    while(file = db_get_file_local_song_copy(db, &hash))
    {
        bool upload_success = audiobox_upload_file(file, userpass);

        if(upload_success)
        {
            db_update_song_server_status(db, hash, true);
            wprintf(L"Successfully upload file: %s\n", file);
        }
        else
        {
            wprintf(L"Failure uploading file: %s\n", file);
        }
        // Mark as uploaded.
        delete [] file;
        delete [] hash;
    }
}
