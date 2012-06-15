#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "libircclient.h"

typedef struct
{
	char 	* channel;
	char 	* nick;

} irc_ctx_t;

irc_ctx_t ctx;

void addlog (const char * fmt, ...)
{
	FILE * fp;
	char buf[1024];
	va_list va_alist;

	va_start (va_alist, fmt);
#if defined (WIN32)
	_vsnprintf (buf, sizeof(buf), fmt, va_alist);
#else
	vsnprintf (buf, sizeof(buf), fmt, va_alist);
#endif
	va_end (va_alist);

	printf ("%s\n", buf);

	if ( (fp = fopen ("irctest.log", "ab")) != 0 )
	{
		fprintf (fp, "%s\n", buf);
		fclose (fp);
	}
}

void dump_event (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
	char buf[512];
	int cnt;

	buf[0] = '\0';

	for ( cnt = 0; cnt < count; cnt++ )
	{
		if ( cnt )
			strcat (buf, "|");

		strcat (buf, params[cnt]);
	}


	addlog ("Event \"%s\", origin: \"%s\", params: %d [%s]", event, origin ? origin : "NULL", cnt, buf);
/*	add_db_entry(event , origin, params);*/
}



void event_join (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
	dump_event (session, event, origin, params, count);
	irc_cmd_user_mode (session, "+i");
	irc_cmd_msg (session, params[0], "Hi all");
}


void event_connect (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
	irc_ctx_t * ctx = (irc_ctx_t *) irc_get_ctx (session);
	dump_event (session, event, origin, params, count);

	irc_cmd_join (session, ctx->channel, 0);
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


	s = irc_create_session (&callbacks);


	if ( !s )
	{
		printf ("Could not create session\n");
		return 1;
	}
	
	ctx.channel = argv[3];
	ctx.nick = argv[2];

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
