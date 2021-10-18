<div class="main">
	<div class="post">
		<table frame="box" rules="groups" summary=""> 
			<thead>
				<tr>
					<th style="text-align:center">{"_NICKNAME"|lang}</th> 
					{if $smarty.session.loggedin=="true"}
					<th style="text-align:center">{"_STEAMID"|lang}</th>
					<th style="text-align:center">{"_ACCESS"|lang}</th>
					{/if}
					<th style="text-align:center">{"_ADMINSINCE"|lang}</th>
					{if $smarty.session.loggedin=="true"}<th style="text-align:center;width:80px">{"_ADMINTO"|lang}</th>{/if}
					<th style="text-align:center">{"_ADMINLASTACTIVE"|lang}</th>
					<th style="text-align:center">Steam Profile</th>
				</tr>
			</thead>
			<tbody>
				<!-- Start Loop -->
				{foreach from=$admins item=admins}
					<tr class="list">
						<td style="text-align:center">{$admins.nickname}</td>
						{if $smarty.session.loggedin=="true"}
						<td style="text-align:center">{$admins.steamid}</td>
						<td style="text-align:center">{$admins.access}</td>
						{/if}
						<td style="text-align:center">{$admins.created|date_format:"%d. %b %Y - %T"}</td>
						{if $smarty.session.loggedin=="true"}
						<td style="text-align:center">{if $admins.expired=="0"}<i>{"_UNLIMITED"|lang}</i>{else}{$admins.expired|date_format:"%d. %b %Y - %T"}{/if}</td>
						{/if}
						<td style="text-align:center">{$admins.lastactive}</td>
						<td style="text-align:center"><a href="https://steamcommunity.com/profiles/{$admins.comid}" target="_blank"><img src="https://steamsignature.com/status/english/{$admins.comid}.png" alt="{"_OPENSTEAMCOMSITE"|lang}"/></a>&nbsp;</td>
					</tr> 
				{/foreach}
				<!-- Stop Loop -->
			</tbody> 
		</table> 
	</div>

	{if $smarty.session.loggedin=="true"}
	<div class="post _center">
		<form method="post" action="">
			<input type="button" class="button" name="showflags" value="{"_INFO_ACCESS"|lang}" onclick="$('#info_access').slideToggle('slow');"/><br/><br/>
		</form>
		<!--<a href="javascript:void(0);" class="button" onclick="ToggleLayer('info_access');">{"_INFO_ACCESS"|lang}</a>-->
	</div>

	<div id="info_access" class="post" style="display:none;">
		<table frame="box" rules="groups"> 
			<thead> 
				<tr> 
					<th>{"_ACCESSPERMS"|lang}</th> 
					<th style="width:350px;">{"_ACCESSFLAGS"|lang}</th>
				</tr> 
			</thead> 
			<tbody> 
				<tr> 
					<td>
						{"_ACCESS_FLAGS"|lang}
					</td> 
					<td class="vtop">
						{"_FLAG_FLAGS"|lang}
					</td> 
				</tr>
			</tbody> 
		</table> 
	</div>
	<div class="clearer">&nbsp;</div>
	{/if}
</div>
