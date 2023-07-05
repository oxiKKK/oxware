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

//
// Please, note that this code was based on the one from Tkz. Damn I wish I could do vector 
// and trigonometry math as AntiValve did... errghhh..
//

#include "precompiled.h"

VarBoolean movement_strafe_hack_allow_on_surf("movement_strafe_hack_allow_on_surf", "Strafes also when surfing", true);
VarBoolean movement_strafe_hack_limit_velocity("movement_strafe_hack_limit_velocity", "Enables velocity limit", false);
VarInteger movement_strafe_hack_limit_velocity_max("movement_strafe_hack_limit_velocity_max", "Maximum velocity to speed up to", 1000, 50, 3000);
VarBoolean movement_strafe_hack_dir_reset("movement_strafe_hack_dir_reset", "Always resets the direction to forward if no key is held", true);
VarFloat movement_strafe_hack_boost("movement_strafe_hack_boost", "How much boost you'll gain while strafing", 1.0f, 0.01f, 1.0f);

enum EDirection
{
	DIR_FORWARD,
	DIR_RIGHT,
	DIR_BACK,
	DIR_LEFT,
};

VarInteger movement_strafe_hack_dir("movement_strafe_hack_dir", "Direction of where to strafe to", DIR_FORWARD, DIR_FORWARD, DIR_LEFT);
VarBoolean movement_strafe_hack_dir_auto("movement_strafe_hack_dir_auto", "Determines the strafe direction automatically", false);

struct direction_data_t
{
	EDirection dir;
	float angle_rad;
	bool view_direction; // "forward" for true or "left" for false
};

static direction_data_t s_direction_data[5] =
{
	{ DIR_FORWARD,	0.0f,								true	}, // 0°
	{ DIR_RIGHT,	std::numbers::pi_v<float> / 2.0f,	false	}, // 90°
	{ DIR_BACK,		std::numbers::pi_v<float>,			true	}, // 180°
	{ DIR_LEFT,		-(std::numbers::pi_v<float> / 2.0f),false	}, // -90° or 270°
};

void CMovementStrafeHack::update()
{
	bool is_on_ladder = CLocalState::the().is_on_ladder();
	if (is_on_ladder)
	{
		return;
	}

	float vel_2d = CLocalState::the().get_local_velocity_2d();
	if (movement_strafe_hack_limit_velocity.get_value() && vel_2d > movement_strafe_hack_limit_velocity_max.get_value())
	{
		return;
	}

	// normally disable strafing on surf if not enabled
	bool is_surfing = CLocalState::the().is_surfing();
	if (is_surfing)
	{
		if (!movement_strafe_hack_allow_on_surf.get_value())
		{
			return;
		}
	}
	else
	{
		bool is_onground = CLocalState::the().is_on_ground_safe();
		if (is_onground)
		{
			return;
		}
	}
	
	//
	// decide direction
	//

	static direction_data_t* current_dir = &s_direction_data[DIR_FORWARD];

	if (!movement_strafe_hack_dir_reset.get_value())
	{
		current_dir = &s_direction_data[DIR_FORWARD];
	}

	if (movement_strafe_hack_dir_auto.get_value())
	{
		if (CClientMovementPacket::the().is_in(IN_FORWARD))
		{
			current_dir = &s_direction_data[DIR_FORWARD];
		}
		else if (CClientMovementPacket::the().is_in(IN_MOVERIGHT))
		{
			current_dir = &s_direction_data[DIR_RIGHT];
		}
		else if (CClientMovementPacket::the().is_in(IN_BACK))
		{
			current_dir = &s_direction_data[DIR_BACK];
		}
		else if (CClientMovementPacket::the().is_in(IN_MOVELEFT))
		{
			current_dir = &s_direction_data[DIR_LEFT];
		}
	}
	else
	{
		current_dir = &s_direction_data[movement_strafe_hack_dir.get_value()];
	}

	auto pmove = *CMemoryHookMgr::the().pmove().get();

	auto cmd = CClientMovementPacket::the().get_cmd();
	auto cl = CMemoryHookMgr::the().cl().get();

	auto vel_3d = pmove->velocity + Vector(1.0f, 1.0f, 0.0f);

	// [-pi, pi], cl->viewangles[YAW] in radians, but based on velocity and adapted to tangens range of values.
	float va_vel_tangens = atanf(vel_3d.y / vel_3d.x);
	float yaw_rad = CMath::the().deg2rad(cl->viewangles[YAW]); // yaw in radians

	float adif = va_vel_tangens - yaw_rad - current_dir->angle_rad;

	//
	// make transition between direction change smooth, so that we don't loose speed
	//

	// this formula will create half circle on the +y axis. this is so that we can transition smoothly from a direction to another
	adif = sin(adif);
	float half_a_circle = sqrt(1.0f - CMath::the().pow2(adif));
	adif = atanf(adif / half_a_circle);

	// this should be something around ~436 * the boost. I assume that this is based out of the maximal possible vanilla maxspeed, 
	// which is 260 with scout * the multiplier (that gets us to maxspeed), i.e. 310 u/s. When you combine this together with X&Y
	// velocity vectors, you get ~438. (sqrt(310*310+310*310), i.e. sqrt((310*310)*2) or sqrt(2)*310)
	float sm_factor = ((260.0f * BUNNYJUMP_MAX_SPEED_FACTOR) * std::numbers::sqrt2_v<float>);
	cmd->sidemove = sm_factor * (adif > 0 ? 1 : -1);

	float angle;
	float osin, ocos, nsin, ncos;

	angle = yaw_rad;
	osin = sin(angle);
	ocos = cos(angle);

	angle = yaw_rad * 2.0f - va_vel_tangens + current_dir->angle_rad;
	nsin = sin(angle);
	ncos = cos(angle);

	cmd->forwardmove = (cmd->sidemove) * (osin * ncos - ocos * nsin);
	cmd->sidemove *= (osin * nsin + ocos * ncos);

	// add a little bit of forward velocity when not doing on a steep turn
	float fs = 0;
	if (atanf(9999 / vel_2d) >= abs(adif))
	{
		Vector view_dir = current_dir->view_direction ? pmove->forward : pmove->right;

		view_dir.z = 0;
		view_dir = view_dir.Normalize();

		float vel = CMath::the().pow2(view_dir[0] * vel_3d[0]) + CMath::the().pow2(view_dir[1] * vel_3d[1]);

		// this will generate a smooth curve towards the +x
		float fm_factor = 69.0f * 100000;
		fs = sqrt(fm_factor / vel);
	}

	cmd->forwardmove += fs;

	cmd->forwardmove *= movement_strafe_hack_boost.get_value();

	//
	// swap moves depending on the direction
	//

	float sdmw = cmd->sidemove;
	float fdmw = cmd->forwardmove;

	switch (current_dir->dir)
	{
		case DIR_FORWARD: // forward
			cmd->forwardmove = fdmw;
			cmd->sidemove = sdmw;
			break;
		case DIR_RIGHT: // right
			cmd->forwardmove = -sdmw;
			cmd->sidemove = fdmw;
			break;
		case DIR_BACK: // back
			cmd->forwardmove = -fdmw;
			cmd->sidemove = -sdmw;
			break;
		case DIR_LEFT: // left
			cmd->forwardmove = sdmw;
			cmd->sidemove = -fdmw;
			break;
	}
}

void CMovementStrafeHack::render_debug()
{
	CEngineFontRendering::the().render_debug("--- Strafe ---");

	auto cl = CMemoryHookMgr::the().cl().get();
	auto pmove = *CMemoryHookMgr::the().pmove().get();
	float va_speed = atan2(pmove->velocity.y, pmove->velocity.x);
	CEngineFontRendering::the().render_debug("va_speed: {} / {}", va_speed, CMath::the().deg2rad(cl->viewangles[YAW]));

	auto vel_3d = pmove->velocity + Vector(1.0f, 1.0f, 0.0f);
	auto vel_2d = CLocalState::the().get_local_velocity_2d();

	// [-pi, pi], cl->viewangles[YAW] in radians, but based on velocity and adapted to tangens range of values.
	float va_vel_tangens = atanf(vel_3d.y / vel_3d.x);
	float yaw_rad = CMath::the().deg2rad(cl->viewangles[YAW]); // yaw in radians

	float adif = va_vel_tangens - yaw_rad - s_direction_data[DIR_FORWARD].angle_rad;
	CEngineFontRendering::the().render_debug("adif: {}", CMath::the().rad2deg(adif));
	CEngineFontRendering::the().render_debug("yaw_r: {}", CMath::the().rad2deg(yaw_rad));
	CEngineFontRendering::the().render_debug("va_based_on_vel: {}", CMath::the().rad2deg(va_vel_tangens));
	CEngineFontRendering::the().render_debug("max forwardmove: {}", (pmove->maxspeed * BUNNYJUMP_MAX_SPEED_FACTOR) * std::numbers::sqrt2);
}

