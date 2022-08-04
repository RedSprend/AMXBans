<?php

/* 	

	AMXBans v6.0
	
	Copyright 2009, 2010 by SeToY & |PJ|ShOrTy

	This file is part of AMXBans.

    AMXBans is free software, but it's licensed under the
	Creative Commons - Attribution-NonCommercial-ShareAlike 2.0

    AMXBans is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    You should have received a copy of the cc-nC-SA along with AMXBans.  
	If not, see <http://creativecommons.org/licenses/by-nc-sa/2.0/>.

*/

session_start();
if(!$_SESSION["loggedin"]) {
	echo "No Access!";
	exit;
}

require_once("config.inc.php");

$id = $_GET["id"];

if(file_exists("../templates/".$config->design."/amxbans.css")) {
	$design="/".$config->design;
}

?>

<html>
<head>
<title>AMXBans <?php echo $config->v_web; ?> - Access Generator</title>
<link rel="stylesheet" type="text/css" href="../templates<?php echo $design; ?>/amxbans.css" />
<style type="text/css">
<!--
body {margin:0; padding: 0;}
-->
</style>
<script type="text/javascript">
<!--
function GetAccess(id) {
	var access = opener.document.getElementById(id).value;
	SetBox( "chka" , access , "a" );
	SetBox( "chkb" , access , "b" );
	SetBox( "chkc" , access , "c" );
	SetBox( "chkd" , access , "d" );
	SetBox( "chke" , access , "e" );
	SetBox( "chkk" , access , "k" );
}
function SetBox( id , access , flag ) {
	var pos = access.indexOf(flag);
	if ( pos >= 0) {
		document.getElementById(id).checked=true;
	} else {
		document.getElementById(id).checked=false;
	}
	return true;
}
function SaveAccess( id ) {
	var access = "";
	access += GetBox( "chka" , "a");
	access += GetBox( "chkb" , "b");
	access += GetBox( "chkc" , "c");
	access += GetBox( "chkd" , "d");
	access += GetBox( "chke" , "e");
	access += GetBox( "chkk" , "k");
	
	opener.document.getElementById(id).value = access;
	WindowClose();
}
function GetBox( id , flag ) {
	if ( document.getElementById(id).checked == true ) {
		return flag;
	} else {
		return "";
	}
}
function WindowClose() {
	if (window.opener)
		self.close();
}
-->
</script>
</head>
<body onLoad="self.focus();GetAccess('<?php echo $id ?>');" style="margin:0; padding: 0;" >

<form name="amxhfrm" method="POST">
<table align="center" width="95%">
<tr><td><input type="checkbox" id="chka"> a - disconnect player on invalid password</input></td></tr>
<tr><td><input type="checkbox" id="chkb"> b - clan tag</input></td></tr>
<tr><td><input type="checkbox" id="chkc"> c - this is steamid/wonid</input></td></tr>
<tr><td><input type="checkbox" id="chkd"> d - this is ip</input></td></tr>
<tr><td><input type="checkbox" id="chke"> e - password is not checked (only name/ip/steamid needed)</input></td></tr>
<tr><td><input type="checkbox" id="chkk"> k - name or tag is case sensitive.  eg: if you set it so the name "Ham"<br/>
is protected and case sensitive (flags "k" only), then anybody<br/>
can use the names "haM", "HAM", "ham", etc, but not "Ham"</input></td></tr>
<tr><td align="center">
	<img src="../images/accept.png" style="cursor:pointer;" title="Accept" onclick="SaveAccess('<?php echo $id ?>');"/>
	<img src="../images/delete.png" style="cursor:pointer;" title="Delete" onclick="opener.document.getElementById('<?php echo $id ?>').value = '';self.close();"/>
	<img src="../images/cancel.png" style="cursor:pointer;" title="Cancel" onclick="self.close();"/>
</td></tr>
</table>

</form>
</body>
</html>