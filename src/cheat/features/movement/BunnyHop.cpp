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

VarBoolean movement_bhop_enable("movement_bhop_enable", "Enables bunnyhop", false);
VarInteger movement_bhop_mode("movement_bhop_mode", "Bunnyhop mode - legit or rage", 0, 0, 1);
VarBoolean movement_bhop_jump_on_ladder("movement_bhop_jump_on_ladder", "Jump if also on a ladder", true);
VarBoolean movement_bhop_jump_in_water("movement_bhop_jump_in_water", "Jump if in water", true);
VarInteger movement_bhop_jump_in_water_interval("movement_bhop_jump_in_water_interval", "Jump every Nth frame if in water", 7, 1, 20);
VarInteger movement_bhop_legit_method("movement_bhop_method", "Bunnyhop method", SIMULJMP_Scroll, SIMULJMP_Scroll, SIMULJMP_Command);
VarInteger movement_bhop_legit_ground_dist_min("movement_bhop_legit_ground_dist_min", "Minimal ground distance where to start jumping", 10, 5, 50);
VarInteger movement_bhop_legit_ground_dist_max("movement_bhop_legit_ground_dist_max", "Maximal ground distance where to start jumping", 25, 5, 50);
VarBoolean movement_bhop_legit_noslowdown("movement_bhop_legit_noslowdown", "Makes it so that you wont lose velocity that often", false);

void CMovementBunnyHop::update(float frametime, hl::usercmd_t* cmd)
{
	if (!movement_bhop_enable.get_value())
	{
		return;
	}

	int bhop_mode = movement_bhop_mode.get_value();

	bool is_in_water = CLocalPlayerState::the().get_waterlevel() >= 1;
	if (is_in_water && movement_bhop_jump_in_water.get_value())
	{
		auto method = (ESimulJumpMethod)movement_bhop_legit_method.get_value();
		jump_in_water(bhop_mode == 1 ? SIMULJMP_Command : method, cmd);
		return;
	}
	
	bool is_onladder = CLocalPlayerState::the().is_on_ladder();

	if (is_onladder && movement_bhop_jump_on_ladder.get_value())
	{
		auto method = (ESimulJumpMethod)movement_bhop_legit_method.get_value();
		ladder_jump(bhop_mode == 1 ? SIMULJMP_Command : method, cmd);
		return;
	}

	if (bhop_mode == 0)
	{
		// unlike rage, legit bhop provides a lot of funcionality and settings that you can tweak.
		legit_bhop(frametime, cmd);
	}
	else if (bhop_mode == 1)
	{ 
		// rage bhop is far more obvious but on the other hand, perfect.
		rage_bhop(frametime, cmd);
	}
}

void CMovementBunnyHop::rage_bhop(float frametime, hl::usercmd_t* cmd)
{
	// activates only the frame when on ground. This is extremely obvious when someone analyzes
	// your demo, but on the otherhand this creates almost no-slow-down effect.
	force_jump_cmd_buttons(cmd, perfect_condition_for_jump());
}

void CMovementBunnyHop::legit_bhop(float frametime, hl::usercmd_t* cmd)
{
	//CConsole::the().info("--- start");
	float gnd_dist = CLocalPlayerState::the().get_ground_dist();

	auto method = (ESimulJumpMethod)movement_bhop_legit_method.get_value();

	bool is_surfing = CLocalPlayerState::the().is_surfing();

	bool is_onground = perfect_condition_for_jump();
//	CConsole::the().info("is_onground: {}", is_onground);
	determine_random_gnddist_for_jump_simul(is_onground);

	if (noslowdown_hop(frametime, cmd))
	{
//		CConsole::the().info("Predicted that will be on ground next frame.");
		return;
	}

	// jump if we satisfy following conditions:
	bool perform_jump = !is_surfing && (is_onground || gnd_dist < m_simul_start_gnddist);

	// block the jump randomly unless we're really near the ground. this creates sort of a randomization
	// pattern for jumps.
	bool block_jump = false;
	static unsigned cnt = 0;
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();
	if (++cnt % cl_enginefuncs->pfnRandomLong(1, 4) == 0 && !is_onground)
	{
		block_jump = true;
	}

	if (perform_jump && !block_jump)
	{
		simulate_jump(frametime, cmd, method);
	}
	else
	{
		// release the jump, so we don't sustain in +jump... 'd be really obvious
		if (m_retained_in_jump)
		{
			//CConsole::the().info("blocked jump");
			force_jump_cmd_buttons(cmd, false);
		}
	}
	//CConsole::the().info("--- end");
}

void CMovementBunnyHop::simulate_jump(float frametime, hl::usercmd_t* cmd, ESimulJumpMethod method)
{
	switch (method)
	{
		case SIMULJMP_Scroll:
		{
			// don't do rapid mouse wheel, because not depending on the fps, the engine would 
			// get flooded by +/-jump commands and the whole key input would get slow and unresponsive.tatic unsigned cnt = 0;
			static int cnt = 0;
			if (++cnt % 2 == 0)
			{
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, WHEEL_DELTA, 0);
				//CConsole::the().info("scroll");
			}

			break;
		}
		case SIMULJMP_Command:
		{
			bool is_onground = perfect_condition_for_jump();
			static bool flip_flop = true;
			if (flip_flop || (is_onground && !m_retained_in_jump))
			{
				//CConsole::the().info("perform jump");
				force_jump_cmd_buttons(cmd, true);
				flip_flop = false;
			}
			else
			{
				//CConsole::the().info("disabled jump");
				force_jump_cmd_buttons(cmd, false);
				flip_flop = true;
			}

			//CConsole::the().info("{}", flip_flop);

			break;
		}
	}
}

void CMovementBunnyHop::force_jump_cmd_buttons(hl::usercmd_t* cmd, bool do_the_jump)
{
	static int in_jump_frame = 0;

	if (do_the_jump && in_jump_frame <= 1)
	{
		cmd->buttons |= IN_JUMP;
		//CConsole::the().info("IN_JUMP");
		CGameUtil::the().record_hud_command("+jump");
		in_jump_frame++;
	}
	else
	{
		cmd->buttons &= ~IN_JUMP;
		//CConsole::the().info("~IN_JUMP");
		CGameUtil::the().record_hud_command("-jump");
		in_jump_frame = 0;
	}

	m_retained_in_jump = do_the_jump;
	
	//
	// While setting/clearing bits inside cmd->buttons (IN_JUMP) may seem like a good idea, it is really not.
	// When recording a demo, anyone that inspects it afterwards with some kind of tool that can pick up
	// usercmd_t buttons and commands executed on the client can see, whenever you're cheating or not. That is,
	// because when you just clear/set bits inside cmd->buttons, there isn't any +/-jump command! And this is it!
	// So, what's the solution? Well, we can send both! Set the IN_JUMP bit and then the +/-jump command.
	// 
	// Additional notes:
	//  When executing the +jump cmd, the command will be processed next frame, because the prediction code that
	//  runs playermove code doesn't know about this command yet, because CBuf_Execute will be called in the next
	//  frame! However, if we set the buttons directly - the pm code will register it this frame, and the command 
	//  will be scheduled, too. (It will be executed the next frame, but whatever - I doubt that people could make 
	//  something out of this)
	//
}

void CMovementBunnyHop::determine_random_gnddist_for_jump_simul(bool on_ground)
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();
	static bool was_on_ground = false;
	if (on_ground != was_on_ground)
	{
		int min = movement_bhop_legit_ground_dist_min.get_value();
		int max = movement_bhop_legit_ground_dist_max.get_value();

		// randomize the value each time we hit/leave ground
		if (on_ground)
		{
			m_simul_start_gnddist = (float)cl_enginefuncs->pfnRandomLong(min, max); // ground leaving distance
		}
		else
		{
			m_simul_start_gnddist = (float)cl_enginefuncs->pfnRandomLong(min, max); // ground entering distance
		}

		was_on_ground = on_ground;
	}
}

bool CMovementBunnyHop::noslowdown_hop(float frametime, hl::usercmd_t* cmd)
{
	if (!movement_bhop_legit_noslowdown.get_value())
	{
		return false;
	}

	float gnd_dist = CLocalPlayerState::the().get_ground_dist();
	float fall_vel_abs = CLocalPlayerState::the().get_fall_velocity_abs();

	// predict if we will be on ground next frame.
	// we can predict it by using current frametime, and seeing if we will land to the ground
	// sooner than the frame time. That means that we should be on ground next frame already.
	float time_till_ground = gnd_dist / fall_vel_abs;
	bool will_be_on_ground_next_frame = fall_vel_abs > 0.0f && (time_till_ground < frametime * 2.0f); // some reserve here...

	if (will_be_on_ground_next_frame)
	{
		// releasae the cmd button just in case it wasn't - therefore we can do the +jump at the exact frame
		// where we land on ground.
		force_jump_cmd_buttons(cmd, false);
		return true;
	}

	return false;
}

bool CMovementBunnyHop::perfect_condition_for_jump()
{
	bool is_onground = CLocalPlayerState::the().is_on_ground();

	float gnd_dist = CLocalPlayerState::the().get_ground_dist();

	// HACKHACK
	return is_onground || gnd_dist <= 0.01f;
}

void CMovementBunnyHop::ladder_jump(ESimulJumpMethod method, hl::usercmd_t* cmd)
{
	if (method == SIMULJMP_Command)
	{
		force_jump_cmd_buttons(cmd, true);
	}
	else if (method == SIMULJMP_Scroll)
	{
		mouse_event(MOUSEEVENTF_WHEEL, 0, 0, WHEEL_DELTA, 0);
	}
}


void CMovementBunnyHop::jump_in_water(ESimulJumpMethod method, hl::usercmd_t* cmd)
{
	static int cnt = 0;

	if (++cnt % movement_bhop_jump_in_water_interval.get_value() != 0)
	{
		return;
	}

	// jump every 10 frames

	if (method == SIMULJMP_Command)
	{
		static bool flip_flop = true;
		if (flip_flop)
		{
			//CConsole::the().info("perform jump");
			force_jump_cmd_buttons(cmd, true);
			flip_flop = false;
		}
		else
		{
			//CConsole::the().info("disabled jump");
			force_jump_cmd_buttons(cmd, false);
			flip_flop = true;
		}
	}
	else if (method == SIMULJMP_Scroll)
	{
		mouse_event(MOUSEEVENTF_WHEEL, 0, 0, WHEEL_DELTA, 0);
	}
}

