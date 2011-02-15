// Copyright (c) 2011, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
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
            // On failure, decrease priority, so we can get around to uploading other things.
            db_adjust_song_upload_priority(db, hash, -1);
            wprintf(L"Failure uploading file: %s\n", file);
        }
        // Mark as uploaded.
        delete [] file;
        delete [] hash;
    }
}
