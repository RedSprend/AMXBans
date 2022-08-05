 Customized AMXBans
 <br><br>
 Requirements:<br>
 (Based on) AMXBans 6.14.4 (http://www.amxbans.net/dl.php?f=6.13)<br>
 This version have modified AMXBans tables, so you have to run sql.sql in your SQL server<br>
 <br>
 Incomplete list of changes:
<br>
 Web:
+ New 'Last Active'-column with a date of all admins last active status (not publicly visible to everybody) in admin_list.php
+ List all visible admins in admin_list.php steam signature banner (credit to sami1)
+ Web admins have the option to hide server(s) from the public (only logged in users will be able to see them)

 Plugin:
+ Changed commands prefix from 'amx_' to 'admin_'
+ Reduced the amount of console commands to integrate it into a simple menu.
+ Ban by Steam ID and IP by default.
+ Notify all admins on the game server whenever a player with previous ban(s) enters the game server.
+ Notify the connecting admin upon entering the game server about all online players' with previous ban(s).
+ List only all players' with a ban history in the ban history menu.
+ Admins can add/remove ban(s) into/from the database from the console directly.
	+ Notifies all other admins on the server if a admin add ban/unban.
	+ (!) Doesn't check if the admin have an account in the database to check their user level if they are only allowed to remove their own bans, yet.

 Admin console commands:
+ admin_addban <"authid" or IP> <minutes (0 = permanent)> <"reason"> <name> [ip]
+ admin_banmenu - displays ban menu
+ admin_banhistorymenu - displays ban history of players menu
+ admin_disconnectedmenu - displays disconnected players menu

<> - argument is required<br>
"" - argument has to be embedded within quote marks<br>
[] - optional argument<br>
<br>
NOTE! Only Tested on Sven Co-op 5+<br>
Might not but should work with other GoldSrc games & mods as well.
