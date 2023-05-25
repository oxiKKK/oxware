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
VarInteger movement_bhop_method("movement_bhop_method", "Bunnyhop method", SIMULJMP_Scroll, SIMULJMP_Scroll, SIMULJMP_Command);

void CMovementBunnyHop::update(float frametime, hl::usercmd_t* cmd)
{
	if (!movement_bhop_enable.get_value())
	{
		return;
	}

	static int ground_frame = 0;

	float distance = CLocalPlayerState::the().get_ground_dist();

	bool is_onground = CLocalPlayerState::the().is_on_ground();
	float velocity_2d = CLocalPlayerState::the().get_local_velocity_2d();
	float maxspeed = CLocalPlayerState::the().get_maxspeed();
	float fall_vel = CLocalPlayerState::the().get_fall_velocity();

	m_fall_velocity_factor = sqrt(abs(fall_vel) * 2.0f) * 0.075f; // tweaked to perfection

	bool is_surfing = CLocalPlayerState::the().is_surfing();

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();

	bool requires_jump_release = movement_bhop_method.get_value() == SIMULJMP_Command;

	determine_random_gnddist_for_jump_simul(is_onground);

	bool is_really_near_ground = is_onground || distance < (5.0f * m_fall_velocity_factor);

	//CConsole::the().info("sqrt(fall_vel): {}", 5.0f * m_fall_velocity_factor);

	static unsigned cnt = 0;

	bool perform_jump = !is_surfing && (is_onground || distance < m_simul_start_gnddist);

	// block the jump randomly unless we're really near the ground
	bool block_jump = false;
	if (++cnt % cl_enginefuncs->pfnRandomLong(1, 4) == 0 && !is_really_near_ground)
	{
		block_jump = true;
	}

	if (perform_jump && !block_jump)
	{
		simulate_jump(frametime, cmd, (ESimulJumpMethod)movement_bhop_method.get_value());
	}
	else
	{
		if (requires_jump_release && m_was_in_jump_lock)
		{
			//CConsole::the().info("blocked jump");
			exec_jump_cmd(false);
			m_was_in_jump_lock = false; // do this only once, otherwise would look blatant
		}
	}
}

void CMovementBunnyHop::simulate_jump(float frametime, hl::usercmd_t* cmd, ESimulJumpMethod method)
{
	switch (method)
	{
		case SIMULJMP_Scroll:
		{
			static unsigned cnt = 0;

			if (++cnt % 2 == 0)
			{
				mouse_event(MOUSEEVENTF_WHEEL, 0, 0, WHEEL_DELTA, 0);
				//CConsole::the().info("scroll");
			}
			else
			{
				//CConsole::the().info("not");
			}

			break;
		}
		case SIMULJMP_Command:
		{
			static bool flip_flop = true;

			if (flip_flop)
			{
				m_was_in_jump_lock = true;

				//CConsole::the().info("perform jump");
				exec_jump_cmd(true);
				flip_flop = false;
			}
			else
			{
				//CConsole::the().info("disabled jump");
				exec_jump_cmd(false);
				flip_flop = true;
			}

			//CConsole::the().info("{}", flip_flop);

			break;
		}
	}
}

//
// While setting/clearing bits inside cmd->buttons (IN_JUMP) may seem like a good idea, it is really not.
// When recording a demo, anyone that inspects it afterwards with some kind of tool that can pick up
// usercmd_t buttons and commands executed on the client can see, whenever you're cheating or not. That is,
// because when you just clear/set bits inside cmd->buttons, there isn't any +/-jump command! And this it!
// So, what's the solution? Well, either fake the demo recording code to send +/-jump commands, even tho we
// will clear/set bits or we can just execute the command. The second option is an easier one... let's roll with it...
//

void CMovementBunnyHop::exec_jump_cmd(bool do_the_jump)
{
	CMemoryHookMgr::the().cl_enginefuncs().get()->pfnClientCmd(do_the_jump ? (char*)"+jump" : (char*)"-jump");
}

void CMovementBunnyHop::determine_random_gnddist_for_jump_simul(bool on_ground)
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();
	static bool was_on_ground = false;
	if (on_ground != was_on_ground)
	{
		if (on_ground)
		{
			m_simul_start_gnddist = m_fall_velocity_factor * cl_enginefuncs->pfnRandomFloat(10.0f, 25.0f); // ground leaving distance
		}
		else
		{
			m_simul_start_gnddist = m_fall_velocity_factor * cl_enginefuncs->pfnRandomFloat(10.0f, 25.0f); // ground entering distance
		}
		was_on_ground = on_ground;
	}
}

