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

#ifndef STRAFEHELPER_H
#define STRAFEHELPER_H
#pragma once

extern VarFloat movement_strafe_helper_accumulation;
extern VarBoolean movement_strafe_helper_accumulation_on_ground;

extern VarBoolean movement_strafe_helper_strafe_with_mouse;
extern VarBoolean movement_strafe_helper_strafe_dir_auto;
extern VarInteger movement_strafe_helper_strafe_dir;

class CMovementStrafeHelper
{
public:
	DECL_BASIC_CLASS(CMovementStrafeHelper);

public:
	void update();

	void render_debug();

private:
	void correction();

	int deduce_direction(hl::usercmd_t* cmd);

private:
	// 0 == left, 1 == right, 2 == forward
	int m_mouse_direction = 0;
};

#endif // STRAFEHELPER_H