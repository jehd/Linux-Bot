void add_db_entry(const char * event, const char * origin, const char ** params);
char* print_db();
char* last_seen(const char * username);
void create_table(sqlite3 * dbhandlex);
