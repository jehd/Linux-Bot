#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "libircclient.h"
#include "callbk.h"

#define DB_FILE "test.db"
sqlite3 *dbhandle;

irc_ctx_t ctx;


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

int main (int argc, char **argv)
{

	irc_callbacks_t	callbacks;
 	irc_session_t * s;
	unsigned short port;

	if ( argc != 4 )
	{
		printf ("Usage: %s <server> <nick> <channel>\n", argv[0]);
		return 1;
	}

	memset (&callbacks, 0, sizeof(callbacks));

	callbacks.event_connect = event_connect;
	callbacks.event_join = event_join;

	callbacks.event_nick = dump_event;
	callbacks.event_quit = dump_event;
	callbacks.event_part = dump_event;
	callbacks.event_mode = dump_event;
	callbacks.event_topic = dump_event;
	callbacks.event_kick = dump_event;
	callbacks.event_channel = event_channel;
	callbacks.event_privmsg = event_privmsg;
	callbacks.event_notice = dump_event;
	callbacks.event_invite = dump_event;
	callbacks.event_umode = dump_event; 


	callbacks.event_unknown = dump_event;
	callbacks.event_numeric = event_numeric;


	s = irc_create_session (&callbacks);


	sqlite3_open(DB_FILE, &dbhandle);
	create_table(dbhandle);




	if ( !s )
	{
		printf ("Could not create session\n");
		return 1;
	}
	
	ctx.channel = argv[3];
	ctx.nick = argv[2];

	irc_set_ctx (s, &ctx);

	// If the port number is specified in the server string, use the port 0 so it gets parsed
	if ( strchr( argv[1], ':' ) != 0 )
		port = 0;

	// To handle the "SSL certificate verify failed" from command line we allow passing ## in front 
	// of the server name, and in this case tell libircclient not to verify the cert
	if ( argv[1][0] == '#' && argv[1][1] == '#' )
	{
		// Skip the first character as libircclient needs only one # for SSL support, i.e. #irc.freenode.net
		argv[1]++;
		
		irc_option_set( s, LIBIRC_OPTION_SSL_NO_VERIFY );
	}
	
	// Initiate the IRC server connection
	if ( irc_connect (s, argv[1], port, 0, argv[2], 0, 0) )
	{
		printf ("Could not connect: %s\n", irc_strerror (irc_errno(s)));
		return 1;
	}

	// and run into forever loop, generating events
	if ( irc_run (s) )
	{
		printf ("Could not connect or I/O error: %s\n", irc_strerror (irc_errno(s)));
		return 1;
	}

	return 1;
}
