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

VarBoolean movement_auto_jof_auto("movement_auto_jof_auto", "Automatically tries to determine the right moment when to jump", true);
VarFloat movement_auto_jof_min_distance("movement_auto_jof_min_distance", "Minimal distance of where to jump from. 2 is good for 100 fps without network lags.", 2.0f, 0.5f, 10.0f);
VarInteger movement_auto_jof_interval("movement_auto_jof_interval", "Time in milliseconds of where to activate the jump again", 1000, 100, 2000);
VarInteger movement_auto_jof_activation_vel_min("movement_auto_jof_activation_vel_min", "Velocity where to start processing best JOF", 260, 0, 310);

void CMovementAutoJOF::update(float frametime)
{
	float gnd_angle = CLocalState::the().get_ground_angle();
	bool is_on_ladder = CLocalState::the().is_on_ladder();
	if (gnd_angle > 0.0f || is_on_ladder)
	{
		return;
	}

	static int timer = 0;

	float edge_dist = CLocalState::the().get_edge_dist();

	float vel_2d = CLocalState::the().get_local_velocity_2d();

	float min_vel = (float)movement_auto_jof_activation_vel_min.get_value();
	if (min_vel != 0 && vel_2d < min_vel)
	{
		return;
	}

	bool should_jump = false;

	if (movement_auto_jof_auto.get_value())
	{
		if (edge_dist <= (vel_2d * frametime) && edge_dist != 0)
		{
			should_jump = true;
		}
	}
	else
	{
		if (edge_dist < movement_auto_jof_min_distance.get_value() && edge_dist != 0)
		{
			should_jump = true;
		}
	}

	if (should_jump && (GetTickCount() - timer) > movement_auto_jof_interval.get_value())
	{
		//CMemoryFnDetourMgr::the().Key_Event().call(VK_SPACE, true);
		CClientMovementPacket::the().set_button_bit(IN_JUMP, true);
		timer = GetTickCount();
	}
}

