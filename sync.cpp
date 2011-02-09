#include "sync.h"
#include "utf.h"
#include "database.h"

void sync_all(database* db)
{
    wchar* file;
    wchar* hash;
    wchar* session_id = db_get_session_id(db);

    while(file = db_get_file_local_song_copy(db, &hash))
    {
        // Mark as uploaded.
        delete [] file;
        delete [] hash;
    }

    delete [] session_id;
}
