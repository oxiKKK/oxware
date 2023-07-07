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

#ifndef ENGINESPEEDCONTROL_H
#define ENGINESPEEDCONTROL_H
#pragma once

extern VarBoolean speed_control_enable;

extern VarInteger speed_control_type;
extern VarFloat speed_control_amount_engine;
extern VarInteger speed_control_amount_server;
extern VarFloat speed_control_amount_net;

extern VarBoolean speed_control_invert;
extern VarBoolean speed_control_negative;

class CEngineSpeedControl
{
public:
	DECL_BASIC_CLASS(CEngineSpeedControl);
	
public:
	void update();

	// call inside CL_CreateMove()
	void server_speedhack(uint8_t msec, bool override_previous = false);

	// see the function definition for more information :-)
	void classic_cs16_cheating_scene_speedhack(double new_speed);

	// modifies realtime global variable
	void realtime_speedhack(double new_speed);

private:
	void reset();
};

#endif // ENGINESPEEDCONTROL_H