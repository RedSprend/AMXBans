/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * AMX Bans - http://www.amxbans.net
 *  Plugin - Main
 * 
 * Copyright (C) 2014  Ryan "YamiKaitou" LeBlanc
 * Copyright (C) 2009, 2010  Thomas Kurz
 * Copyright (C) 2003, 2004  Ronald Renes / Jeroen de Rover
 * Forked from "Admin Base (SQL)" in AMX Mod X (version 1.8.1)
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

#define PLUGINNAME "AMXBans Main"
#define PLUGINAUTHOR "YamiKaitou, Rick"
new const PLUGINVERSION[] = "6.13.1";

#include <amxmodx>
#include <amxmisc>
#include <sqlx>
#include <time>

// Amxbans Core natives
#include "include/amxbans_core.inc"

// Amxbans .inl files
#include "include/amxbans/global_vars.inl"
//#include "include/amxbans/color_chat.inl"
#include "include/amxbans/init_functions.inl"
#include "include/amxbans/check_player.inl"
#include "include/amxbans/check_flag.inl"
#include "include/amxbans/menu_stocks.inl"
#include "include/amxbans/menu_ban.inl"
#include "include/amxbans/menu_disconnected.inl"
#include "include/amxbans/menu_history.inl"
#include "include/amxbans/menu_flag.inl"
#include "include/amxbans/cmdBan.inl"
#include "include/amxbans/cmdUnban.inl"
#include "include/amxbans/web_handshake.inl"

// 16k * 4 = 64k stack size
#pragma dynamic 16384 		// Give the plugin some extra memory to use

new szGame[20];

public plugin_init()
{
	register_plugin(PLUGINNAME, PLUGINVERSION, PLUGINAUTHOR)

	register_dictionary("amxbans.txt")
	register_dictionary("common.txt")
	register_dictionary("time.txt")

	get_modname(szGame, charsmax(szGame));

	g_supported_game = false;

	register_clcmd("admin_banmenu", "cmdBanMenu", ADMIN_BAN, "- displays ban menu")
	register_clcmd("custom_ban_reason", "setCustomBanReason", ADMIN_BAN, "- configures custom ban message (only available via ban menu)")
	register_clcmd("admin_banhistorymenu", "cmdBanhistoryMenu", ADMIN_BAN, "- displays ban history of players menu")
	register_clcmd("admin_disconnectedmenu", "cmdBanDisconnectedMenu", ADMIN_BAN, "- displays disconnected players menu")
//	register_clcmd("admin_flaggingmenu","cmdFlaggingMenu",ADMIN_BAN,"- displays flagging menu")

	register_srvcmd("amx_sethighbantimes", "setHighBantimes")
	register_srvcmd("amx_setlowbantimes", "setLowBantimes")
	register_srvcmd("amx_setflagtimes","setFlagTimes")

	register_srvcmd("amx_reloadreasons", "cmdFetchReasons")

	pcvar_serverip		=	register_cvar("amxbans_server_address","", FCVAR_PROTECTED)
	pcvar_server_nick 	= 	register_cvar("amxbans_servernick", "", FCVAR_PROTECTED)
	pcvar_discon_in_banlist	=	register_cvar("amxbans_discon_players_saved","10", FCVAR_PROTECTED)
	pcvar_complainurl	= 	register_cvar("amxbans_complain_url", "www.yoursite.com", FCVAR_PROTECTED) // Dont use http:// then the url will not show
	pcvar_debug 		= 	register_cvar("amxbans_debug", "0", FCVAR_PROTECTED) // Set this to 1 to enable debug
	pcvar_add_mapname	=	register_cvar("amxbans_add_mapname_in_servername", "0", FCVAR_PROTECTED)
	pcvar_flagged_all	=	register_cvar("amxbans_flagged_all_server","1", FCVAR_PROTECTED)
	pcvar_show_in_hlsw 	= 	register_cvar("amxbans_show_in_hlsw", "1", FCVAR_PROTECTED)
	pcvar_show_hud_messages	= 	register_cvar("amxbans_show_hud_messages", "1", FCVAR_PROTECTED)
	pcvar_higher_ban_time_admin = 	register_cvar("amxbans_higher_ban_time_admin", "n", FCVAR_PROTECTED)
	pcvar_admin_mole_access = 	register_cvar("amxbans_admin_mole_access", "r", FCVAR_PROTECTED)
	pcvar_show_name_evenif_mole = 	register_cvar("amxbans_show_name_evenif_mole", "1", FCVAR_PROTECTED)
	pcvar_custom_statictime =	register_cvar("amxbans_custom_statictime","1440", FCVAR_PROTECTED)
	pcvar_show_prebanned 	=	register_cvar("amxbans_show_prebanned","1", FCVAR_PROTECTED)
	pcvar_default_banreason	=	register_cvar("amxbans_default_ban_reason","unknown", FCVAR_PROTECTED)
	pcvar_prefix 		= 	get_cvar_pointer("amx_sql_prefix");

	register_concmd("admin_addban", "cmdAddBan", ADMIN_BAN, "<^"authid^" or IP> <minutes (0 = permanent)> <^"reason^"> <name> [ip]")
	register_srvcmd("admin_addban", "cmdAddBan", -1, "<^"authid^" or IP> <minutes (0 = permanent)> <^"reason^"> <name> [ip]")
	register_srvcmd("amx_ban", "cmdBan", -1, "<^"authid^" or nickname or IP> <minutes (0 = permanent)> <reason>") // used for Add Online ban
/*	register_concmd("admin_ban", "cmdBan", ADMIN_BAN, "<^"authid^" or nickname or IP> <minutes (0 = permanent)> <reason>")
	register_concmd("admin_banip", "cmdBan", ADMIN_BAN, "<^"authid^" or nickname or IP> <minutes (0 = permanent)> <reason>")
	register_srvcmd("admin_banip", "cmdBan", -1, "<^"authid^" or nickname or IP> <minutes (0 = permanent)> <reason>")*/
	register_concmd("admin_unban", "cmdUnban", ADMIN_BAN, "<^"authid^" or IP>");
	register_srvcmd("admin_unban", "cmdUnban", -1, "<^"authid^" or IP>");

	register_srvcmd("amx_list", "cmdLst", ADMIN_RCON, "sends playerinfos to web")

	g_coloredMenus 		= 	colored_menus()
	g_MyMsgSync 		= 	CreateHudSyncObj()

	g_banReasons		=	ArrayCreate(128,7)
	g_banReasons_Bantime 	=	ArrayCreate(1,7)

	g_disconPLname		=	ArrayCreate(32,1)
	g_disconPLauthid	=	ArrayCreate(35,1)
	g_disconPLip		=	ArrayCreate(22,1)

	new configsDir[64]
	get_configsdir(configsDir, charsmax(configsDir))

	server_cmd("exec %s/sql.cfg", configsDir)
	server_cmd("exec %s/amxbans.cfg", configsDir)

	set_task(0.1, "sql_init")
}

create_forwards()
{
	MFHandle[Ban_MotdOpen]=CreateMultiForward("amxbans_ban_motdopen",ET_IGNORE,FP_CELL)
	MFHandle[Player_Flagged]=CreateMultiForward("amxbans_player_flagged",ET_IGNORE,FP_CELL,FP_CELL,FP_STRING)
	MFHandle[Player_UnFlagged]=CreateMultiForward("amxbans_player_unflagged",ET_IGNORE,FP_CELL)
}

public addMenus()
{
	new szKey[64]
/*	format(szKey,charsmax(szKey),"%L",LANG_SERVER,"ADMMENU_FLAGGING")
	AddMenuItem(szKey,"admin_flaggingmenu",ADMIN_BAN,PLUGINNAME)*/
	format(szKey,charsmax(szKey),"%L",LANG_SERVER,"ADMMENU_DISCONNECTED")
	AddMenuItem(szKey,"admin_disconnectedmenu",ADMIN_BAN,PLUGINNAME)
	format(szKey,charsmax(szKey),"%L",LANG_SERVER,"ADMMENU_HISTORY")
	AddMenuItem(szKey,"admin_banhistorymenu",ADMIN_BAN,PLUGINNAME)
}

public sql_init()
{
	new error[128], errno;

	SQL_SetAffinity("mysql")
	g_SqlX = SQL_MakeStdTuple()
	new Handle:temp = SQL_Connect(g_SqlX, errno, error, charsmax(error))

	if(temp == Empty_Handle)
	{
		server_print("[AMXBans] %L", LANG_SERVER, "SQL_CANT_CON", error)
		return
	}

	SQL_FreeHandle(temp);

	get_pcvar_string(pcvar_prefix, g_dbPrefix, charsmax(g_dbPrefix));

	create_forwards()
	set_task(0.1, "banmod_online")
	set_task(0.2, "fetchReasons")
	set_task(2.0, "addMenus")
}

//////////////////////////////////////////////////////////////////
public get_higher_ban_time_admin_flag()
{
	new flags[24]
	get_pcvar_string(pcvar_higher_ban_time_admin, flags, charsmax(flags))
	
	return(read_flags(flags))
}

public get_admin_mole_access_flag()
{
	new flags[24]
	get_pcvar_string(pcvar_admin_mole_access, flags, charsmax(flags))
	
	return(read_flags(flags))
}

public delayed_kick(player_id)
{
	player_id-=200
	new userid = get_user_userid(player_id)
	new kick_message[128]

	format(kick_message, charsmax(kick_message), "%L", player_id,"KICK_MESSAGE")

	if ( get_pcvar_num(pcvar_debug) >= 1 )
		log_amx("[AMXBANS DEBUG] Delayed Kick ID: <%d>", player_id)

	server_cmd("kick #%d  %s",userid, kick_message)

	g_kicked_by_amxbans[player_id] = true
	g_being_banned[player_id] = false

	return PLUGIN_CONTINUE
}

/*********  Error handler  ***************/
MySqlX_ThreadError(szQuery[], error[], errnum, failstate, id)
{
	if (failstate == TQUERY_CONNECT_FAILED)
	{
		log_amx("%L", LANG_SERVER, "TCONNECTION_FAILED")
	}
	else if (failstate == TQUERY_QUERY_FAILED)
	{
		log_amx("%L", LANG_SERVER, "TQUERY_FAILED")
	}
	log_amx("%L", LANG_SERVER, "TQUERY_ERROR", id)
	log_amx("%L", LANG_SERVER, "TQUERY_MSG", error, errnum)
	log_amx("%L", LANG_SERVER, "TQUERY_STATEMENT", szQuery)
}

/*********    client functions     ************/
public client_authorized(id)
{
	//fix for the invalid tuple error at mapchange, only a fast fix now
	if(g_SqlX==Empty_Handle)
	{
		set_task(2.0,"client_authorized",id)
		return PLUGIN_HANDLED
	}
	//check if an activ ban exists
	check_player(id)

	return PLUGIN_CONTINUE
}

public client_putinserver(id)
{
	//fix for the invalid tuple error at mapchange, only a fast fix now
	if(g_SqlX==Empty_Handle)
	{
		set_task(5.0, "client_putinserver", id)
		return PLUGIN_HANDLED
	}

	//set_task(5.0, "NotifyPlayer", id + 5)

	//check if the player was banned before
	prebanned_check(id) // a lot of players will cause an issue with too many threaded queries on map change

	//remove the player from the disconnect player list because he is already connected ;-)
	disconnect_remove_player(id)

	if( is_user_admin(id) )
	{ // last active
		new szAuthID[MAX_AUTHID_LENGTH], szName[MAX_NAME_LENGTH], szQuery[512]
		get_user_authid(id, szAuthID, charsmax(szAuthID))
		get_user_name(id, szName, charsmax(szName))

		formatex(szQuery, charsmax(szQuery), "UPDATE `%s_amxadmins` SET lastactive = NOW() WHERE ((flags LIKE '%%c%%' AND steamid = '%s') OR (flags LIKE '%%k%%' AND username = '%s'))", g_dbPrefix, szAuthID, szName)

		SQL_ThreadQuery(g_SqlX, "LastActiveHandler", szQuery)
	}

	return PLUGIN_CONTINUE
}

public LastActiveHandler(failstate, Handle:query, error[], errnum, data[], size)
{
	if( failstate )
	{
		new szQuery[256]
		MySqlX_ThreadError( szQuery, error, errnum, failstate, 17 )
		return PLUGIN_HANDLED
	}

	SQL_FreeHandle(query)

	return PLUGIN_HANDLED
}

public client_disconnect(id)
{
	g_being_banned[id] = false

	if(task_exists(id))
		remove_task(id)

	if(task_exists(id + 5))
		remove_task(id + 5)

	if(task_exists(id + 123))
		remove_task(id + 123)

	if(task_exists(id + 200))
		remove_task(id + 200)

	if(task_exists(id + 338))
		remove_task(id + 338)

	if(!g_kicked_by_amxbans[id])
	{
		//only add players to disconnect list if not kicked by amxbans
		disconnected_add_player(id)
	}
	else if(g_being_flagged[id])
	{
		// if kicked by amxbans maybe remove the flagged, not added yet
		/*****///remove_flagged_by_steam(0,id,0)
	}
	//reset some vars
	g_kicked_by_amxbans[id] = false
	g_being_flagged[id] = false
	for(new i = 1; i <= get_maxplayers(); i++)
	{
		if( bPlayerSQLCheck[i][id] )
			bPlayerSQLCheck[i][id] = false
	}
}

/*********    timecmd functions     ************/
public setHighBantimes()
{
	new arg[32]
	new argc = read_argc() - 1
	g_highbantimesnum = argc

	if(argc < 1 || argc > 14)
	{
		log_amx("[AMXBANS] You have more than 14 or less than 1 bantimes set in amx_sethighbantimes")
		log_amx("[AMXBANS] Loading default bantimes")
		loadDefaultBantimes(1)

		return PLUGIN_HANDLED
	}

	new i = 0
	new num[32], flag[32]
	while (i < argc)
	{
		read_argv(i + 1, arg, 31)
		parse(arg, num, 31, flag, 31)

		if(equali(flag, "m"))
		{ 
			g_HighBanMenuValues[i] = str_to_num(num)
		}
		else if(equali(flag, "h"))
		{
			g_HighBanMenuValues[i] = (str_to_num(num) * 60)
		}
		else if(equali(flag, "d"))
		{
			g_HighBanMenuValues[i] = (str_to_num(num) * 1440)
		}
		else if(equali(flag, "w"))
		{
			g_HighBanMenuValues[i] = (str_to_num(num) * 10080)
		}
		i++
	}
	return PLUGIN_HANDLED
}

public setLowBantimes()
{
	new arg[32]
	new argc = read_argc() - 1
	g_lowbantimesnum = argc
	
	if(argc < 1 || argc > 14)
	{
		log_amx("[AMXBANS] You have more than 14 or less than 1 bantimes set in amx_setlowbantimes")
		log_amx("[AMXBANS] Loading default bantimes")
		loadDefaultBantimes(2)
		
		return PLUGIN_HANDLED
	}

	new i = 0
	new num[32], flag[32]
	while (i < argc)
	{
		read_argv(i + 1, arg, 31)
		parse(arg, num, 31, flag, 31)

		if(equali(flag, "m"))
		{ 
			g_LowBanMenuValues[i] = str_to_num(num)
		}
		else if(equali(flag, "h"))
		{
			g_LowBanMenuValues[i] = (str_to_num(num) * 60)
		}
		else if(equali(flag, "d"))
		{
			g_LowBanMenuValues[i] = (str_to_num(num) * 1440)
		}
		else if(equali(flag, "w"))
		{
			g_LowBanMenuValues[i] = (str_to_num(num) * 10080)
		}
		i++
	}
	return PLUGIN_HANDLED
}

public setFlagTimes()
{
	new arg[32]
	new argc = read_argc() - 1
	g_flagtimesnum = argc
	if(argc < 1 || argc > 14)
	{
		log_amx("[AMXBANS] You have more than 14 or less than 1 flagtimes set in amx_setflagtimes")
		log_amx("[AMXBANS] Loading default flagtimes")
		loadDefaultBantimes(3)
		
		return PLUGIN_HANDLED
	}
	
	new i = 0
	new num[32], flag[32]
	while (i < argc)
	{
		read_argv(i + 1, arg, 31)
		parse(arg, num, 31, flag, 31)

		if(equali(flag, "m"))
		{ 
			g_FlagMenuValues[i] = str_to_num(num)
		}
		else if(equali(flag, "h"))
		{
			g_FlagMenuValues[i] = (str_to_num(num) * 60)
		}
		else if(equali(flag, "d"))
		{
			g_FlagMenuValues[i] = (str_to_num(num) * 1440)
		}
		else if(equali(flag, "w"))
		{
			g_FlagMenuValues[i] = (str_to_num(num) * 10080)
		}
		i++
	}
	return PLUGIN_HANDLED
}

loadDefaultBantimes(num)
{
	if(num == 1 || num == 0)
		server_cmd("amx_sethighbantimes 5 60 240 600 6000 0")
	if(num == 2 || num == 0)
		server_cmd("amx_setlowbantimes 5 30 60 480 600 1440")
	if(num == 3 || num == 0)
		server_cmd("amx_setflagtimes 60 240 600 1440 10080 40320 90720 0")
}

/*********    mysql escape functions     ************/
mysql_escape_string(const source[],dest[],len)
{
	copy(dest, len, source);
	replace_all(dest,len,"\\","\\\\");
	replace_all(dest,len,"\0","\\0");
	replace_all(dest,len,"\n","\\n");
	replace_all(dest,len,"\r","\\r");
	replace_all(dest,len,"\x1a","\Z");
	replace_all(dest,len,"'","\'");
	replace_all(dest,len,"^"","\^"");
}

mysql_get_username_safe(id,dest[],len)
{
	new name[128]
	get_user_name(id,name,charsmax(name))
	mysql_escape_string(name,dest,len)
}

mysql_get_servername_safe(dest[],len)
{
	new server_name[256]
	get_cvar_string("hostname", server_name, charsmax(server_name))
	mysql_escape_string(server_name,dest,len)
}

public plugin_end()
{
	ArrayDestroy(g_banReasons)
	ArrayDestroy(g_banReasons_Bantime)
	g_SqlX = Empty_Handle;
}