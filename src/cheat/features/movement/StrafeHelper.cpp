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

VarBoolean movement_strafe_helper_strafe_with_mouse("movement_strafe_helper_strafe_with_mouse", "Performs moves with mouse. Works only forward/backward", false);
VarFloat movement_strafe_helper_accumulation("movement_strafe_helper_accumulation", "Adds YAW angles to your strafes making them more efficient", 1.0f, 0.01f, 3.0f);
VarBoolean movement_strafe_helper_accumulation_on_ground("movement_strafe_helper_accumulation_on_ground", "Enables strafe accumulation when on ground", false);

enum
{
	LEFT, RIGHT, FORWARD
};

void CMovementStrafeHelper::update()
{
	if (!mouse_x_ptr)
	{
		return; // we have to have this
	}

	bool is_onladder = CLocalState::the().is_on_ladder();
	if (is_onladder)
	{
		return;
	}

	auto cmd = CClientMovementPacket::the().get_cmd();
	auto cl = CMemoryHookMgr::the().cl().get();

	float x = *mouse_x_ptr;
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
		cmd->forwardmove = 0;
		CClientMovementPacket::the().set_button_bit(IN_FORWARD, false);
		CClientMovementPacket::the().set_button_bit(IN_BACK, false);

		switch (m_mouse_direction)
		{
			case RIGHT:
			{
				CClientMovementPacket::the().set_button_bit(IN_MOVERIGHT, true);
				CClientMovementPacket::the().set_button_bit(IN_MOVELEFT, false);
				cmd->sidemove = 250;
				break;
			}
			case LEFT:
			{
				CClientMovementPacket::the().set_button_bit(IN_MOVELEFT, true);
				CClientMovementPacket::the().set_button_bit(IN_MOVERIGHT, false);
				cmd->sidemove = -250;
				break;
			}
			case FORWARD:
			{
				CClientMovementPacket::the().set_button_bit(IN_MOVELEFT, false);
				CClientMovementPacket::the().set_button_bit(IN_MOVERIGHT, false);
				break;
			}
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

