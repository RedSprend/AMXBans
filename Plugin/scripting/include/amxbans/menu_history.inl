/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * AMX Bans - http://www.amxbans.net
 *  Include - menu_history
 * 
 * Copyright (C) 2020  Rick
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
 
// TO-DO:
// Threaded queries are capable of throwing a "out of bounds" error if the offender disconnected while the threaded query is still running (aka player index becomes invalid).
// Store the authid and retrieve the info from that instead of player index.

#if defined _menu_history_included
    #endinput
#endif
#define _menu_history_included

#include "unixtime.inc"

stock const MonthName[][] = { "Invalid month", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" }
#define NumberToMonth(%1) MonthName[%1]

#include <amxmodx>

public cmdBanhistoryMenu(id,level,cid)
{
	if (!cmd_access(id,level,cid,1))
		return PLUGIN_HANDLED

	new menu = menu_create("menu_history","actionHistoryMenu")
	new callback=menu_makecallback("callback_MenuGetBannedPlayers")

	MenuSetProps(id,menu,"BANHISTORY_MENU")
	if( !MenuGetBannedPlayers(menu,callback) )
	{
		menu_destroy(menu)
		client_print(id, print_chat, "%L", LANG_PLAYER, "NO_BANNED_PLAYERS")
		return PLUGIN_HANDLED
	}

	menu_display(id,menu,0)

	return PLUGIN_HANDLED
}

public actionHistoryMenu(id,menu,item)
{
	if(item < 0)
	{
		menu_destroy(menu)
		return PLUGIN_HANDLED
	}

	new acc,szInfo[3],szText[128],callb
	menu_item_getinfo(menu,item,acc,szInfo,charsmax(szInfo),szText,charsmax(szText),callb)

	new pid = str_to_num(szInfo)

	menu_destroy(menu)

	new data[2]
	data[0] = id
	data[1] = pid

	new pquery[1024]

	if(g_supported_game)
	{
		format(pquery, charsmax(pquery), "SELECT amxban_motd FROM `%s%s` WHERE address = '%s:%s'", g_dbPrefix, tbl_serverinfo, g_ip, g_port)
	}
	else
	{
		new authid[MAX_AUTHID_LENGTH], ip[MAX_IP_LENGTH]
		get_user_authid(pid, authid, charsmax(authid))
		get_user_ip(id, ip, charsmax(ip), 1)

		formatex(pquery, charsmax(pquery), "SELECT COUNT(*) FROM `%s%s` WHERE (player_id='%s' AND ban_type='S' OR player_id='%s' AND ban_type='SI' OR player_ip='%s' AND ban_type='SI') AND expired=1", g_dbPrefix, tbl_bans, authid, authid, ip)
		//formatex(pquery, charsmax(pquery), "SELECT COUNT(*) FROM `%s%s` WHERE player_id='%s' AND expired=1", g_dbPrefix, tbl_bans, authid)
	}

	SQL_ThreadQuery(g_SqlX, "select_motd_history", pquery, data, sizeof(data))

	return PLUGIN_HANDLED
}

public select_motd_history(failstate, Handle:query, error[], errnum, data[], size)
{
	if (failstate)
	{
		new szQuery[256]
		MySqlX_ThreadError( szQuery, error, errnum, failstate, 9 )
		return PLUGIN_HANDLED
	}

	new id = data[0]
	new pid = data[1]

	if(!pid)
	{
		client_print(id, print_chat, "%i is not a valid player (player left the game?)", pid)
		return PLUGIN_HANDLED
	}

	new name[MAX_NAME_LENGTH], authid[MAX_AUTHID_LENGTH], ip[MAX_IP_LENGTH]
	get_user_name(pid, name, charsmax(name))
	get_user_authid(pid, authid, charsmax(authid))
	get_user_ip(pid, ip, charsmax(ip), 1)

	new szTitle[128]
	formatex(szTitle, charsmax(szTitle), "%L", LANG_PLAYER, "HISTORY_MOTD", name, authid)

	if(!g_supported_game)
	{
		new ban_count = SQL_ReadResult(query, 0)

		SQL_FreeHandle(query)

		if( ban_count == 0 )
		{
			client_print(id, print_chat, "%L", LANG_PLAYER, "NO_BAN_HISTORY", name)
			return PLUGIN_HANDLED
		}

		new szQuery[1024]
		formatex(szQuery, charsmax(szQuery), "SELECT b.ban_created,b.ban_reason,b.ban_length,b.admin_nick,aa.nickname \
			FROM `%s%s` AS b, `%s_amxadmins` AS aa \
			WHERE b.player_id='%s' AND (aa.steamid=b.admin_nick OR aa.steamid=b.admin_ip OR aa.steamid=b.admin_id OR aa.nickname=b.admin_id OR aa.nickname=b.admin_nick) AND b.expired=1 ORDER BY b.ban_created DESC",
			g_dbPrefix, tbl_bans, g_dbPrefix, authid)

		new pData[4]
		pData[0] = id
		pData[1] = pid
		pData[2] = ban_count

		SQL_ThreadQuery(g_SqlX, "player_ban_history", szQuery, pData, sizeof(pData))

		return PLUGIN_HANDLED
	}

	new motd_url[256]

	if(!SQL_NumResults(query))
	{
		SQL_FreeHandle(query)
		return PLUGIN_HANDLED
	}

	SQL_ReadResult(query, 0, motd_url, charsmax(motd_url))

	//http://URL/motd.php?sid=%s&adm=%d&lang=%s
	if(contain(motd_url,"?sid=%s&adm=%d&lang=%s") != -1)
	{
		new url[256], lang[5]

		get_user_info(id,"lang",lang,charsmax(lang))
		if(equal(lang,""))
			get_cvar_string("amx_language",lang,charsmax(lang))

		//copy(url,charsmax(url),g_motdurl)
		formatex(url,charsmax(url),motd_url,authid,1,lang)
		if(get_pcvar_num(pcvar_debug) >= 2)
			log_amx("[AMXBans BanHistory Motd] %s",url)

		show_motd(id, url, szTitle)
	}
	else
	{
		log_amx("[AMXBans ERROR BanHistory] %L",LANG_SERVER,"NO_MOTD")
		client_print(id,print_chat,"%L",id,"NO_MOTD")
		//ColorChat(id, RED, "[AMXBans]^x01 %L",id,"NO_MOTD")
	}

	SQL_FreeHandle(query)

	return PLUGIN_HANDLED
}

public player_ban_history(failstate, Handle:szQuery, error[], errnum, data[], size)
{
	if(failstate)
	{
		new szQuery[256]
		MySqlX_ThreadError( szQuery, error, errnum, failstate, 9 )
		return PLUGIN_HANDLED
	}

	new id = data[0]
	new pid = data[1]
	new ban_count = data[2]

	if(!id)
		return PLUGIN_HANDLED

	if(!pid)
	{
		client_print(id, print_chat, "%i is not a valid player (player left the game?)", pid)
		return PLUGIN_HANDLED
	}

	new authid[MAX_AUTHID_LENGTH]
	get_user_authid(pid, authid, charsmax(authid))

	if(!SQL_NumRows(szQuery))
	{
		SQL_FreeHandle(szQuery)

		new puery[1024]
		formatex(puery, charsmax(puery), "SELECT ban_created,ban_reason,ban_length,admin_nick FROM `%s%s` WHERE player_id='%s' AND expired=1 ORDER BY ban_created DESC", g_dbPrefix, tbl_bans, authid)

		new pData[5]
		pData[0] = id
		pData[1] = pid
		pData[2] = ban_count
		pData[3] = true

		SQL_ThreadQuery(g_SqlX, "player_ban_history", puery, pData, sizeof(pData))

		return PLUGIN_HANDLED
	}

	new bool:bNoLongerAdmin = false
	if( data[3] )
		bNoLongerAdmin = true

	new iBanCreated, szReason[128], iBanLength, szAdminNickname[MAX_NAME_LENGTH], szAdminName[MAX_NAME_LENGTH]
	new iYear, iMonth, iDay, iHour, iMinute, iSecond

	new szMsg[1024], len

	new name[MAX_NAME_LENGTH]
	get_user_name(pid, name, charsmax(name))

	client_print(id, print_console, "=================================")
	client_print(id, print_console, "%L", LANG_PLAYER, "HISTORY_MOTD", name, authid)
	client_print(id, print_console, "%L (%i)", LANG_PLAYER, "EXPIRED_BANS", ban_count )

	new rows = SQL_NumRows(szQuery)
	while(SQL_MoreResults(szQuery))
	{
		iBanCreated = SQL_ReadResult(szQuery, 0)
		SQL_ReadResult(szQuery, 1, szReason, charsmax(szReason))
		iBanLength = SQL_ReadResult(szQuery, 2)
		SQL_ReadResult(szQuery, 3, szAdminNickname, charsmax(szAdminNickname))
		if( !bNoLongerAdmin )
			SQL_ReadResult(szQuery, 4, szAdminName, charsmax(szAdminName))

		UnixToTime(iBanCreated, iYear, iMonth, iDay, iHour, iMinute, iSecond)

		formatex(szMsg, charsmax(szMsg), "^n%L: %02d %s %d - %02d:%02d:%02d", LANG_PLAYER, "INVOKED", iDay, NumberToMonth(iMonth), iYear, iHour, iMinute, iSecond)
		client_print(id, print_console, "%s", szMsg)
		formatex(szMsg, charsmax(szMsg), "%L", LANG_PLAYER, "MSG_2", szReason)
		client_print(id, print_console, "%s", szMsg)
		formatex(szMsg, charsmax(szMsg), "%L: %s", LANG_PLAYER, "BAN_LENGTH", get_time_length_ex(iBanLength * 60))
		client_print(id, print_console, "%s", szMsg)
		if( bNoLongerAdmin )
			formatex(szMsg, charsmax(szMsg), "%L", LANG_PLAYER, "MSG_5", szAdminNickname)
		else if( !equali(szAdminNickname, szAdminName) )
			formatex(szMsg, charsmax(szMsg), "%L (%s)", LANG_PLAYER, "MSG_6", szAdminNickname, szAdminName)
		else
			formatex(szMsg, charsmax(szMsg), "%L", LANG_PLAYER, "MSG_6", szAdminNickname)
		client_print(id, print_console, "%s", szMsg)

		if( --rows == 0 ) // reached last row
			break

		SQL_NextRow(szQuery)
	}

	client_print(id, print_console, "=================================")

	len = 0
	formatex(szMsg[len], charsmax(szMsg) - len, "%L", LANG_PLAYER, "CHECK_CONSOLE")
	client_print(id, print_chat, "* %L", LANG_PLAYER, "CHECK_CONSOLE")

	SQL_FreeHandle(szQuery)

	return PLUGIN_HANDLED
}