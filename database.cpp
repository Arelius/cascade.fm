#include "database.h"
#include "quick_string.h"
#include "ext/sqlite3.h"
#include "sys.h"
#include "utf.h"

#include <assert.h>
#include <direct.h>
#include <shlobj.h>

const wchar* db_file_name = L"music.cas";
const wchar* db_app_data = L"\\Cascade\\";

const wchar* search_dir_schema =
    L"CREATE TABLE search_directories (directory TEXT UNIQUE);";

const wchar* file_status_schema =
    L"CREATE TABLE file_status (filename TEXT UNIQUE, hash_id TEXT, modified INTEGER);"; 

const wchar* dir_scan_schema =
    L"CREATE TABLE scan_directories (directory TEXT UNIQUE);";

const wchar* song_info_schema =
    L"CREATE TABLE song_info (id UNIQUE PRIMARY KEY, hash TEXT UNIQUE, exists_on_server INTEGER, exists_locally);";

// These should be stored in the database object.
struct database
{
    sqlite3* db;

    // db: file_status
    sqlite3_stmt* add_file_status_stmt;
    sqlite3_stmt* check_file_status_stmt;

    // db: scan_directories
    sqlite3_stmt* add_scan_dir_stmt;
    sqlite3_stmt* rm_scan_dir_stmt;
    sqlite3_stmt* check_scan_dir_stmt;
};

void db_dump_error(database* db)
{
    wprintf(L"%s\n", (wchar*)sqlite3_errmsg16(db->db));
}

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

    // Could do this later to avoid the case it's never used.

    err = sqlite3_prepare16(db->db,
                            L"SELECT filename, modified from file_status where filename == ?;",
                            -1, &db->check_file_status_stmt, NULL);

    err = sqlite3_prepare16(db->db,
                            L"INSERT OR REPLACE INTO file_status (filename, hash_id, modified) VALUES (?, ?, ?);",
                            -1, &db->add_file_status_stmt, NULL);

    err = sqlite3_prepare16(db->db,
                            L"INSERT OR IGNORE INTO scan_directories (directory) VALUES (?);",
                            -1, &db->add_scan_dir_stmt, NULL);

    err = sqlite3_prepare16(db->db,
                            L"DELETE FROM scan_directories where directory = ?;",
                            -1, &db->rm_scan_dir_stmt, NULL);

    err = sqlite3_prepare16(db->db,
                            L"SELECT directory from scan_directories;",
                            -1, &db->check_scan_dir_stmt, NULL);

    return db;
}

void db_close(database* db)
{
    sqlite3_finalize(db->add_file_status_stmt);
    sqlite3_finalize(db->check_file_status_stmt);
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
    db_run_schema(db, dir_scan_schema);
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

void db_inject_library_directories(database* db)
{
    sqlite3_stmt* statement;

    int err = sqlite3_prepare16(db->db,
                                L"INSERT OR IGNORE INTO scan_directories (directory) SELECT directory from search_directories;",
                                -1, &statement, NULL);
    assert(err == SQLITE_OK);

    err = sqlite3_step(statement);
    assert(err == SQLITE_DONE);

    sqlite3_finalize(statement);
}

void db_add_local_file(database* db, const wchar* file, time64 time)
{
    int err = sqlite3_bind_text16(db->check_file_status_stmt, 1, file, -1, SQLITE_TRANSIENT);
    assert(err == SQLITE_OK);

    err = sqlite3_step(db->check_file_status_stmt);
    
    if(err == SQLITE_ROW)
    {
        if(sqlite3_column_int64(db->check_file_status_stmt, 1) == time)
        {
            sqlite3_reset(db->check_file_status_stmt);
            return;
        }
        err = sqlite3_step(db->check_file_status_stmt);
    }
    
    // Unique constraint ensure only one result.
    assert(err == SQLITE_DONE);
    sqlite3_reset(db->check_file_status_stmt);

    // Add it newly to the db, if a differen't time exists.

    err = sqlite3_bind_text16(db->add_file_status_stmt, 1, file, -1, SQLITE_TRANSIENT);
    assert(err == SQLITE_OK);
    err = sqlite3_bind_null(db->add_file_status_stmt, 2);
    assert(err == SQLITE_OK);
    err = sqlite3_bind_int64(db->add_file_status_stmt, 3, time);
    assert(err == SQLITE_OK);

    err = sqlite3_step(db->add_file_status_stmt);
    assert(err == SQLITE_DONE);

    sqlite3_reset(db->add_file_status_stmt);
}

void db_add_local_dir(database* db, const wchar* file)
{
    int err = sqlite3_bind_text16(db->add_scan_dir_stmt, 1, file, -1, SQLITE_TRANSIENT);
    assert(err == SQLITE_OK);

    err = sqlite3_step(db->add_scan_dir_stmt);
    assert(err == SQLITE_DONE);
    
    sqlite3_reset(db->add_scan_dir_stmt);
}

wchar* db_get_local_dir_copy(database* db)
{
    int err = sqlite3_step(db->check_scan_dir_stmt);
    if(err == SQLITE_DONE)
    {
        sqlite3_reset(db->check_scan_dir_stmt);
        return NULL;
    }
    assert(err == SQLITE_ROW);

    const wchar* str = (wchar*)sqlite3_column_text16(db->check_scan_dir_stmt, 0);

    wchar* ret = (wchar*)malloc(str_byte_length(str));
    return str_copy(ret, str);
}

void db_remove_local_dir(database* db, const wchar* dir)
{
    int err = sqlite3_bind_text16(db->rm_scan_dir_stmt, 1, dir, -1, SQLITE_TRANSIENT);
    assert(err == SQLITE_OK);

    err = sqlite3_step(db->rm_scan_dir_stmt);
    assert(err == SQLITE_DONE);

    sqlite3_reset(db->rm_scan_dir_stmt);
}
