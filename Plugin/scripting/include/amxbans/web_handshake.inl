/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 * AMX Bans - http://www.amxbans.net
 *  Include - web_handshake
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


#if defined _web_handshake_included
    #endinput
#endif
#define _web_handshake_included

#include <amxmodx>

public cmdLst(id,level,cid)
{
	if(id) return PLUGIN_HANDLED
	
	new name[MAX_NAME_LENGTH],authid[MAX_AUTHID_LENGTH],ip[MAX_IP_LENGTH],status,immun,userid

	//console_print(id,"%c%c%c%c",-1,-1,-1,-1)
/*	new players[MAX_PLAYERS], pnum
	get_players(players, pnum, "ch")
	for( new i = 0; i < pnum; i++ )*/
	for(new i = 1; i <= get_maxplayers(); i++)
	{
		if(is_user_connected(i)) {
			get_user_name(i,name,charsmax(name))
			get_user_ip(i,ip,charsmax(ip),1)
			get_user_authid(i,authid,charsmax(authid))
			userid=get_user_userid(i)
			
			status=0
			if(is_user_bot(i)) status=1
			if(is_user_hltv(i)) status=2
			
			immun=0
			if(get_user_flags(i) & ADMIN_IMMUNITY) immun=1
			
			console_print(id,"%s%c%d%c%s%c%s%c%d%c%d",name,-4,userid,-4,authid,-4,ip,-4,status,-4,immun)
		}
	}
	return PLUGIN_HANDLED
}
