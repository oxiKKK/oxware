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

InCommand CMovement::bunnyhop = InCommand("movement_bhop", VK_SPACE, false, IN_ACTCOND_Alive | IN_ACTCOND_Connected);
InCommand CMovement::airstuck = InCommand("movement_air_stuck", VK_XBUTTON1, false, IN_ACTCOND_Alive | IN_ACTCOND_Connected); // mouse4
InCommand CMovement::gs = InCommand("movement_gs", VK_XBUTTON2, false, IN_ACTCOND_Alive | IN_ACTCOND_Connected); // mouse5
InCommand CMovement::eb = InCommand("movement_eb", 'C', false, IN_ACTCOND_Alive | IN_ACTCOND_Connected );
InCommand CMovement::strafe = InCommand("movement_strafe_hack", VK_MBUTTON, false, IN_ACTCOND_Alive | IN_ACTCOND_Connected ); // mouse3
InCommand CMovement::strafe_helper = InCommand("movement_strafe_helper", NULL, false, IN_ACTCOND_Alive | IN_ACTCOND_Connected );
InCommand CMovement::fastrun = InCommand("movement_fastrun", NULL, false, IN_ACTCOND_Alive | IN_ACTCOND_Connected );
InCommand CMovement::auto_jof = InCommand("movement_auto_jof", NULL, false, IN_ACTCOND_Alive | IN_ACTCOND_Connected );

VarBoolean debug_render_info_movement("debug_render_info_movement", "Movement information", false);
VarBoolean debug_render_info_movement_bhop("debug_render_info_bhop", "Bunnyhop information", false);
VarBoolean debug_render_info_movement_strafe("debug_render_info_movement_strafe", "Strafehack information", false);
VarBoolean debug_render_info_movement_strafe_helper("debug_render_info_movement_strafe_helper", "Strafe helper information", false);

void CMovement::update_clientmove(float frametime, hl::usercmd_t *cmd)
{
	run_incommands(frametime, cmd);

	run_debug(cmd);

	feed_plot(frametime, cmd);
}

void CMovement::run_incommands(float frametime, hl::usercmd_t *cmd)
{
	if (bunnyhop.is_active())
	{
		CMovementBunnyHop::the().update(frametime);
	}

	if (gs.is_active())
	{
		CMovementGroundStrafe::the().update(frametime);
	}

	if (eb.is_active())
	{
		CMovementEdgeBug::the().update(frametime);
	}

	if (airstuck.is_active())
	{
		CMovementAirStuck::the().update();
	}

	if (strafe_helper.is_active())
	{
		CMovementStrafeHelper::the().update();
	}

	if (strafe.is_active())
	{
		CMovementStrafeHack::the().update();
	}

	if (fastrun.is_active())
	{
		CMovementFastRun::the().update();
	}

	if (auto_jof.is_active())
	{
		CMovementAutoJOF::the().update(frametime);
	}
}

void CMovement::run_debug(hl::usercmd_t* cmd)
{
	if (debug.get_value())
	{
		if (debug_render_info_movement.get_value())
		{
			render_debug(cmd);
		}

		if (debug_render_info_movement_bhop.get_value())
		{
			CMovementBunnyHop::the().render_debug();
		}

		if (debug_render_info_movement_strafe.get_value())
		{
			CMovementStrafeHack::the().render_debug();
		}

		if (debug_render_info_movement_strafe_helper.get_value())
		{
			CMovementStrafeHelper::the().render_debug();
		}
	}
}

void CMovement::render_debug(hl::usercmd_t* cmd)
{
	CEngineFontRendering::the().render_debug("--- Movement ---");

	float gnd_dist = CLocalState::the().get_ground_dist();
	float edge_dist = CLocalState::the().get_edge_dist();
	float gnd_angle = CLocalState::the().get_ground_angle();
	bool is_surfing = CLocalState::the().is_surfing();
	float fall_vel = CLocalState::the().get_fall_velocity();
	float vel_2d = CLocalState::the().get_local_velocity_2d();
	Vector vel_vec = CLocalState::the().get_local_velocity_vec();

	auto pmove = *CMemoryHookMgr::the().pmove().get();

	float velocity = CLocalState::the().get_local_velocity_2d();
	static float last_velocity = velocity;

	static float rolling_accel = 0.0f;

	rolling_accel = 0.9f * rolling_accel + (1.0f - 0.9f) * (velocity - last_velocity);
	last_velocity = velocity;

	float movespeed = sqrt((cmd->forwardmove * cmd->forwardmove) + (cmd->sidemove * cmd->sidemove) + (cmd->upmove * cmd->upmove));

	CEngineFontRendering::the().render_debug("Acceleration: {:0.3f} u/frame", rolling_accel);
	CEngineFontRendering::the().render_debug("Ground distance: {:0.3f} units", gnd_dist);
	CEngineFontRendering::the().render_debug("Edge distance: {:0.3f} units", edge_dist);
	CEngineFontRendering::the().render_debug("Ground angle: {:0.1f} a", gnd_angle);
	CEngineFontRendering::the().render_debug("Fall velocity: {:0.3f} u/s", fall_vel);
	CEngineFontRendering::the().render_debug("Velocity 2D: {:0.3f} u/s", vel_2d);
	CEngineFontRendering::the().render_debug("Velocity 3D: {} u/s", vel_vec);
	CEngineFontRendering::the().render_debug("Is surfing: {}", is_surfing);
	CEngineFontRendering::the().render_debug("Water level: {}", pmove->waterlevel);
	CEngineFontRendering::the().render_debug("Water type: {}", pmove->watertype);
	CEngineFontRendering::the().render_debug("Water jump time: {}", pmove->waterjumptime);
	CEngineFontRendering::the().render_debug("Forwardmove: {}", cmd->forwardmove);
	CEngineFontRendering::the().render_debug("Sidemove: {}", cmd->sidemove);
	CEngineFontRendering::the().render_debug("Maxspeed: {}", pmove->maxspeed);
	CEngineFontRendering::the().render_debug("Movespeed: {}", movespeed);
	CEngineFontRendering::the().render_debug("Viewangles: {}", cmd->viewangles);
	CEngineFontRendering::the().render_debug("Yaw: {} a", cmd->viewangles[YAW]);
	CEngineFontRendering::the().render_debug("usehull: {}", pmove->usehull);

	auto va_delta = CLocalState::the().get_viewangle_delta();
	CEngineFontRendering::the().render_debug("VA delta: {}", va_delta);
}

void CMovement::feed_plot(float frametime, hl::usercmd_t * cmd)
{
	CClientMovementPacketPlot::the().feed_by_name("On Ground", CColor(70, 130, 180, 230), MPVisualDataEntry(CLocalState::the().is_on_ground_safe()));
	CClientMovementPacketPlot::the().feed_by_name("Velocity", CColor(255, 69, 0, 230), MPVisualDataEntry(CLocalState::the().get_local_velocity_2d(), 500));
	CClientMovementPacketPlot::the().feed_by_name("Ground Dist", CColor(255, 69, 0, 230), MPVisualDataEntry(CLocalState::the().get_ground_dist(), 300));
	CClientMovementPacketPlot::the().feed_by_name("IN_JUMP", CColor(255, 192, 203, 230), MPVisualDataEntry((cmd->buttons & IN_JUMP)));
	CClientMovementPacketPlot::the().feed_by_name("IN_DUCK", CColor(218, 165, 32, 230), MPVisualDataEntry((cmd->buttons & IN_DUCK)));
	CClientMovementPacketPlot::the().feed_by_name("IN_MOVELEFT", CColor(218, 165, 32, 230), MPVisualDataEntry((cmd->buttons & IN_MOVELEFT)));
	CClientMovementPacketPlot::the().feed_by_name("IN_MOVERIGHT", CColor(32, 128, 68, 230), MPVisualDataEntry((cmd->buttons & IN_MOVERIGHT)));
	CClientMovementPacketPlot::the().feed_by_name("IN_FORWARD", CColor(218, 165, 124, 230), MPVisualDataEntry((cmd->buttons & IN_FORWARD)));
	CClientMovementPacketPlot::the().feed_by_name("IN_BACK", CColor(34, 213, 32, 230), MPVisualDataEntry((cmd->buttons & IN_BACK)));
}
