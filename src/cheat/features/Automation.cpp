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

VarBoolean auto_reload("auto_reload", "Automatically reloads when the clip is empty", false);
VarBoolean auto_pistol("auto_pistol", "Shoots as fast as possible when the fire button is held", false);
VarBoolean auto_zoom("auto_zoom", "Automatically zooms in when the weapon is fired", false);

void CAutomation::update()
{
	// chat opened
	if (CLocalState::the().is_in_messagemode())
	{
		return;
	}

	if (::auto_reload.get_value())
	{
		auto_reload();
	}

	if (::auto_pistol.get_value())
	{
		auto_pistol();
	}

	if (::auto_zoom.get_value())
	{
		auto_zoom();
	}
}

void CAutomation::auto_reload()
{
	auto current_weapon = CWeapons::the().get_current_weapon();
	
	if (CClientMovementPacket::the().is_in(IN_ATTACK) && current_weapon->is_out_of_ammo() && current_weapon->is_gun())
	{
		CClientMovementPacket::the().set_button_bit(IN_ATTACK, false);
		CClientMovementPacket::the().set_button_bit(IN_RELOAD, true);
	}
}

void CAutomation::auto_pistol()
{
	auto current_weapon = CWeapons::the().get_current_weapon();
	
	if (CClientMovementPacket::the().is_in(IN_ATTACK) && current_weapon->is_pistol() && !current_weapon->is_reloading())
	{
		static bool fire = false;

		if (current_weapon->can_attack() && fire)
		{
			CClientMovementPacket::the().set_button_bit(IN_ATTACK, true);
			fire = false;
		}
		else if (!fire)
		{
			CClientMovementPacket::the().set_button_bit(IN_ATTACK, false);
			fire = true;
		}
	}
}

void CAutomation::auto_zoom()
{
	auto current_weapon = CWeapons::the().get_current_weapon();

	if (CClientMovementPacket::the().is_in(IN_ATTACK) && current_weapon->is_sniper() && CLocalState::the().get_fov() == 90)
	{
		CClientMovementPacket::the().set_button_bit(IN_ATTACK, false);
		CClientMovementPacket::the().set_button_bit(IN_ATTACK2, true);
	}
}
