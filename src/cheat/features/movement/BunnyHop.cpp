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
	bool is_onground = CLocalPlayerState::the().is_on_ground();

	// activates only the frame when on ground. This is extremely obvious when someone analyzes
	// your demo, but on the otherhand this creates almost no-slow-down effect.
	force_jump_cmd_buttons(cmd, is_onground);
}

void CMovementBunnyHop::legit_bhop(float frametime, hl::usercmd_t* cmd)
{
	//CConsole::the().info("--- start");
	float gnd_dist = CLocalPlayerState::the().get_ground_dist();

	bool is_onground = CLocalPlayerState::the().is_on_ground();
	float fall_vel_abs = CLocalPlayerState::the().get_fall_velocity_abs();

	bool is_surfing = CLocalPlayerState::the().is_surfing();
	float velocity_2d = CLocalPlayerState::the().get_local_velocity_2d();

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();

	bool requires_jump_release = movement_bhop_legit_method.get_value() == SIMULJMP_Command;

	determine_random_gnddist_for_jump_simul(is_onground);

	// predict if we will be on ground next frame.
	// we can predict it by using current frametime, and seeing if we will land to the ground
	// sooner than the frame time. That means that we should be on ground next frame already.
	float time_till_ground = gnd_dist / fall_vel_abs;
	bool will_be_on_ground_next_frame = fall_vel_abs > 0.0f && (time_till_ground < frametime);

	if (movement_bhop_legit_method.get_value() != SIMULJMP_Scroll && movement_bhop_legit_noslowdown.get_value() &&
		noslowdown_hop(cmd, will_be_on_ground_next_frame))
	{
		return;
	}

	bool is_really_near_ground = is_onground || will_be_on_ground_next_frame;

	// jump if we satisfy following conditions:
	bool perform_jump = !is_surfing && (is_onground || gnd_dist < m_simul_start_gnddist);

	// block the jump randomly unless we're really near the ground. this creates sort of a randomization
	// pattern for jumps.
	bool block_jump = false;
	static unsigned cnt = 0;
	if (++cnt % cl_enginefuncs->pfnRandomLong(1, 4) == 0 && !is_really_near_ground)
	{
		block_jump = true;
	}

	if (perform_jump && !block_jump)
	{
		simulate_jump(frametime, cmd, (ESimulJumpMethod)movement_bhop_legit_method.get_value());
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

			//CConsole::the().info("{}", flip_flop);

			break;
		}
	}
}

void CMovementBunnyHop::force_jump_cmd_buttons(hl::usercmd_t* cmd, bool do_the_jump)
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();
	if (do_the_jump && !m_retained_in_jump)
	{
		cmd->buttons |= IN_JUMP;
		//CConsole::the().info("IN_JUMP");
		CGameUtil::the().record_hud_command("+jump");
		m_retained_in_jump = true;
}
	else if (!do_the_jump && m_retained_in_jump)
	{
		cmd->buttons &= ~IN_JUMP;
		//CConsole::the().info("~IN_JUMP");
		CGameUtil::the().record_hud_command("-jump");
		m_retained_in_jump = false;
	}
	
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

bool CMovementBunnyHop::noslowdown_hop(hl::usercmd_t* cmd, bool will_be_on_ground_next_frame)
{
	if (will_be_on_ground_next_frame)
	{
		// releasae the cmd button just in case it wasn't - therefore we can do the +jump at the exact frame
		// where we land on ground.
		force_jump_cmd_buttons(cmd, false);
		return true;
	}

	return false;
}

