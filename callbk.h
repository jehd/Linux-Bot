#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "libircclient-1.6/include/libircclient.h"
#include "sqlite3.h"
#include <dlfcn.h>

int logging = 0;

typedef struct
{
	char 	* channel;
	char 	* nick;

} irc_ctx_t;
irc_ctx_t ctx;

/**
*Methode wird bei jedem IRC-Event aufgerufen und behandelt das Logging.
*
*@param session: aktuelle IRC Session
*@param event: Event-Typ
*@param origin: Event-Quelle
*@param params: Paremeter des Events, z.B. Chat-Text
*@param count: Azahl der Parameter
*/

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


	if (logging == 1)
	{
		void* myaddlog_handle;
		void (*addlog)();
		myaddlog_handle = dlopen("addlog.so", RTLD_NOW);
		addlog = (void(*)())dlsym(myaddlog_handle, "addlog");
		(*addlog) ("Event \"%s\", origin: \"%s\", params: %d [%s]", event, origin ? origin : "NULL", cnt, buf);
	}

	//else if (logging == 2)
		//add_db_entry(event , origin, params);
}

/**
*Methode wird aufgerufen, wenn ein Channel betreten wurde.
*Setzt obligatorisch den user mode und sendet eine Begrüßungs-Nachricht in den Chat.
*Ruft die Logging-Funktion auf.
*
*@param session: aktuelle IRC Session
*@param event: Event-Typ
*@param origin: Event-Quelle
*@param params: Paremeter des Events, z.B. Chat-Text
*@param count: Azahl der Parameter
*/

void event_join (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
	dump_event (session, event, origin, params, count);
	irc_cmd_user_mode (session, "+i");
	irc_cmd_msg (session, params[0], "Hi all");
}

/**
*Methode wird aufgerufen, wenn sich mit dem IRC-Server verbunden wurde.
*Weist der Variablen ctx die aktuelle session zu.
*Ruft die Logging-Funktion auf.
*
*@param session: aktuelle IRC Session
*@param event: Event-Typ
*@param origin: Event-Quelle
*@param params: Paremeter des Events, z.B. Chat-Text
*@param count: Azahl der Parameter
*/

void event_connect (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
	irc_ctx_t * ctx = (irc_ctx_t *) irc_get_ctx (session);
	dump_event (session, event, origin, params, count);

	irc_cmd_join (session, ctx->channel, 0);
}

/**
*Methode wird aufgerufen, wenn eine private IRC-Nachricht empfangen wurde.
*Ruft die Logging-Funktion auf.
*
*@param session: aktuelle IRC Session
*@param event: Event-Typ
*@param origin: Event-Quelle
*@param params: Paremeter des Events, z.B. Chat-Text
*@param count: Azahl der Parameter
*/

void event_privmsg (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
	dump_event (session, event, origin, params, count);

	printf ("'%s' said me (%s): %s\n", 
		origin ? origin : "someone",
		params[0], params[1] );
}

/**
*Methode wird aufgerufen, wenn eine Nachricht im Chat-room geschrieben wurde.
*Reagiert auf Chat-Nachrichten:
*topic: Ändert das Topic.
*nick: Ändert den Nickname.
*quit: Verlässt den IRC-Chat.
*mode: Ändert den Channel mode (z.B.: +t = geschütztes Topic)
*Botname: logging on: schaltet das logging ein.
*Botname: logging off: schatet das Logging aus.
*Ruft die Logging-Funktion auf.
*
*@param session: aktuelle IRC Session
*@param event: Event-Typ
*@param origin: Event-Quelle
*@param params: Paremeter des Events, z.B. Chat-Text
*@param count: Azahl der Parameter
*/

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
		irc_cmd_quit (session, "Quitting.");

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

/**
*Methode wird aufgerufen, wenn ein numerisches, nicht zugeordnetes Event eintritt.
*Loggt ausschließlich.
*
*@param session: aktuelle IRC Session
*@param event: Event-Typ
*@param origin: Event-Quelle
*@param params: Paremeter des Events, z.B. Chat-Text
*@param count: Azahl der Parameter
*/

void event_numeric (irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count)
{
	char buf[24];
	sprintf (buf, "%d", event);

	dump_event (session, buf, origin, params, count);
}
