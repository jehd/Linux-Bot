/** @mainpage XCTL 
 *  
 *  @section sec1 Einleitung 
 *  Dies ist die Dokumention des IRC-Bots. Der Bot wird über die Kommandozeile gestartet, z.B.
 *  ./mybotv3 irc.quakenet.org:6667 mynick db.
 *  
 *  (Für nähere Informationen siehe manpage.)
 *  @section sec1 Überblick
 *  @section sec1_1 Strukturierung des Codes
 *   - Main-Funktion (mybotv3.c)
 *   - Dämonisierung (daemon.h)
 *   - Sqlite3-Logging (dbase.h, dbase.c, dbase.so)
 *   - DB-Loggin (dbase.h, dbase.c, dbase.so)
 *   - Events (callbk.h)
 *  @section sec1_2 README
 *  Readme vorhanden (README.md)
 *  @section sec1_3 License
 *  Lizenzbestimmungen (LGPL) in license.txt
 *  @section sec1_4 Manpage
 *  Manpage vorhanden (mymanpage)
 *  @section sec1_5 Makefile
 *  Manpage vorhanden (Makefile.h)
 * 
 *  @section sec2 Funktionalitäten
 *  @subsection sec2_1 Grundfunktionalitäten
 *   - Der Bot implementiert die libircclient-Bibliothek (/libircclient-1.6)
 *   - Der Bot kann sich auf einem IRC-Server anmelden (mybotv3.c -> irc_connect)
 *   - Der Bot kann sich in einem Channel anmelden. (callbk.h -> event_connect -> irc_cmd_join)
 *   - Servername und Channel werden über die Kommandozeile als Parameter übergeben. (z.B. ./mybotv3 irc.quakenet.org:6667 mynick db)
 *   - Der Name des Bots wird über die Kommandozeile angegeben.
 *   - Der Bot hat weitere Steuerungsmöglichkeiten.
 *  @subsection sec2_2 Steuerung des Bots
 *   - Der Bot reagiert auf "Botname: xxx". (callbk.h -> event_channel)
 *   - Das Logging ist per IRC-Commnand an/abschaltbar. "logging on" "logging off". (callbk.h -> event_channel)
 *   - Der Bot gibt ein lastseen an. (callbk.h -> event_channel -> dbase.c)
 *   - Der Bot gibt das log in den Chat aus. (callbk.h -> event_channel -> dbase.c)
 *  @subsection sec2_3 Dämonisieren
 *  Bot wird über daemonize() zu Beginn der main dämonisiert (mybotv3.c -> daemon.h);  
 *  @subsection sec2_4 Plugin-Mechanismus
 *  Logging über Plugin gelöst (dbase.so)
 *  @subsection sec2_5 Weitere Plugins
 *  Logging in Textdatei (txtlogger.so)
 *  @subsection sec2_5 Private Nachrichten
 *  Der Bot reagiert auf private Nachrichten (callbk.h -> event_privmsg)
 *  @subsection sec2_6 Per Command Topic ändern, Nick change etc.
 *  Der Bot ändert auf Befehl das Topic, seinen Nick etc. (callbk.h -> event-channel)
 */
