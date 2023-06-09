/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
* 
*	This program is licensed under the MIT license. By downloading, copying, 
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a 
*	copy of this software and associated documentation files (the "Software"), 
*	to deal in the Software without restriction, including without limitation 
*	the rights to use, copy, modify, merge, publish, distribute, sublicense, 
*	and/or sell copies of the Software, and to permit persons to whom the 
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included 
*	in all copies or substantial portions of the Software. 
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
*	IN THE SOFTWARE.
*/

#include "precompiled.h"

VarBoolean remove_screenshake("remove_screenshake", "Removes all screenshake.", false);
VarBoolean remove_viewmodel("remove_viewmodel", "Removes viewmodel.", false);

bool CRemovals::remove_screenshake()
{
	return ::remove_screenshake.get_value();
}

bool CRemovals::remove_viewmodel()
{
	return ::remove_viewmodel.get_value();
}

VarBoolean remove_hud_enable("remove_hud_enable", "Enabled HUD removal.", false);
VarBoolean remove_hud_crosshair("remove_hud_crosshair", "Removes crosshair from the HUD.", false);
VarBoolean remove_hud_weapons("remove_hud_weapons", "Removes weapon pickup indicators from the HUD.", false);
VarBoolean remove_hud_flashlight("remove_hud_flashlight", "Removes flashlight from the HUD.", false);
VarBoolean remove_hud_health("remove_hud_health", "Removes health from the HUD.", false);
VarBoolean remove_hud_timer("remove_hud_timer", "Removes timer from the HUD.", false);
VarBoolean remove_hud_money("remove_hud_money", "Removes money from the HUD.", false);

void CRemovals::remove_hud_modifier()
{
	int return_flags = 0;
	static int prev_return_flags = return_flags;

	if (remove_hud_enable.get_value())
	{
		if (remove_hud_weapons.get_value())		return_flags |= HIDEHUD_WEAPONS;
		if (remove_hud_crosshair.get_value())	return_flags |= HIDEHUD_MISCSTATUS;
		if (remove_hud_flashlight.get_value())	return_flags |= HIDEHUD_FLASHLIGHT;
		if (remove_hud_health.get_value())		return_flags |= HIDEHUD_HEALTH;
		if (remove_hud_timer.get_value())		return_flags |= HIDEHUD_PLAYERDEAD;
		if (remove_hud_money.get_value())		return_flags |= HIDEHUD_NEEDSUIT;
	}

	if (CUserMSGDetourMgr::the().HideWeapon_fn().is_installed() && prev_return_flags != return_flags)
	{
		CUserMSGDetourMgr::the().HideWeapon_fn().call_usermsg(sizeof(byte), &return_flags);

		prev_return_flags = return_flags;
	}
}

VarBoolean remove_motd("remove_motd", "When on, no MOTD dialog will be shown", false);

bool CRemovals::remove_motd()
{
	return ::remove_motd.get_value();
}

VarBoolean remove_sniper_scope("remove_sniper_scope", "Removes sniper scope when scoped in", false);

bool CRemovals::remove_sniper_scope()
{
	return ::remove_sniper_scope.get_value();
}
