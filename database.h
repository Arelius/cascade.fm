#include "utf.h"

struct sqlite3;
typedef sqlite3 database;

database* db_open(const wchar* db_file = 0);
void db_close(database* db);
void db_init(database* db);
