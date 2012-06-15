#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "libircclient-1.6/include/libircclient.h"
#include "sqlite3.h"
#include <time.h>



sqlite3 *dbhandle;

void add_db_entry(const char * event, const char * origin, const char ** params)
{
	sqlite3_stmt *vm;
        char* sql_query;
	
	time_t time = time(0);

        sql_query = sqlite3_mprintf("INSERT INTO events (event, origin, params) VALUES ('%s', '%s', '%s', '%s')", event, origin, params, time);
	sqlite3_finalize(vm);

}

void print_db()
{
	sqlite3_exec(dbhandle, "Select * FROM 'events'", NULL, NULL, NULL);
}

char* last_seen(const char * username)
{
	sqlite3_stmt *vm;
	char* sql_query;
	sql_query = sqlite3_mprintf("Select time FROM 'event' WHERE origin = '%s' ORDER BY time DESC", username);
	
	sqlite3_prepare(handle, sql_query, strlen(sql_query), &vm, NULL);

	sqlite3_finalize(vm);

	return sql_query;
}


void create_table(sqlite3 *handle)
{

	sqlite3_exec(handle, "CREATE TABLE events (id integer primary key, event text, origin text, params text, time date);", NULL, NULL, NULL);

}
