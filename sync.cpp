#include "sync.h"
#include "utf.h"
#include "database.h"

void sync_all(database* db)
{
    wchar* file;
    wchar* hash;

    while(file = db_get_file_local_song_copy(db, &hash))
    {
        // Upload file!
        // Mark as uploaded.
        delete [] file;
    }
}
