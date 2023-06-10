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

#ifndef MOVEMENT_H
#define MOVEMENT_H
#pragma once

extern VarBoolean movement_bhop_enable;
extern VarBoolean movement_air_stuck_enable;
extern VarBoolean movement_gs_enable;
extern VarBoolean movement_eb_enable;
extern VarBoolean movement_strafe_hack_enable;
extern VarBoolean movement_strafe_helper_enable;
extern VarBoolean movement_fastrun_enable;

extern VarBoolean debug_render_info_movement;
extern VarBoolean debug_render_info_movement_bhop;
extern VarBoolean debug_render_info_movement_strafe;
extern VarBoolean debug_render_info_movement_strafe_helper;

class CMovement
{
public:
	DECL_BASIC_CLASS(CMovement);

public:
	void update_clientmove(float frametime, hl::usercmd_t *cmd);

private:
	static InCommand bunnyhop;			// CMovementBunnyHop
	static InCommand airstuck;			// CMovementAirStuck
	static InCommand gs;				// CMovementGroundStrafe
	static InCommand eb;				// CMovementEdgeBug
	static InCommand strafe;			// CMovementStrafeHack
	static InCommand strafe_helper;		// CMovementStrafeHelper
	static InCommand fastrun;			// CMovementFastRun

	void render_debug(hl::usercmd_t* cmd);

	void feed_plot(float frametime, hl::usercmd_t *cmd);
};

#endif // MOVEMENT_H