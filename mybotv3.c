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

void* mylib_handle;

sqlite3 *dbhandle;

sqlite3 *dbhandle;

static void daemonize()
{
  pid_t pid, sid;

  if (getppid() == 1) return;

  pid = fork();
  if ( pid < 0 ) exit (1);

  /* Wenn ein Kindprozess erzeugt wurde, kann der 
     Eltern Prozess sich beenden 
  */

  if (pid > 0) exit (0);

  /* Neues file mode mask */
  umask(0);

  /* Neue SID Umgebung */
  sid = setsid();
  if (sid < 0) exit(1);

  /* Aendern des Arbeitsverzeichnisses */
  if ((chdir("/") < 0)) exit(1); 

  /* Aendern der default Filedescriptoren */
  freopen("/dev/null", "r", stdin);
  freopen("/dev/null", "w", stdout);
  freopen("/dev/null", "w", stderr);
}

void (*create_table)();

int main (int argc, char **argv)
{
	daemonize();

	mylib_handle = dlopen("libdb.so", RTLD_NOW);
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


	create_table = (void(*)()) dlsym(mylib_handle, "create_table");

	(*create_table)(dbhandle);

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
