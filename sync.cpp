#include "sync.h"
#include "utf.h"
#include "database.h"

void sync_all(database* db, const wchar* userpass)
{
    wchar* file;
    wchar* hash;

    while(file = db_get_file_local_song_copy(db, &hash))
    {
        // Mark as uploaded.
        delete [] file;
        delete [] hash;
    }
}
