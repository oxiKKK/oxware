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

// Note that this code was inspired by Tkz.. yikes.

#include "precompiled.h"

VarBoolean movement_fastrun_no_slowdown("movement_fastrun_no_slowdown", "Enables you to not lose speed that much while fastrunning", false);
VarInteger movement_fastrun_max_speed("movement_fastrun_max_speed", "Enables you to not lose speed that much while fastrunning", 277, 251, 278);
VarBoolean movement_fastrun_fast_walk("movement_fastrun_fast_walk", "Enables you to walk fast without making stepping sounds", false);
VarInteger movement_fastrun_fast_walk_max_speed("movement_fastrun_fast_walk_max_speed", "Tolerance of how fast you want to walk while speed-walking", 140, 131, 149);

void CMovementFastRun::update()
{
	bool is_onground = CLocalState::the().is_on_ground_safe();
	bool is_surfing = CLocalState::the().is_surfing();
	if (!is_onground || is_surfing)
	{
		return;
	}

	if (!CClientMovementPacket::the().is_moving())
	{
		return;
	}

	if (movement_fastrun_fast_walk.get_value())
	{
		if (fast_walk())
		{
			return;
		}
	}

	float vel_2d = CLocalState::the().get_local_velocity_2d();
	if (vel_2d > movement_fastrun_max_speed.get_value())
	{
		return;
	}

	auto pmove = *CMemoryHookMgr::the().pmove().get();

	if (movement_fastrun_no_slowdown.get_value() && vel_2d >= pmove->maxspeed)
	{
		faster_run();
	}
	else
	{
		slower_run();
	}
}

void CMovementFastRun::faster_run()
{
	float angle_to_add = 0.0f;

	auto cmd = CClientMovementPacket::the().get_cmd();

	if (CClientMovementPacket::the().is_in(IN_MOVELEFT) && CClientMovementPacket::the().is_in(IN_MOVERIGHT))
	{
		return; // stop
	}

	if (CClientMovementPacket::the().is_in(IN_FORWARD) && CClientMovementPacket::the().is_in(IN_BACK))
	{
		return; // stop
	}

	if (CClientMovementPacket::the().is_in(IN_FORWARD))
	{
		if (CClientMovementPacket::the().is_in(IN_MOVELEFT))
		{
			angle_to_add = 45.0f;
		}
		else if (CClientMovementPacket::the().is_in(IN_MOVERIGHT))
		{
			angle_to_add = -45.0f;
		}
		else
		{
			angle_to_add = 0.0f;
		}
	}
	else if (CClientMovementPacket::the().is_in(IN_BACK))
	{
		if (CClientMovementPacket::the().is_in(IN_MOVELEFT))
		{
			angle_to_add = 135.0f;
		}
		else if (CClientMovementPacket::the().is_in(IN_MOVERIGHT))
		{
			angle_to_add = -135.0f;
		}
		else
		{
			angle_to_add = 180.0f;
		}
	}
	else if (CClientMovementPacket::the().is_in(IN_MOVELEFT))
	{
		angle_to_add = 90.0f;
	}
	else if (CClientMovementPacket::the().is_in(IN_MOVERIGHT))
	{
		angle_to_add = -90.0f;
	}

	auto pmove = *CMemoryHookMgr::the().pmove().get();
	auto cl = CMemoryHookMgr::the().cl().get();

	bool dir_rig = true;

	float vel_yaw = CMath::the().rad2deg(atan2(pmove->velocity.y, pmove->velocity.x));

	vel_yaw -= angle_to_add;
	vel_yaw = cl->viewangles[YAW] - vel_yaw;

	int fadif = (int)vel_yaw;

	fadif = (fadif + 360) % 360;
	dir_rig = fadif <= 180;

	float x = 135.063748f * (dir_rig ? -1 : +1);
	float y = 267.876434f;

	float a = CMath::the().deg2rad(angle_to_add);
	float sinA = sin(a);
	float cosA = cos(a);

	cmd->sidemove = x * cosA - y * sinA;
	cmd->forwardmove = y * cosA + x * sinA;

	x = cmd->sidemove;
	y = cmd->forwardmove;

	float newa = CMath::the().deg2rad(-vel_yaw);
	sinA = sin(newa);
	cosA = cos(newa);

	cmd->sidemove = x * cosA - y * sinA;
	cmd->forwardmove = y * cosA + x * sinA;
}

void CMovementFastRun::slower_run()
{
	auto cmd = CClientMovementPacket::the().get_cmd();

	static bool fast_run = false;
	if ((cmd->buttons & IN_FORWARD && cmd->buttons & IN_MOVELEFT) || (cmd->buttons & IN_BACK && cmd->buttons & IN_MOVERIGHT))
	{
		if (fast_run)	{ fast_run = false; cmd->sidemove -= 89.6f; cmd->forwardmove -= 89.6f; }
		else			{ fast_run = true;  cmd->sidemove += 89.6f; cmd->forwardmove += 89.6f; }
	}
	else if ((cmd->buttons & IN_FORWARD && cmd->buttons & IN_MOVERIGHT) || (cmd->buttons & IN_BACK && cmd->buttons & IN_MOVELEFT))
	{
		if (fast_run)	{ fast_run = false; cmd->sidemove -= 89.6f; cmd->forwardmove += 89.6f; }
		else			{ fast_run = true;  cmd->sidemove += 89.6f; cmd->forwardmove -= 89.6f; }
	}
	else if (cmd->buttons & IN_FORWARD || cmd->buttons & IN_BACK)
	{
		if (fast_run)	{ fast_run = false; cmd->sidemove -= 126.6f; }
		else			{ fast_run = true;  cmd->sidemove += 126.6f; }
	}
	else if (cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT)
	{
		if (fast_run)	{ fast_run = false; cmd->forwardmove -= 126.6f; }
		else			{ fast_run = true;  cmd->forwardmove += 126.6f; }
	}
}

bool CMovementFastRun::fast_walk()
{
	bool in_speed = CHLCommandsDetourMgr::the().in_speed;

	float vel_2d = CLocalState::the().get_local_velocity_2d();

	// after 150 u/s, you start making stepping sounds however, for example when you turn
	// your speed gets increased, so it eventually hits 150 and you make a sound.
	if (vel_2d > movement_fastrun_fast_walk_max_speed.get_value() && in_speed)
	{
		return true;
	}

	return false;
}
