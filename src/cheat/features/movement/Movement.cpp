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

VarBoolean movement_bhop_enable("movement_bhop_enable", "Enables BunnyHop hacks", false);
VarBoolean movement_air_stuck_enable("movement_air_stuck_enable", "Allows to get stuck in the mid air, when on", false);
VarBoolean movement_gs_enable("movement_gs_enable", "Enables GroundStrafe hacks", false);
VarBoolean movement_eb_enable("movement_eb_enable", "Enables EdgeBug hacks", false);

InCommand CMovement::bunnyhop = InCommand("movement_bhop", VK_SPACE, &movement_bhop_enable);
InCommand CMovement::airstuck = InCommand("movement_air_stuck", VK_XBUTTON1, &movement_air_stuck_enable); // mouse4
InCommand CMovement::gs = InCommand("movement_gs", VK_XBUTTON2, &movement_gs_enable); // mouse5

VarBoolean debug_render_info_movement("debug_render_info_movement", "Movement information", false);
VarBoolean debug_render_info_movement_bhop("debug_render_info_bhop", "Bunnyhop information", false);

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

void CMovement::update_clientmove(float frametime, hl::usercmd_t *cmd)
{
	auto local = CEntityMgr::the().get_local_player();
	if (local && local->is_alive())
	{
		if (bunnyhop.is_active())
		{
			CMovementBunnyHop::the().update(frametime);
		}

		if (gs.is_active())
		{
			CMovementGroundStrafe::the().update(frametime);
		}

		if (movement_eb_enable.get_value())
		{
			CMovementEdgeBug::the().update(frametime);
		}

		if (airstuck.is_active())
		{
			CMovementAirStuck::the().update();
		}
	}

	if (debug.get_value())
	{
		if (debug_render_info_movement.get_value())
		{
			render_debug();
		}

		if (debug_render_info_movement_bhop.get_value())
		{
			CMovementBunnyHop::the().render_debug();
		}
	}

	feed_plot(frametime, cmd);
}

void CMovement::render_debug()
{
	CEngineFontRendering::the().render_debug("--- Movement ---");

	float gnd_dist = CLocalState::the().get_ground_dist();
	float edge_dist = CLocalState::the().get_edge_dist();
	float gnd_angle = CLocalState::the().get_ground_angle();
	bool is_surfing = CLocalState::the().is_surfing();
	float fall_vel = CLocalState::the().get_fall_velocity();
	float vel_2d = CLocalState::the().get_local_velocity_2d();

	auto pmove = *CMemoryHookMgr::the().pmove().get();

	CEngineFontRendering::the().render_debug("Ground distance: {:0.3f} units", gnd_dist);
	CEngineFontRendering::the().render_debug("Edge distance: {:0.3f} units", edge_dist);
	CEngineFontRendering::the().render_debug("Ground angle: {:0.1f} °", gnd_angle);
	CEngineFontRendering::the().render_debug("Fall velocity: {:0.3f} u/s", fall_vel);
	CEngineFontRendering::the().render_debug("Velocity 2D: {:0.3f} u/s", vel_2d);
	CEngineFontRendering::the().render_debug("Is surfing: {}", is_surfing);
	CEngineFontRendering::the().render_debug("Water level: {}", pmove->waterlevel);
	CEngineFontRendering::the().render_debug("Water type: {}", pmove->watertype);
	CEngineFontRendering::the().render_debug("Water jump time: {}", pmove->waterjumptime);
}

void CMovement::feed_plot(float frametime, hl::usercmd_t * cmd)
{
	CClientMovementPacketPlot::the().feed_by_name("On Ground", CColor(70, 130, 180, 230), MPVisualDataEntry(CLocalState::the().is_on_ground_safe()));
	CClientMovementPacketPlot::the().feed_by_name("Velocity", CColor(255, 69, 0, 230), MPVisualDataEntry(CLocalState::the().get_local_velocity_2d(), 500));
	CClientMovementPacketPlot::the().feed_by_name("Ground Dist", CColor(255, 69, 0, 230), MPVisualDataEntry(CLocalState::the().get_ground_dist(), 300));
	CClientMovementPacketPlot::the().feed_by_name("IN_JUMP", CColor(255, 192, 203, 230), MPVisualDataEntry((cmd->buttons & IN_JUMP)));
	CClientMovementPacketPlot::the().feed_by_name("IN_DUCK", CColor(218, 165, 32, 230), MPVisualDataEntry((cmd->buttons & IN_DUCK)));
	CClientMovementPacketPlot::the().feed_by_name("FPS", CColor(152, 251, 152, 230), MPVisualDataEntry(1.0f / frametime, 500));
}
