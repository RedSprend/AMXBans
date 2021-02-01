/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * AMX Bans - http://www.amxbans.net
 *  Include - cmdUnban
 * 
 * Copyright (C) 2014  Ryan "YamiKaitou" LeBlanc
 * Copyright (C) 2009, 2010  Thomas Kurz
 * Copyright (C) 2003, 2004  Ronald Renes / Jeroen de Rover
 * 
 * 
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at
 *  your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *  In addition, as a special exception, the author gives permission to
 *  link the code of this program with the Half-Life Game Engine ("HL
 *  Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *  L.L.C ("Valve"). You must obey the GNU General Public License in all
 *  respects for all of the code used other than the HL Engine and MODs
 *  from Valve. If you modify this file, you may extend this exception
 *  to your version of the file, but you are not obligated to do so. If
 *  you do not wish to do so, delete this exception statement from your
 *  version.
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#if defined _cmdunban_included
    #endinput
#endif
#define _cmdunban_included

public cmdUnban(id, level, cid)
{
	/* Checking if the admin has the right access */
	if( !cmd_access(id, level, cid, 2) )
		return PLUGIN_HANDLED;

	read_argv(1, g_choicePlayerAuthid[id], charsmax(g_choicePlayerAuthid[]));

	if( equali(g_choicePlayerAuthid[id], "STEAM_ID_PENDING") ||
		equali(g_choicePlayerAuthid[id], "STEAM_ID_LAN") ||
		equali(g_choicePlayerAuthid[id], "VALVE_ID_LAN") ||
		equali(g_choicePlayerAuthid[id], "127.0.0.1") )
	{
		console_print( id, "Invalid authorization ID." );
		return PLUGIN_HANDLED;
	}

	new authid[35], authip[20], szQuery[512], len;
	len = formatex(szQuery, charsmax(szQuery), "SELECT `bid` FROM `%s%s` WHERE ", g_dbPrefix, tbl_bans);
	if( contain(g_choicePlayerAuthid[id], "STEAM_0:") == 0 || contain(g_choicePlayerAuthid[id], "VALVE_") == 0 )
	{
		copy(authid, charsmax(authid), g_choicePlayerAuthid[id]);
		len += formatex(szQuery[len], charsmax(szQuery)-len, "`player_id` = '%s'", authid);
	}
	else
	{
		copy(authip, charsmax(authip), g_choicePlayerAuthid[id]);
		len += formatex(szQuery[len], charsmax(szQuery)-len, "`player_ip` = '%s'", authip);
	}
	len += formatex(szQuery[len], charsmax(szQuery)-len, " AND `expired` = 0");

	console_print(id, "Searching for %s in database...", g_choicePlayerAuthid[id]);

	new data[1];
	data[0] = id;

	SQL_ThreadQuery(g_SqlX, "cmd_unban_1", szQuery, data, sizeof(data));

	return PLUGIN_HANDLED;
}

public cmd_unban_1(failstate, Handle:query, error[], errnum, data[], size)
{
	if( failstate )
	{
		new szQuery[256];
		SQL_GetQueryString(query, szQuery, charsmax(szQuery));
		MySqlX_ThreadError(szQuery, error, errnum, failstate, 6);
		return PLUGIN_HANDLED;
	}

	new id = data[0];

	new bool:serverCmd = false;
	/* Determine if this was a server command or a command issued by a player in the game */
	if( id == 0 )
		serverCmd = true;

	if( SQL_NumResults(query) == 1 )
	{
		new banid = SQL_ReadResult(query, 0);
		new admin_nick[64];
		mysql_get_username_safe(id, admin_nick, charsmax(admin_nick));

		SQL_FreeHandle(query);

		if( serverCmd )
		{
			new servernick[100];
			get_pcvar_string(pcvar_server_nick, servernick, charsmax(servernick));
			if (strlen(servernick))
				copy(admin_nick, charsmax(admin_nick), servernick);
		}

		new pquery[512];
		formatex(pquery, charsmax(pquery), "INSERT INTO `%s%s` \
			(`bid`, `edit_time`, `admin_nick`, `edit_reason`) \
			VALUES ('%d', UNIX_TIMESTAMP(NOW()), '%s', 'In-Game Unban')", \
			g_dbPrefix, tbl_bans_edit, banid, admin_nick);

		new szData[2];
		szData[0] = id;
		szData[1] = banid;

		SQL_ThreadQuery(g_SqlX, "cmd_unban_2", pquery, szData, sizeof(szData));
	}
	else if( SQL_NumResults(query) == 0 )
	{
		SQL_FreeHandle(query);

		if(serverCmd)
			server_print("[AMXBans] %L", LANG_SERVER, "PLAYER_NOT_BANNED", g_choicePlayerAuthid[id]);
		else
			console_print(id, "%L", id, "PLAYER_NOT_BANNED", g_choicePlayerAuthid[id]);
	}
	else
	{
		if(serverCmd)
			server_print("[AMXBans] %L", LANG_SERVER, "TOO_MANY_BANS", g_choicePlayerAuthid[id], SQL_NumResults(query));
		else
			console_print(id, "%L", id, "TOO_MANY_BANS", g_choicePlayerAuthid[id], SQL_NumResults(query));

		SQL_FreeHandle(query);
	}

	return PLUGIN_HANDLED;
}

public cmd_unban_2(failstate, Handle:query, error[], errnum, data[], size)
{
	if( failstate )
	{
		new szQuery[256];
		SQL_GetQueryString(query, szQuery, charsmax(szQuery));
		MySqlX_ThreadError(szQuery, error, errnum, failstate, 6);
		return PLUGIN_HANDLED;
	}

	SQL_FreeHandle(query);

	new id = data[0];
	new banid = data[1];

	new pquery[512];
	formatex(pquery, charsmax(pquery), "UPDATE `%s%s` \
		SET `ban_length` = '-1', `expired` = '1' \
		WHERE `bid` = %d", \
		g_dbPrefix, tbl_bans, banid);

	new szData[1];
	szData[0] = id;

	SQL_ThreadQuery(g_SqlX, "cmd_unban_3", pquery, szData, sizeof(szData));

	return PLUGIN_HANDLED
}

public cmd_unban_3(failstate, Handle:query, error[], errnum, data[], size)
{
	if( failstate )
	{
		new szQuery[256];
		SQL_GetQueryString(query, szQuery, charsmax(szQuery));
		MySqlX_ThreadError(szQuery, error, errnum, failstate, 6);
		return PLUGIN_HANDLED;
	}

	new id = data[0];

	new bool:serverCmd = false;
	/* Determine if this was a server command or a command issued by a player in the game */
	if (id == 0)
		serverCmd = true;

	if( serverCmd )
	{
		server_print("%L", "UNBAN_CONSOLE", LANG_SERVER, g_choicePlayerAuthid[id]);
	}
	else
	{
		console_print(id, "%L", id, "UNBAN_CONSOLE", g_choicePlayerAuthid[id]);

		new name[32]
		get_user_name(id, name, charsmax(name))
		for( new i = 1; i <= MaxClients; i++ )
		{
			if( is_user_bot(i) || is_user_hltv(i) || !is_user_connected(i) || !is_user_admin(i) )
				continue;

			client_print(id, print_chat, "ADMIN %s: ban %s", name, g_ident)
		}
	}

	log_amx("[AMXBans] %L", "LOG_UNBAN", LANG_SERVER, "CONSOLE", 0, "CONSOLE", "", g_choicePlayerAuthid[id]);

	SQL_FreeHandle(query);

	return PLUGIN_HANDLED;
}