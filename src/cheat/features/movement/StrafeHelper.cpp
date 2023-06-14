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

VarFloat movement_strafe_helper_accumulation("movement_strafe_helper_accumulation", "Adds YAW angles to your strafes making them more efficient", 1.0f, 0.01f, 3.0f);
VarBoolean movement_strafe_helper_accumulation_on_ground("movement_strafe_helper_accumulation_on_ground", "Enables strafe accumulation when on ground", false);

enum EStrafeDir
{
	RIGHT, LEFT, FORWARD
};

enum EMoveDirection
{
	MOVE_FORWARD, // ^
	MOVE_RIGHT,   // >
	MOVE_BACK,    // v
	MOVE_LEFT,    // <
};

// strafing with mouse
VarBoolean movement_strafe_helper_strafe_with_mouse("movement_strafe_helper_strafe_with_mouse", "Performs moves with mouse. Works only forward/backward", false);
VarBoolean movement_strafe_helper_strafe_dir_auto("movement_strafe_helper_strafe_dir_auto", "Automatically deduces strafe direction based on keys held", false);
VarInteger movement_strafe_helper_strafe_dir("movement_strafe_helper_strafe_dir", "Strafe direction", MOVE_FORWARD, MOVE_FORWARD, MOVE_LEFT);

struct move_direction_t
{
	bool move_forward, move_right, move_back, move_left;
	float sidemove, forwardmove;
};

// the "forward" strafe is always no strafe, so we zero everything
static const std::array<std::array<move_direction_t, 2>, 4> g_move_dirs =
{{
	// MOVE_FORWARD
	// forward and back vector is FORWARD & BACK
	// side vectors are LEFT & RIGHT
	{
		move_direction_t( // STRAFE RIGHT
			false, true, false, false,
			250, 0
		),
		move_direction_t( // STRAFE LEFT
			false, false, false, true,
			-250, 0
		),
	},
	// MOVE_RIGHT
	// forward and back vector is RIGHT & LEFT
	// side vectors are FORWARD & BACK
	{
		move_direction_t( // STRAFE RIGHT
			false, false, true, false,
			0, -250
		),
		move_direction_t( // STRAFE LEFT
			true, false, false, false,
			0, 250
		),
	},
	// MOVE_BACK
	// forward and back vector is BACK & FORWARD
	// side vectors are RIGHT & LEFT
	{
		move_direction_t( // STRAFE RIGHT
			false, true, false, true,
			-250, 0
		),
		move_direction_t( // STRAFE LEFT
			false, true, false, false,
			250, 0
		),
	},
	// MOVE_LEFT
	// forward and back vector is LEFT & RIGHT
	// side vectors are BACK & FORWARD
	{
		move_direction_t( // STRAFE RIGHT
			true, false, false, false,
			0, 250
		),
		move_direction_t( // STRAFE LEFT
			false, false, true, false,
			0, -250
		),
	},

}};

void CMovementStrafeHelper::update()
{
	bool is_onladder = CLocalState::the().is_on_ladder();
	if (is_onladder)
	{
		return;
	}

	auto cmd = CClientMovementPacket::the().get_cmd();
	auto cl = CMemoryHookMgr::the().cl().get();

	float x = CLocalState::the().get_viewangle_delta().x;
	if (x > 0)
	{
		m_mouse_direction = RIGHT;
	}
	else if (x < 0)
	{
		m_mouse_direction = LEFT;
	}
	else // no mouse movement
	{
		m_mouse_direction = FORWARD;
	}

	bool is_onground = CLocalState::the().is_on_ground_safe();
	if (is_onground)
	{
		if (movement_strafe_helper_accumulation_on_ground.get_value())
		{
			correction();
		}
		return;
	}

	if (movement_strafe_helper_strafe_with_mouse.get_value())
	{
		EMoveDirection wanted_dir;
		
		if (!movement_strafe_helper_strafe_dir_auto.get_value())
		{
			wanted_dir = (EMoveDirection)movement_strafe_helper_strafe_dir.get_value();
		}
		else
		{
			wanted_dir = (EMoveDirection)deduce_direction(cmd);
		}

		if (m_mouse_direction == FORWARD)
		{
			// no strafe direction always resets everything
			cmd->forwardmove = cmd->sidemove = 0;
			CClientMovementPacket::the().set_button_bit(IN_FORWARD, false);
			CClientMovementPacket::the().set_button_bit(IN_MOVERIGHT, false);
			CClientMovementPacket::the().set_button_bit(IN_BACK, false);
			CClientMovementPacket::the().set_button_bit(IN_MOVELEFT, false);
		}
		else
		{
			const auto& move_dir = g_move_dirs[wanted_dir];
			const auto& strafe_dir = move_dir[m_mouse_direction];

			// set stuff based on direction
			cmd->forwardmove = strafe_dir.forwardmove;
			cmd->sidemove = strafe_dir.sidemove;
			CClientMovementPacket::the().set_button_bit(IN_FORWARD, strafe_dir.move_forward);
			CClientMovementPacket::the().set_button_bit(IN_MOVERIGHT, strafe_dir.move_right);
			CClientMovementPacket::the().set_button_bit(IN_BACK, strafe_dir.move_back);
			CClientMovementPacket::the().set_button_bit(IN_MOVELEFT, strafe_dir.move_left);
		}
	}

	correction();
}

void CMovementStrafeHelper::render_debug()
{
	CEngineFontRendering::the().render_debug("--- Strafe Helper ---");

	CEngineFontRendering::the().render_debug("Mouse direction: {}", (int)m_mouse_direction);
}

void CMovementStrafeHelper::correction()
{
	auto cmd = CClientMovementPacket::the().get_cmd();
	auto cl = CMemoryHookMgr::the().cl().get();

	float acc = movement_strafe_helper_accumulation.get_value();
	if (acc && (m_mouse_direction == LEFT || m_mouse_direction == RIGHT))
	{
		cl->viewangles[YAW] += (m_mouse_direction == LEFT) ? acc : -acc;
		cmd->viewangles[YAW] += (m_mouse_direction == LEFT) ? acc : -acc;
	}
}

int CMovementStrafeHelper::deduce_direction(hl::usercmd_t* cmd)
{
	if (cmd->buttons & IN_FORWARD)
	{
		return MOVE_FORWARD;
	}
	else if (cmd->buttons & IN_MOVERIGHT)
	{
		return MOVE_RIGHT;
	}
	else if (cmd->buttons & IN_BACK)
	{
		return MOVE_BACK;
	}
	else if (cmd->buttons & IN_MOVELEFT)
	{
		return MOVE_LEFT;
	}

	return MOVE_FORWARD;
}

