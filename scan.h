const size_t Hash_Buffer_Len = 16384;

void scan_all(struct database* db);
void hash_all(struct database* db);
bool hash_file(const wchar* filename, char* buffer);
