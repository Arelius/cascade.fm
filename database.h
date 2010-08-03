#include "utf.h"

struct sqlite3;
typedef sqlite3 database;

database* db_open(const wchar* db_file = 0);
void db_close(database* db);
void db_init(database* db);
void db_add_search_dir(database* db, const wchar* dir);
int db_rm_search_dir(database* db, const wchar* dir);
void db_print_search_dir(database* db, void (*print)(const wchar* dir, void* UP), void* UP);
