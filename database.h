// Copyright (c) 2011, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include "utf.h"
#include "sys.h"

struct database;

// *_copy functions return a string that will need to be delete[] 'ed

database* db_open(const wchar* db_file = 0);
void db_close(database* db);
void db_init(database* db);
void db_add_search_dir(database* db, const wchar* dir);
int db_rm_search_dir(database* db, const wchar* dir);
void db_print_search_dir(database* db, void (*print)(const wchar* dir, void* UP), void* UP);

void db_inject_library_directories(database* db);

// If false they are entered as NULL/UNKNOWN into the database.
void db_add_song_info(database* db, const char* hash, bool bOnServer, bool bOnClient);
void db_update_song_server_status(database* db, const char* hash, bool bOnServer);
wchar* db_get_file_local_song_copy(database* db, char** out_hash);
void db_adjust_song_upload_priority(database* db, const char* hash, int increase);

void db_add_local_file(database* db, const wchar* file, time64 time);
wchar* db_get_local_file_copy(database* db);
void db_add_local_file_hash(database* db, const wchar* file, const char* hash);

void db_add_local_dir(database* db, const wchar* file);
wchar* db_get_local_dir_copy(database* db);
void db_remove_local_dir(database* db, const wchar* dir);

void db_set_session_id(database* db, const char* sessionid);
wchar* db_get_session_id(database* db);
