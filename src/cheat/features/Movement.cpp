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

VarBoolean movement_air_stuck_enable("movement_air_stuck_enable", "Allows to get stuck in the mid air, when on", false);
VarInteger movement_air_stuck_intensity("movement_air_stuck_intensity", "Determines how \"froze\" you will be", 0, 0, 5);

InCommandSimple CMovement::bunnyhop = InCommandSimple(
	"movement_bhop"
);

#if 0 // test
InCommandCustom incmd_test = InCommandCustom(
	"incmd_test", 
	[](InCommandCustom* _this)
	{
		CConsole::the().info("dn");
	},
	[](InCommandCustom* _this)
	{
		CConsole::the().info("up");
	}
);
#endif

void CMovement::update_air_stuck(hl::usercmd_t* to)
{
	if (!movement_air_stuck_enable.get_value())
	{
		return;
	}

	to->msec = movement_air_stuck_intensity.get_value();
}

void CMovement::update_clientmove(float frametime, hl::usercmd_t *cmd)
{
	if (bunnyhop.is_active())
	{
		bunnyhop_update(frametime, cmd);
	}
}

void CMovement::bunnyhop_update(float frametime, hl::usercmd_t *cmd)
{
	static bool flip_flop = false;

	if (flip_flop)
	{
		flip_flop = !flip_flop;
		return;
	}

	static int ground_frame = 0;

	float distance = abs(CLocalPlayerState::the().get_ground_dist() / CLocalPlayerState::the().get_fall_velocity() / frametime * 2);

	bool is_onground = CLocalPlayerState::the().is_on_ground();
	float velocity_2d = CLocalPlayerState::the().get_local_velocity_2d();
	float maxspeed = CLocalPlayerState::the().get_maxspeed();

	if (is_onground)
	{
		cmd->buttons |= IN_JUMP;
	}

	return;

	if (!flip_flop && (is_onground || (distance >= 5 && distance <= 18)))
	{
		if (velocity_2d <= maxspeed * 1.2f)
		{
			cmd->buttons |= IN_JUMP;
			flip_flop = true;
		}
		else
		{
			if (is_onground)
			{
				//if (ground_frame < 5)
				//{
				//	ground_frame++;
				//}
				//else
				{
					cmd->buttons |= IN_JUMP;
					flip_flop = true;
					ground_frame = 0;
				}
			}
		}
	}
}
