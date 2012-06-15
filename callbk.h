
void event_join (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
	/*dump_event (session, event, origin, params, count);*/
	irc_cmd_user_mode (session, "+i");
	irc_cmd_msg (session, params[0], "Hi all");
}
