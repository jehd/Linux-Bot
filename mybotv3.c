#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "libircclient.h"
#include "callbk.h"
#include <dlfcn.h>
#define DB_FILE "test.db"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "daemon.h"

void* mylib_handle;

sqlite3 *dbhandle;



void (*create_table)();



/**
*Main Funktion
*
*Verweist die IRC Events auf die jeweiligen Funktionen.
*Nimmt die Benutzereingabe entgegen.
*Startet Endlossschleife.
*
*@param argv[1] irc server
*@param argv[2] nickname
*@param argv[3] channel
*@param argv[4] logging-Plugins
*/

int main (int argc, char **argv)
{
	//daemonize();

	int i;
	
	irc_callbacks_t	callbacks;
 	irc_session_t * s;
	unsigned short port;



	if ( argc < 4 )
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


	
	if ( !s )
	{
		printf ("Could not create session\n");
		return 1;
	}

	ctx.channel = argv[3];
	ctx.nick = argv[2];

	irc_set_ctx (s, &ctx);

	if (argv[4] != NULL)
	{
	

		if (strcmp(argv[4], "txt") == 0)
		{
			mylib_handle = dlopen("txtlogger.so", RTLD_NOW);
			logging = 1;
		}
		else if (strcmp(argv[4], "db") == 0)
		{

			


			mylib_handle = dlopen("dbase.so", RTLD_NOW);



			if (mylib_handle == NULL)
			{
				printf("%s}n", dlerror());
				return 1;
			}
	

			sqlite3_open(DB_FILE, &dbhandle);
//			init_mylib_handle(dbhandle);

			create_table = (void(*)()) dlsym(mylib_handle, "create_table");

			if (create_table == NULL)
			{
				printf ("%s\n", dlerror());
				return 1;
			}		

			(*create_table)(dbhandle);
			sqlite3_close(dbhandle);
printf("hier_c\n");
			
				logging = 2;
				printf("logging to db started...");
			


//		}

	}

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
