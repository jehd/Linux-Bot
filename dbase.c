#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "libircclient-1.6/include/libircclient.h"
#include "sqlite3.h"

#define DB_FILE "test.db"

sqlite3 *dbhandle;

void add_db_entry(const char * event, const char * origin, const char ** params)
{
	sqlite3_exec(dbhandle, "INSERT INTO customer (firstname, lastname) VALUES (event, params)", NULL, NULL, NULL);
}

void print_db()
{
	sqlite3_exec(dbhandle, "Select * FROM 'customer'", NULL, NULL, NULL);
}


void create_table(sqlite3 *handle)
{

	sqlite3_exec(handle, "CREATE TABLE customer (id integer primary key, firstname text, lastname text);", NULL, NULL, NULL);

}
