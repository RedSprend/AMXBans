/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * AMX Bans - http://www.amxbans.net
 *  Include - check_player
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


#if defined _check_player_included
    #endinput
#endif
#define _check_player_included

#include <amxmodx>
#include <amxmisc>
#include <sqlx>

public prebanned_check(id)
{
	if(!get_pcvar_num(pcvar_show_prebanned) || is_user_bot(id))
		return PLUGIN_HANDLED

	if( get_user_flags(id) & ADMIN_KICK )
	{
		for(new i = 1; i <= get_maxplayers(); i++)
		{
			if( is_user_bot(id) || is_user_hltv(id) )
				continue

			bPlayerSQLCheck[id][i] = false
		}

		set_task( 3.0, "NotifyAdmin", id + 338 )
		return PLUGIN_HANDLED
	}

	if( g_SqlX )
	{
		new szAuthID[MAX_AUTHID_LENGTH], szIP[MAX_IP_LENGTH], pquery[512]
		get_user_authid(id, szAuthID, charsmax(szAuthID))
		get_user_ip(id, szIP, charsmax(szIP), 1)

		formatex(pquery, charsmax(pquery), "SELECT COUNT(*) FROM `%s%s` WHERE (player_id='%s' AND ban_type='S' OR player_id='%s' AND ban_type='SI' OR player_ip='%s' AND ban_type='SI') AND expired=1", g_dbPrefix, tbl_bans, szAuthID, szAuthID, szIP)
		//formatex(pquery, charsmax(pquery), "SELECT COUNT(*) FROM `%s%s` WHERE player_id='%s' AND expired=1",g_dbPrefix, tbl_bans, szAuthID)

		new data[1]
		data[0] = id

		SQL_ThreadQuery(g_SqlX, "prebanned_check_", pquery, data, sizeof(data))
	}

	return PLUGIN_HANDLED
}

public prebanned_check_(failstate, Handle:query, error[], errnum, data[], size)
{
	if( failstate )
	{
		new szQuery[256]
		MySqlX_ThreadError( szQuery, error, errnum, failstate, 16 )
		return PLUGIN_HANDLED
	}

	new id = data[0]
	new ban_count = SQL_ReadResult(query, 0)
	SQL_FreeHandle(query)

	if( ban_count <= 0 || !is_user_connected(id) )
		return PLUGIN_HANDLED

	new name[MAX_NAME_LENGTH], authid[MAX_AUTHID_LENGTH]
	get_user_authid(id, authid, charsmax(authid))
	get_user_name(id, name, charsmax(name))

	g_being_flagged[id] = true

/*	new players[MAX_PLAYERS], pnum
	get_players(players, pnum, "ch")
	for( new i = 0; i < pnum; i++ )*/
	for(new i = 1; i <= get_maxplayers(); i++)
	{
		if( /*i == id ||*/ !is_user_connected(i) || is_user_bot(id) || is_user_hltv(id) || bPlayerSQLCheck[id][i] )
			continue

		if( get_user_flags(i) & ADMIN_CHAT )
		{
			client_print(i, print_chat, "%L", i, "PLAYER_BANNED_BEFORE", name, authid, ban_count, (ban_count > 1) ? "s" : "" )
			bPlayerSQLCheck[i][id] = true
		}
	}

	return PLUGIN_HANDLED
}

public prebanned_check_single( id, iPlayer )
{
	new authid[MAX_AUTHID_LENGTH], ip[MAX_IP_LENGTH], pquery[512]
	get_user_authid(iPlayer, authid, charsmax(authid))
	get_user_ip(iPlayer, ip, charsmax(ip), 1)

	formatex(pquery, charsmax(pquery), "SELECT COUNT(*) FROM `%s%s` WHERE (player_id='%s' AND ban_type='S' OR player_id='%s' AND ban_type='SI' OR player_ip='%s' AND ban_type='SI') AND expired=1", g_dbPrefix, tbl_bans, authid, authid, ip)
	//formatex(pquery, charsmax(pquery), "SELECT COUNT(*) FROM `%s%s` WHERE player_id='%s' AND expired=1", g_dbPrefix, tbl_bans, authid)

	new data[2]
	data[0] = id
	data[1] = iPlayer

	if( g_SqlX )
	{
		SQL_ThreadQuery(g_SqlX, "prebanned_check_single_", pquery, data, sizeof(data))
	}

	return PLUGIN_HANDLED
}

public prebanned_check_single_(failstate, Handle:query, error[], errnum, data[], size)
{
	if( failstate )
	{
		new szQuery[256]
		MySqlX_ThreadError( szQuery, error, errnum, failstate, 16 )
		return PLUGIN_HANDLED
	}

	new id = data[0], iPlayer = data[1]
	new ban_count = SQL_ReadResult(query, 0)
	SQL_FreeHandle(query)

	if( ban_count <= 0 || !is_user_connected(id) || !is_user_connected(iPlayer) || bPlayerSQLCheck[id][iPlayer] )
		return PLUGIN_HANDLED

	new name[MAX_NAME_LENGTH], authid[MAX_AUTHID_LENGTH]
	get_user_authid(iPlayer, authid, charsmax(authid))
	get_user_name(iPlayer, name, charsmax(name))

	bPlayerSQLCheck[id][iPlayer] = true
	g_being_flagged[iPlayer] = true

	client_print(id, print_chat, "%L", iPlayer, "PLAYER_BANNED_BEFORE", name, authid, ban_count, (ban_count > 1) ? "s" : "")

	return PLUGIN_HANDLED
}

public NotifyAdmin( id )
{
	id -= 338;

/*	new players[MAX_PLAYERS], pnum
	get_players(players, pnum, "ch")
	for( new i = 0; i < pnum; i++ )*/
	for(new i = 1; i <= get_maxplayers(); i++)
	{
		if( is_user_bot(id) || is_user_hltv(id) || /*i == id ||*/ !is_user_connected(i) || bPlayerSQLCheck[id][i] )
			continue

		prebanned_check_single(id, i)
	}
}

/*************************************************************************/

public check_player(id)
{
	new authid[MAX_AUTHID_LENGTH], ip[MAX_IP_LENGTH]
	get_user_authid(id, authid, charsmax(authid))
	get_user_ip(id, ip, charsmax(ip), 1)

	new data[1], pquery[1024]
	formatex(pquery, charsmax(pquery), "SELECT bid,ban_created,ban_length,ban_reason,admin_nick,admin_id,admin_ip,player_nick,player_id,player_ip,server_name,server_ip,ban_type FROM `%s%s` WHERE (player_id='%s' OR player_ip='%s' AND NOT player_ip='0.0.0.0') AND expired=0",g_dbPrefix, tbl_bans, authid, ip)

	data[0] = id

	SQL_ThreadQuery(g_SqlX, "check_player_", pquery, data, sizeof(data))

	return PLUGIN_HANDLED
}

public check_player_(failstate, Handle:query, error[], errnum, data[], size)
{
	if (failstate)
	{
		new szQuery[256]
		MySqlX_ThreadError( szQuery, error, errnum, failstate, 17 )
		return PLUGIN_HANDLED
	}

	new id = data[0]

	if(!SQL_NumResults(query))
	{
		SQL_FreeHandle(query)
		check_flagged(id)
		return PLUGIN_HANDLED
	}

	new ban_reason[128], admin_nick[MAX_NAME_LENGTH], admin_authid[MAX_AUTHID_LENGTH], admin_ip[MAX_IP_LENGTH], ban_type[4], player_nick[MAX_NAME_LENGTH], player_authid[MAX_AUTHID_LENGTH], player_ip[MAX_IP_LENGTH], server_name[100], server_ip[MAX_IP_LENGTH]

	new bid = SQL_ReadResult(query, 0)
	new ban_created = SQL_ReadResult(query, 1)
	new ban_length_int = SQL_ReadResult(query, 2) * 60 //sec to min
	SQL_ReadResult(query, 3, ban_reason, charsmax(ban_reason))
	SQL_ReadResult(query, 4, admin_nick, charsmax(admin_nick))
	SQL_ReadResult(query, 5, admin_authid, charsmax(admin_authid))
	SQL_ReadResult(query, 6, admin_ip, charsmax(admin_ip))
	SQL_ReadResult(query, 7, player_nick, charsmax(player_nick))
	SQL_ReadResult(query, 8, player_authid, charsmax(player_authid))
	SQL_ReadResult(query, 9, player_ip, charsmax(player_ip))
	SQL_ReadResult(query, 10, server_name, charsmax(server_name))
	SQL_ReadResult(query, 11, server_ip, charsmax(server_ip))
	SQL_ReadResult(query, 12, ban_type, charsmax(ban_type))

	SQL_FreeHandle(query)

	if ( get_pcvar_num(pcvar_debug) >= 1 )
		log_amx("[AMXBans] Player Check on Connect:^nbid: %d ^nwhen: %d ^nlenght: %d ^nreason: %s ^nadmin: %s ^nadminsteamID: %s ^nPlayername %s ^nserver: %s ^nserverip: %s ^nbantype: %s",\
		bid, ban_created, ban_length_int, ban_reason, admin_nick, admin_authid, player_nick, server_name, server_ip, ban_type)

	new current_time_int = get_systime(0)

	if ((ban_length_int == 0) || (ban_created == 0) || ((ban_created+ban_length_int) > current_time_int))// || equal(ban_type, "SI") && equal(player_nick, player_nick)) // nickname is also banned if IP is banned
	{
		// A ban was found for the connecting player!! Lets see how long it is or if it has expired

		new complain_url[256]
		get_pcvar_string(pcvar_complainurl, complain_url, charsmax(complain_url))

		client_cmd(id, "echo =================================")

		new show_activity = get_cvar_num("amx_show_activity")

		if(get_user_flags(id)&get_admin_mole_access_flag() || id == 0)
			show_activity = 1

		switch(show_activity)
		{
			case 1:
			{
				if(ban_length_int > 0)
					client_cmd(id, "echo %L", id, "MSG_9")
			}
			case 2: client_cmd(id, "echo %L", id, "MSG_8", admin_nick)
			case 3:
			{
				if (is_user_admin(id))
					client_cmd(id, "echo %L", id, "MSG_8", admin_nick)
				else
					client_cmd(id, "echo %L", id, "MSG_9")
			}
			case 4:
			{
				if (is_user_admin(id))
					client_cmd(id, "echo %L", id, "MSG_8", admin_nick)
			}
			case 5:
			{
				if (is_user_admin(id))
					client_cmd(id, "echo %L", id, "MSG_9")
			}
		}

		if(ban_length_int == 0)
		{
			client_cmd(id, "echo %L", id, "MSG_10")
		}
		else
		{
			new cTimeLength[128]
			new iSecondsLeft = (ban_created + ban_length_int - current_time_int)
			get_time_length(id, iSecondsLeft, timeunit_seconds, cTimeLength, charsmax(cTimeLength))
			client_cmd(id, "echo %L", id, "MSG_12", cTimeLength)
		}

		client_cmd(id, "echo %L", id, "MSG_2", ban_reason)

		replace_all(complain_url, charsmax(complain_url), "https://","")
		client_cmd(id, "echo %L", id, "MSG_7", complain_url)
		client_cmd(id, "echo =================================")

		if ( get_pcvar_num(pcvar_debug) >= 1 )
			log_amx("[AMXBans] BID:<%d> Player:<%s> <%s> connected and got kicked, because of an active ban", bid, player_nick, player_authid)

		new id_str[3]
		num_to_str(id, id_str, charsmax(id_str))

		if ( get_pcvar_num(pcvar_debug) >= 1 )
			log_amx("[AMXBans] Delayed Kick-TASK ID1: <%d>  ID2: <%s>", id, id_str)

		add_kick_to_db(bid)

		set_task_ex(0.8, "delayed_kick", id + 200)

		return PLUGIN_HANDLED
	}

	// The ban has expired
	//client_cmd(id, "echo %L",LANG_PLAYER,"MSG_11")

	new pquery[1024]
	formatex(pquery, charsmax(pquery), "UPDATE `%s%s` SET expired=1 WHERE bid=%d", g_dbPrefix, tbl_bans, bid)

	if(g_SqlX)
		SQL_ThreadQuery(g_SqlX, "insert_to_banhistory", pquery)

	if ( get_pcvar_num(pcvar_debug) >= 1 )
		log_amx("[AMXBans] PRUNE BAN: %s",pquery)

	check_flagged(id)

	return PLUGIN_HANDLED
}

public insert_to_banhistory(failstate, Handle:query, error[], errnum, data[], size)
{
	if (failstate)
	{
		new szQuery[256]
		MySqlX_ThreadError( szQuery, error, errnum, failstate, 19 )
	}

	SQL_FreeHandle(query)

	return PLUGIN_HANDLED
}

/*************************************************************************/
public add_kick_to_db(bid)
{
	new pquery[1024]
	formatex(pquery,charsmax(pquery),"UPDATE `%s%s` SET `ban_kicks`=`ban_kicks`+1 WHERE `bid`=%d",g_dbPrefix, tbl_bans, bid)

	if(g_SqlX)
		SQL_ThreadQuery(g_SqlX, "_add_kick_to_db", pquery)

	return PLUGIN_HANDLED
}

public _add_kick_to_db(failstate, Handle:query, error[], errnum, data[], size)
{
	if (failstate)
	{
		new szQuery[256]
		MySqlX_ThreadError( szQuery, error, errnum, failstate, 19 )
	}

	SQL_FreeHandle(query)

	return PLUGIN_HANDLED
}