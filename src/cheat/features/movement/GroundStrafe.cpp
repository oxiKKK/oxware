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

VarInteger movement_gs_mode("movement_gs_mode", "GroundStrafe mode - legit or rage", GSMODE_Legit, GSMODE_Legit, GSMODE_Rage);
VarBoolean movement_gs_mode_noslowdown("movement_gs_mode_noslowdown", "Enables GS noslowdown", false);
VarInteger movement_gs_mode_noslowdown_method("movement_gs_mode_noslowdown_method", "Noslowdown method", GSNSDN_EngineSpeed, GSNSDN_ServerSpeed, GSNSDN_EngineSpeed);
VarInteger movement_gs_mode_noslowdown_factor("movement_gs_mode_noslowdown_factor", "How much to no-slowdown", 1, 1, 10);
VarInteger movement_gs_legit_ground_dist_min("movement_gs_legit_ground_dist_min", "Minimal ground distance where to start ducking", 5, 2, 20);
VarInteger movement_gs_legit_ground_dist_max("movement_gs_legit_ground_dist_max", "Maximal ground distance where to start ducking", 12, 2, 20);
VarInteger movement_gs_legit_efficiency("movement_gs_legit_efficiency", "Efficiency settings of the GS", 1, 0, 2);
VarInteger movement_gs_legit_scroll_density("movement_gs_legit_scroll_density", "How dense will the resulting scroll pattern be", 5, 1, 10);

void CMovementGroundStrafe::update(float frametime)
{
	int gs_mode = movement_gs_mode.get_value();

	bool is_surfing = CLocalState::the().is_surfing();
	if (is_surfing)
	{
		return;
	}

	if (gs_mode == GSMODE_Legit)
	{
		// legit GS is more tweakable and more legitimate
		legit_gs(frametime);
	}
	else if (gs_mode == GSMODE_Rage)
	{
		// rage GS is just blatant but perfect
		rage_gs(frametime);
	}
}

void CMovementGroundStrafe::rage_gs(float frametime)
{
	static bool did_duck = false;
	if (perfect_condition_for_duck() && !did_duck)
	{
		CClientMovementPacket::the().set_button_bit(IN_DUCK, true);
		nsdn_speedhack();
		did_duck = true;
	}
	else
	{
		if (did_duck)
		{
			CClientMovementPacket::the().set_button_bit(IN_DUCK, false);
			did_duck = false;
		}
	}
}

void CMovementGroundStrafe::legit_gs(float frametime)
{
	float gnd_dist = CLocalState::the().get_ground_dist();
	bool is_onground = perfect_condition_for_duck();
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	determine_random_gnddist_for_duck_simul(is_onground);

	auto gs_eff = (EGSEfficiency)movement_gs_legit_efficiency.get_value();

	if (gs_eff == GSEFF_Helper && duck_helper(frametime))
	{
		return;
	}

	// duck if we satisfy following conditions:
	bool perform_duck = (is_onground || gnd_dist < m_simul_start_gnddist);

	// block the duck randomly unless we're really near the ground. this creates sort of a randomization
	// pattern for ducks.
	bool block_duck = false;
	if (randomize_duck_pattern(is_onground))
	{
		block_duck = true;
	}

	if (perform_duck && !block_duck)
	{
		if (gs_eff == GSEFF_RandomFOGBased)
		{
			// randomize this as well
			static int cnt = 0;
			if (++cnt % cl_enginefuncs->pfnRandomLong(1, 10) == 0)
			{
				return;
			}
		}

		if (is_onground)
		{
			nsdn_speedhack();
		}

		simulate_duck(frametime);
	}
	else
	{
		if (m_remained_in_duck)
		{
			if (!CClientMovementPacket::the().set_button_bit_atomic(IN_DUCK))
			{
				m_remained_in_duck = false;
			}
		}
	}
}

void CMovementGroundStrafe::simulate_duck(float frametime)
{
	m_remained_in_duck = CClientMovementPacket::the().set_button_bit_atomic(IN_DUCK);
}

void CMovementGroundStrafe::determine_random_gnddist_for_duck_simul(bool on_ground)
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();
	static bool was_on_ground = false;
	if (on_ground != was_on_ground)
	{
		int min = movement_gs_legit_ground_dist_min.get_value();
		int max = movement_gs_legit_ground_dist_max.get_value();

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

bool CMovementGroundStrafe::duck_helper(float frametime)
{
	// try to predict if we'll be on ground next frame. this is a hard one.
	if (predicted_nextframe_on_ground(frametime))
	{
		// releasae the cmd button just in case it wasn't - therefore we can do the +duck at the exact frame
		// where we land on ground.
		CClientMovementPacket::the().set_button_bit(IN_DUCK, false);
		return true;
	}

	return false;
}

bool CMovementGroundStrafe::perfect_condition_for_duck()
{
	return CLocalState::the().is_on_ground_safe();
}

bool CMovementGroundStrafe::randomize_duck_pattern(bool is_onground)
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();
	static unsigned cnt = 0;
	int dens = movement_gs_legit_scroll_density.get_value();
	int max = 10;
	if (dens == max)
	{
		return false;
	}

	if (++cnt % cl_enginefuncs->pfnRandomLong(1, max - dens) != 0 && !is_onground)
	{
		return true;
	}

	return false;
}

bool CMovementGroundStrafe::predicted_nextframe_on_ground(float frametime)
{
	float gnd_dist = CLocalState::the().get_ground_dist();
	float fall_vel = CLocalState::the().get_fall_velocity();
	bool is_onground = perfect_condition_for_duck();

	if (is_onground)
	{
		// already on ground
		return false;
	}

	if (gnd_dist == 0.0f || fall_vel == 0.0f)
	{
		return false;
	}

	// See if our fall distance is bigger than the distance to ground. Then we're
	// guaranteed to be next frame on the ground.
	return fall_vel > 0.0f && ((fall_vel * frametime) * 2.0f > gnd_dist);
}

void CMovementGroundStrafe::nsdn_speedhack()
{
	if (!movement_gs_mode_noslowdown.get_value())
	{
		return;
	}

	auto method = (EGSNoSlowDownMethod)movement_gs_mode_noslowdown_method.get_value();
	int factor = movement_gs_mode_noslowdown_factor.get_value();

	switch (method)
	{
		case GSNSDN_ServerSpeed:
		{
			int speed = std::max((10 - (factor - 1)), 0);
			CEngineSpeedControl::the().server_speedhack(speed);
			break;
		}
		case GSNSDN_EngineSpeed:
		{
			CEngineSpeedControl::the().classic_cs16_cheating_scene_speedhack(((double)(10 - (factor - 1)) / 4.0) / 10.0);
			break;
		}
	}
}
