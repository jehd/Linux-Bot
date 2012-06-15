#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "libircclient-1.6/include/libircclient.h"
#include "sqlite3.h"

int logging = 0;

typedef struct
{
	char 	* channel;
	char 	* nick;

} irc_ctx_t;


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

void event_privmsg (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
	dump_event (session, event, origin, params, count);

	printf ("'%s' said me (%s): %s\n", 
		origin ? origin : "someone",
		params[0], params[1] );
}

void event_channel (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
	dump_event (session, event, origin, params, count);

	char nickbuf[128];

	if ( count != 2 )
		return;

	printf ("'%s' said in channel %s: %s\n", 
		origin ? origin : "someone",
		params[0], params[1] );

	if ( !origin )
		return;

	irc_target_get_nick (origin, nickbuf, sizeof(nickbuf));

	if ( !strcmp (params[1], "quit") )
		irc_cmd_quit (session, "of course, Master!");

	if ( !strcmp (params[1], "help") )
	{
		irc_cmd_msg (session, params[0], "quit, help, dcc chat, dcc send, ctcp");
	}

	 if ( !strcmp (params[1], "ctcp"))
	{
	
		printf("...> %s <...", params[1]);
		irc_cmd_ctcp_request (session, nickbuf, "PING 223");
		irc_cmd_ctcp_request (session, nickbuf, "FINGER");
		irc_cmd_ctcp_request (session, nickbuf, "VERSION");
		irc_cmd_ctcp_request (session, nickbuf, "TIME");
	}
        

	if ( !strcmp (params[1], "topic") )
		irc_cmd_topic (session, params[0], 0);
	else if ( strstr (params[1], "topic ") == params[1] )
		irc_cmd_topic (session, params[0], params[1] + 6);

	if ( strstr (params[1], "mode ") == params[1] )
		irc_cmd_channel_mode (session, params[0], params[1] + 5);

	if ( strstr (params[1], "nick ") == params[1] )
		irc_cmd_nick (session, params[1] + 5);

	if ( strstr (params[1], "whois ") == params[1] )
		irc_cmd_whois (session, params[1] + 5);


	char *pch;
	char search_string[100];
	strcpy (search_string, ctx.nick);	
	strncat (search_string, ":", 1);
	
	pch = strstr (params[1], search_string);
	if (pch != NULL)
	{
		printf("angesprochen!\n");
		
		pch = strstr (params[1], "logging off");
		if (pch != NULL)
			logging = 0;

		pch = strstr (params[1], "logging on");
		if (pch != NULL)
			logging = 1;
			

	}
}

void event_numeric (irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count)
{
	char buf[24];
	sprintf (buf, "%d", event);

	dump_event (session, buf, origin, params, count);
}
