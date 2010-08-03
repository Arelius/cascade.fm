#include "database.h"
#include "quick_string.h"
#include "ext/sqlite3.h"
#include "sys.h"

#include <assert.h>
#include <direct.h>
#include <shlobj.h>

const wchar* db_file_name = L"music.cas";
const wchar* db_app_data = L"\\Cascade\\0.1\\";

const wchar* search_dir_schema =
    L"CREATE TABLE search_directories (directory TEXT UNIQUE);";

const wchar* file_status_schema =
    L"CREATE TABLE file_status (filename TEXT UNIQUE, hash TEXT, modified INTEGER, scanned INTEGER);"; 

const wchar* song_info_schema =
    L"CREATE TABLE song_info (hash TEXT UNIQUE, exists_on_server INTEGER, exists_locally);";

// These should be stored in the database object.
struct database
{
    sqlite3* db;
    sqlite3_stmt* add_file_status_stmt;
};

// Just try to open and then close it to test.
bool db_exists(const wchar* db_file)
{
    return sys_file_exists(db_file);
}

database* db_open(const wchar* db_file)
{
    database* db = new database();
    sqlite3* sdb;
    int err = SQLITE_OK;
    if(db_file)
    {
        // Check for db_file.
        // Create db_file.
        err = sqlite3_open16(db_file, &sdb);
    }
    else
    {
        wchar buffer[MAX_PATH];
        QuickString<MAX_PATH> tmp_file;
        _wgetcwd(buffer, MAX_PATH);


        // Check for local file first.
        tmp_file.append_string(buffer);
        tmp_file.append_string(L"\\");
        tmp_file.append_string(db_file_name);
        if(db_exists(tmp_file.utf_16()))
        {
            err = sqlite3_open16(tmp_file.utf_16(), &sdb);
        }
        else
        {
            tmp_file.clear();

            // Check directory.
            // Create directory.
            SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, buffer);
            tmp_file.append_string(buffer);
            tmp_file.append_string(db_app_data);
            if(!sys_file_exists(tmp_file.utf_16()))
                sys_make_path(tmp_file.utf_16());

            // Check for AppData file.
            // Create AppData file.
            tmp_file.append_string(db_file_name);
            err = sqlite3_open16(tmp_file.utf_16(), &sdb);
        }
    }

    if(err != SQLITE_OK)
    {
        fwprintf(stderr,
                 L"DB Open Error: %s\n",
                 (const wchar*)sqlite3_errmsg16(sdb));
        return NULL;
    }
    
    db->db = sdb;
    return db;
}

void db_close(database* db)
{
    sqlite3_close(db->db);
    delete db;
}

void db_run_schema(database* db, const wchar* schema)
{
    sqlite3_stmt* statement;

    int err = sqlite3_prepare16(db->db, schema, -1, &statement, NULL);
    assert(err == SQLITE_OK);

    err = sqlite3_step(statement);
    assert(err == SQLITE_DONE);

    sqlite3_finalize(statement);    
}

void db_init(database* db)
{
    db_run_schema(db, search_dir_schema);
    db_run_schema(db, file_status_schema);
}

void db_add_search_dir(database* db, const wchar* dir)
{
    sqlite3_stmt* statement;

    int err = sqlite3_prepare16(db->db,
                                L"Insert into search_directories VALUES(?);",
                                -1, &statement, NULL);
    
    assert(err == SQLITE_OK);

    err = sqlite3_bind_text16(statement, 1, dir, -1, SQLITE_TRANSIENT);
    assert(err == SQLITE_OK);

    err = sqlite3_step(statement);
    assert(err == SQLITE_DONE);

    sqlite3_finalize(statement);
}

int db_rm_search_dir(database* db, const wchar* dir)
{
    sqlite3_stmt* statement;
    
    int err = sqlite3_prepare16(db->db,
                                L"Delete from search_directories where directory GLOB ?;",
                                -1, &statement, NULL);
    assert(err == SQLITE_OK);

    err = sqlite3_bind_text16(statement, 1, dir, -1, SQLITE_TRANSIENT);
    assert(err == SQLITE_OK);

    err = sqlite3_step(statement);
    assert(err == SQLITE_DONE);

    sqlite3_finalize(statement);

    return sqlite3_changes(db->db);
}

void db_print_search_dir(database* db, void (*print)(const wchar* dir, void* UP), void* UP)
{
    sqlite3_stmt* statement;

    int err = sqlite3_prepare16(db->db,
                                L"Select directory from search_directories;",
                                -1, &statement, NULL);
    assert(err == SQLITE_OK);

    err = sqlite3_step(statement);

    while(err == SQLITE_ROW)
    {
        wchar* dir = (wchar*)sqlite3_column_text16(statement, 0);
        print(dir, UP);
        err = sqlite3_step(statement);
    }
    assert(err == SQLITE_DONE);
}

