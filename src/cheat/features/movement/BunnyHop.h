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

#ifndef BUNNYHOP_H
#define BUNNYHOP_H
#pragma once

extern VarBoolean movement_bhop_enable;
extern VarInteger movement_bhop_method;

enum ESimulJumpMethod
{
	SIMULJMP_Scroll,
	SIMULJMP_Command,
};

class CMovementBunnyHop
{
public:
	DECL_BASIC_CLASS(CMovementBunnyHop);

public:
	void update(float frametime, hl::usercmd_t *cmd);

private:
	void simulate_jump(float frametime, hl::usercmd_t* cmd, ESimulJumpMethod method);

	void exec_jump_cmd(bool do_the_jump);
	bool m_was_in_jump_lock;

	float m_simul_start_gnddist;
	void determine_random_gnddist_for_jump_simul(bool on_ground);

	float m_fall_velocity_factor;
};

#endif // BUNNYHOP_H