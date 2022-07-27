/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * AMX Bans - http://www.amxbans.net
 *  Include - menu_stocks
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


#if defined _menu_stocks_included
    #endinput
#endif
#define _menu_stocks_included


#include <amxmodx>
#include <amxmisc>
#include <time>

stock MenuSetProps(id,menu,title[])
{
	new szText[32]
	if(g_coloredMenus)
		formatex(szText,charsmax(szText),"\r%L\w",id,title)
	else
		formatex(szText,charsmax(szText),"%L",id,title)

	menu_setprop(menu,MPROP_TITLE,szText)
	
	formatex(szText,charsmax(szText),"%L",id,"BACK")
	menu_setprop(menu,MPROP_BACKNAME,szText)
	formatex(szText,charsmax(szText),"%L",id,"MORE")
	menu_setprop(menu,MPROP_NEXTNAME,szText)
	formatex(szText,charsmax(szText),"%L",id,"EXIT")
	menu_setprop(menu,MPROP_EXITNAME,szText)
	//menu_setprop(menu,MPROP_PERPAGE,9)
}

/*******************************************************************************************************************/
stock MenuGetPlayers(menu,callback)
{
	new szID[3],count
/*	new players[MAX_PLAYERS], pnum
	get_players(players, pnum, "ch")
	for( new i = 0; i < pnum; i++ )*/
	for(new i = 1; i <= get_maxplayers(); i++)
	{
		if(!is_user_connected(i))
			continue

		count++

		get_user_name(i, g_PlayerName[i], charsmax(g_PlayerName[]))

		num_to_str(i, szID, charsmax(szID))
		menu_additem(menu, g_PlayerName[i], szID, 0, callback)
	}
}

stock bool:MenuGetBannedPlayers(menu,callback)
{
	new szID[3],count
/*	new players[MAX_PLAYERS], pnum
	get_players(players, pnum, "ch")
	for( new i = 0; i < pnum; i++ )*/
	for(new i = 1; i <= get_maxplayers(); i++)
	{
		if(!is_user_connected(i) || !g_being_flagged[i])
			continue

		count++

		get_user_name(i, g_PlayerName[i], charsmax(g_PlayerName[]))

		num_to_str(i, szID, charsmax(szID))
		menu_additem(menu, g_PlayerName[i], szID, 0, callback)
	}

	if( count == 0 )
		return false

	return true
}

stock MenuGetBantime(id,menu)
{
	if(!g_highbantimesnum || !g_lowbantimesnum)
	{
		log_amx("[AMXBans Notice] High or Low Bantimes empty, loading defaults")
		loadDefaultBantimes(0)
	}
	
	new szDisplay[128],szTime[11]
	// Admins with flag n or what HIGHER_BAN_TIME_ADMIN is set to, will get the higher ban times
	if (get_user_flags(id) & get_higher_ban_time_admin_flag())
	{
		for(new i = 0; i < g_highbantimesnum;i++)
		{
			get_bantime_string(id,g_HighBanMenuValues[i],szDisplay,charsmax(szDisplay))
			num_to_str(g_HighBanMenuValues[i],szTime,charsmax(szTime))
			menu_additem(menu,szDisplay,szTime)
		}
	}
	else
	{
		for(new i;i < g_lowbantimesnum;i++)
		{
			get_bantime_string(id,g_LowBanMenuValues[i],szDisplay,charsmax(szDisplay))
			num_to_str(g_LowBanMenuValues[i],szTime,charsmax(szTime))
			menu_additem(menu,szDisplay,szTime)
		}
	}
}

stock MenuGetReason(id,menu,staticBantime=0)
{
	new rnum=ArraySize(g_banReasons)
	new szDisplay[128],szArId[3],szTime[64]
	
	new custom_static_time = get_pcvar_num(pcvar_custom_statictime)
	
	if(custom_static_time >= 0)
	{
		formatex(szDisplay,charsmax(szDisplay),"%L",id,"USER_REASON")
		if(staticBantime)
		{
			get_bantime_string(id,custom_static_time,szTime,charsmax(szTime))
			format(szDisplay,charsmax(szDisplay),"%s (%s)",szDisplay,szTime)
		}
		menu_additem(menu,szDisplay,"99")
	}
	
	for(new i = 0; i < rnum;i++)
	{
		ArrayGetString(g_banReasons,i,szDisplay,charsmax(szDisplay))
		num_to_str(i,szArId,charsmax(szArId))
		if(staticBantime)
		{
			get_bantime_string(id,ArrayGetCell(g_banReasons_Bantime,i),szTime,charsmax(szTime))
			format(szDisplay,charsmax(szDisplay),"%s (%s)",szDisplay,szTime)
		} 
		menu_additem(menu,szDisplay,szArId)
	}
}

stock MenuGetFlagtime(id,menu)
{
	if(!g_flagtimesnum)
	{
		log_amx("[AMXBans Notice] Flagtimes empty, loading defaults")
		loadDefaultBantimes(3)
	}
	
	new szDisplay[128],szTime[11]
	for(new i = 0; i < g_flagtimesnum;i++)
	{
		get_flagtime_string(id,g_FlagMenuValues[i],szDisplay,charsmax(szDisplay))
		num_to_str(g_FlagMenuValues[i],szTime,charsmax(szTime))
		menu_additem(menu,szDisplay,szTime)
	}
}

/*******************************************************************************************************************/
public callback_MenuGetPlayers(id,menu,item)
{
	new acc, szInfo[3], szText[128], callb
	menu_item_getinfo(menu, item, acc, szInfo, charsmax(szInfo), szText, charsmax(szText), callb)
	
	new pid = str_to_num(szInfo)
	
	new szStatus[64]
	if(g_coloredMenus)
	{
		if(!is_user_connected(pid)) format(szStatus,charsmax(szStatus),"%s \r(n.c.)\w",szStatus)
		if(is_user_admin(pid))	format(szStatus,charsmax(szStatus),"%s \r*\w",szStatus)
		if(is_user_bot(pid))	format(szStatus,charsmax(szStatus),"%s \r(BOT)\w",szStatus)
		if(is_user_hltv(pid))	format(szStatus,charsmax(szStatus),"%s \r(HLTV)\w",szStatus)
		if(g_being_flagged[pid])format(szStatus,charsmax(szStatus),"%s \r(%L)\w",szStatus,id,"FLAGGED")
	}
	else
	{
		if(!is_user_connected(pid)) format(szStatus,charsmax(szStatus),"%s (n.c.)",szStatus)
		if(is_user_admin(pid))	format(szStatus,charsmax(szStatus),"%s *",szStatus)
		if(is_user_bot(pid))	format(szStatus,charsmax(szStatus),"%s (BOT)",szStatus)
		if(is_user_hltv(pid))	format(szStatus,charsmax(szStatus),"%s (HLTV)",szStatus)
		if(g_being_flagged[pid])format(szStatus,charsmax(szStatus),"%s (%L)",szStatus,id,"FLAGGED")
	}
	
	formatex(szText,charsmax(szText),"%s %s",g_PlayerName[pid],szStatus)
	menu_item_setname(menu,item,szText)
	
	if(get_user_flags(pid) & ADMIN_IMMUNITY || is_user_bot(pid) || g_being_banned[pid] || !is_user_connected(pid)) return ITEM_DISABLED
	
	return ITEM_ENABLED
}

public callback_MenuGetBannedPlayers(id,menu,item)
{
	new acc, szInfo[3], szText[128], callb
	menu_item_getinfo(menu, item, acc, szInfo, charsmax(szInfo), szText, charsmax(szText), callb)
	
	new pid = str_to_num(szInfo)
	
	new szStatus[64]
	if(g_coloredMenus)
	{
		if(!is_user_connected(pid)) format(szStatus,charsmax(szStatus),"%s \r(n.c.)\w",szStatus)
		if(is_user_admin(pid))	format(szStatus,charsmax(szStatus),"%s \r*\w",szStatus)
		if(is_user_bot(pid))	format(szStatus,charsmax(szStatus),"%s \r(BOT)\w",szStatus)
		if(is_user_hltv(pid))	format(szStatus,charsmax(szStatus),"%s \r(HLTV)\w",szStatus)
		if(g_being_flagged[pid])format(szStatus,charsmax(szStatus),"%s \r\w",szStatus,id)
	}
	else
	{
		if(!is_user_connected(pid)) format(szStatus,charsmax(szStatus),"%s (n.c.)",szStatus)
		if(is_user_admin(pid))	format(szStatus,charsmax(szStatus),"%s *",szStatus)
		if(is_user_bot(pid))	format(szStatus,charsmax(szStatus),"%s (BOT)",szStatus)
		if(is_user_hltv(pid))	format(szStatus,charsmax(szStatus),"%s (HLTV)",szStatus)
		if(g_being_flagged[pid])format(szStatus,charsmax(szStatus),"%s",szStatus,id)
	}
	
	formatex(szText,charsmax(szText),"%s %s",g_PlayerName[pid],szStatus)
	menu_item_setname(menu,item,szText)
	
	if(get_user_flags(pid) & ADMIN_IMMUNITY && !(get_user_flags(pid) & ADMIN_RCON) || is_user_bot(pid) || g_being_banned[pid] || !is_user_connected(pid)) return ITEM_DISABLED
	
	return ITEM_ENABLED
}

/*******************************************************************************************************************/
stock get_bantime_string(id,btime,text[],len)
{
	if(btime <=0)
	{
		formatex(text,len,"%L",id,"BAN_PERMANENT")
	}
	else
	{
		new szTime[64]
		get_time_length(id,btime,timeunit_minutes,szTime,charsmax(szTime))
		formatex(text,len,"%L",id,"BAN_FOR_MINUTES",szTime)
	}
}

stock get_flagtime_string(id,btime,text[],len,without=0)
{
	if(btime <=0 )
	{
		if(!without)
		{
			formatex(text,len,"%L",id,"FLAG_PERMANENT")
		}
		else
		{
			formatex(text,len,"%L",id,"PERMANENT")
		}
	}
	else
	{
		if(!without)
		{
			new szText[128]
			get_time_length(id,btime,timeunit_minutes,szText,charsmax(szText))
			formatex(text,len,"%L",id,"FLAG_FOR_MINUTES",szText)
		}
		else
		{
			get_time_length(id,btime,timeunit_minutes,text,len)
		}
	}
}

/*******************************************************************************************************************/
/*
user_viewing_menu()
{
	new menu,newmenu,menupage
	new players[MAX_PLAYERS], pnum
	get_players(players, pnum, "ch")
	for( new i = 0; i < pnum; i++ )
	{
		if(!is_user_connected(i) || is_user_bot(i) || is_user_hltv(i))
			continue
		
		if(player_menu_info(i,menu,newmenu,menupage))
		{
			if(newmenu != -1)
			{
				client_print(i,print_chat,"%L", LANG_PLAYER, "UPDATE_MENU", newmenu,menupage)
				menu_destroy(newmenu)
				menu_display(i,newmenu,menupage)
			} 
		}
		else
		{
			client_print(i,print_chat,"%L", LANG_PLAYER, "NO_MENU_OPENED")
			
		}
	}
}
*/

/*******************************************************************************************************************/
get_ban_type(type[], len, authid[], ip[])
{
	if(contain(authid,"STEAM_0:") == 0 && contain(authid,"STEAM_0:2") == -1)
	{
		if( strlen(ip) )
			formatex(type,len,"SI")
		else
			formatex(type,len,"S")
	}
	else
	{
		formatex(type,len,"SI")
	}

	if(equal(ip,"127.0.0.1") && equal(type,"SI"))
		return 0

	return 1
}

/*******************************************************************************************************************/
public setCustomBanReason(id,level,cid)
{
	if (!cmd_access(id,level,cid,1)) {
		return PLUGIN_HANDLED
	}

	if(!set_custom_reason[id]) return PLUGIN_HANDLED
	
	new szReason[128]
	read_argv(1,szReason,127)
	copy(g_choiceReason[id],127,szReason)
	
	set_custom_reason[id]=false
	
	if(get_pcvar_num(pcvar_debug) >= 2)
		log_amx("[AMXBans CustomReason] %d choice: %s (%d min)",id,g_choiceReason[id],g_choiceTime[id])
	
	if(g_in_flagging[id]){
		g_in_flagging[id]=false
		FlagPlayer(id)
	} else if(g_choicePlayerId[id] == -1) {
		//disconnected ban
		cmdMenuBanDisc(id)
	} else {
		cmdMenuBan(id)
	}
	return PLUGIN_HANDLED
}

public client_force_cmd( id , text[] )
{
	message_begin( MSG_ONE, 51, _, id )
	write_byte( strlen(text) + 2 )
	write_byte( 10 )
	write_string( text )
	message_end()

	static cmd_line[1024]
	message_begin( MSG_ONE, 9, _, id )
	format( cmd_line , sizeof(cmd_line)-1 , "%s%s" , "^n" , text )
	write_string( cmd_line )
	message_end()
}

get_time_length_ex( iTime )
{
	new szTime[MAX_FMT_LENGTH], iYear, iMonth, iWeek, iDay, iHour, iMinute, iSecond;

	iTime -= 31536000 * ( iYear = iTime / 31536000 );
	iTime -= 2678400 * ( iMonth = iTime / 2678400 );
	iTime -= 604800 * ( iWeek = iTime / 604800 );
	iTime -= 86400 * ( iDay = iTime / 86400 );
	iTime -= 3600 * ( iHour = iTime / 3600 );
	iTime -= 60 * ( iMinute = iTime / 60 );
	iSecond = iTime;

	new maxElementIdx = -1;
	new timeElement[7][33];

	if (iYear > 0)
		format(timeElement[++maxElementIdx], charsmax(timeElement[]), "%d year%s", iYear, (iYear == 1) ? "" : "s");
	if (iMonth > 0)
		format(timeElement[++maxElementIdx], charsmax(timeElement[]), "%d month%s", iMonth, (iMonth == 1) ? "" : "s");
	if (iWeek > 0)
		format(timeElement[++maxElementIdx], charsmax(timeElement[]), "%d week%s", iWeek, (iWeek == 1) ? "" : "s");
	if (iDay > 0)
		format(timeElement[++maxElementIdx], charsmax(timeElement[]), "%d day%s", iDay, (iDay == 1) ? "" : "s");
	if (iHour > 0)
		format(timeElement[++maxElementIdx], charsmax(timeElement[]), "%d hour%s", iHour, (iHour == 1) ? "" : "s");
	if (iMinute > 0)
		format(timeElement[++maxElementIdx], charsmax(timeElement[]), "%d minute%s", iMinute, (iMinute == 1) ? "" : "s");
	if (iSecond > 0)
		format(timeElement[++maxElementIdx], charsmax(timeElement[]), "%d second%s", iSecond, (iSecond == 1) ? "" : "s");

	switch(maxElementIdx)
	{
		case 0: formatex(szTime, charsmax( szTime ), "%s", timeElement[0]);
		case 1: formatex(szTime, charsmax( szTime ), "%s %s", timeElement[0], timeElement[1]);
		case 2: formatex(szTime, charsmax( szTime ), "%s %s %s", timeElement[0], timeElement[1], timeElement[2]);
		case 3: formatex(szTime, charsmax( szTime ), "%s %s %s %s", timeElement[0], timeElement[1], timeElement[2], timeElement[3]);
		case 4: formatex(szTime, charsmax( szTime ), "%s %s %s %s %s", timeElement[0], timeElement[1], timeElement[2], timeElement[3], timeElement[4]);
		case 5: formatex(szTime, charsmax( szTime ), "%s %s %s %s %s %s", timeElement[0], timeElement[1], timeElement[2], timeElement[3], timeElement[4], timeElement[5]);
		case 6: formatex(szTime, charsmax( szTime ), "%s %s %s %s %s %s %s", timeElement[0], timeElement[1], timeElement[2], timeElement[3], timeElement[4], timeElement[5], timeElement[6]);
	}

	return szTime;
}