 Customized AMXBans
 
 Requirements:
 (Based on) AMXBans 6.13 (http://www.amxbans.net/dl.php?f=6.13)
 
 Incomplete list of changes:

 Web:
+ New 'Last Active'-column with a date of all admins last active status (not publicly visible to everybody) in admin_list.php
+ List all (visible) admins steam signature banner in admin_list.php

 Plugin:
+ Changed commands prefix from 'amx_' to 'admin_'
+ Reduced the amount of console commands to integrate it into a simple menu.
+ Ban by Steam ID and IP by default.
+ Notify all admins on the game server whenever a player with previous ban(s) enters the game server.
+ Notify the connecting admin upon entering the game server about all players' with previous ban(s).
+ List only all players' with a ban history in the ban history menu.
+ Admins can add/remove ban(s) into/from the database from the console directly.
	+ Notifies all other admins on the server if a admin unbans.
	+ Doesn't check if the admin have an account on the website to check their user level to only allow them to remove their own bans, yet.

 Client console commands:
+ admin_addban <^"authid"^ or IP> <minutes (0 = permanent)> <^"reason"^> <name> [ip]
+ admin_banmenu - displays ban menu
+ admin_banhistorymenu - displays ban history of players menu
+ admin_disconnectedmenu - displays disconnected players menu

<> - argument is required

^"^" - argument has to be embedded within quote marks

[] - optional argument

NOTE! Only Tested on Sven Co-op 5+

Might not but should work with other GoldSrc games & mods as well.
