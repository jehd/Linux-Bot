#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "libircclient-1.6/include/libircclient.h"
#include "sqlite3.h"
#include <time.h>
#include "dbase.h"



sqlite3 *dbhandle;

/**
*Methode trägt ein Event in die Sqlite3-Datenbank ein.
*
*@param event: Event-Name
*@param origin: Event-Quelle
*@param params: Event-Parameter
*/

void add_db_entry(const char * event, const char * origin, const char ** params)
{

	sqlite3_stmt *vm;
        char* sql_query;
	
	time_t now;
	now = time(NULL);
     

	sql_query = sqlite3_mprintf("INSERT INTO events (id, event, origin, params, time) VALUES ('1', '%s', '%s', '%s', '%s')", event, origin, params, now);
	sqlite3_finalize(vm);

	printf("hier_2\n");

}

/**
*Methode holt das gesamte Log aus der Datenbank.
*
*
*/

char* print_db()
{
	sql_query = sqlite3_mprintf("SELECT * FROM events)";
	irc_cmd_msg (session, params[0], "Hi all");
	sqlite3_finalize(vm);

	return sql_query;
}

/**
*Methode gibt aus, wann IRC-Chatter zum letzten Mal gesehen wurde.
*
*@param username: Benutzername des IRC-Chatters.
*/


char* last_seen(const char * username)
{
	sqlite3_stmt *vm;
	char* sql_query;
	sql_query = sqlite3_mprintf("Select time FROM 'event' WHERE origin = '%s' ORDER BY time DESC", username);
	
	sqlite3_prepare(dbhandle, sql_query, strlen(sql_query), &vm, NULL);

	sqlite3_finalize(vm);

	return sql_query;
}

/**
*Methode erstellt die Tabelle events. Wird zum Programmstart aufgerufen.
*
*@param dbhandlex: sqlite Pointer.
*/


void create_table(sqlite3 * dbhandlex)
{
	sqlite3_exec(dbhandlex, "CREATE TABLE events (id integer primary key, event text, origin text, params text, time text);", NULL, NULL, NULL);
	printf("TABLE CREATED!!!");

}
