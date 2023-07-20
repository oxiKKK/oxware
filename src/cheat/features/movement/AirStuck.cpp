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

VarFloat movement_air_stuck_intensity("movement_air_stuck_intensity", "Determines how \"froze\" you will be", 0.0f, 0.0f, 3.0f);
VarInteger movement_air_stuck_type("movement_air_stuck_type", "Type of the speedhack that will be used", 0, 0, 1);

void CMovementAirStuck::update()
{
	float value = movement_air_stuck_intensity.get_value();

	// override previous value.
	switch (movement_air_stuck_type.get_value())
	{
		case 0: // server
		{
			float ft = CLocalState::the().get_engine_frametime();

			// make it somewhat fps-based so that this doesn't get affected when changing fps
			// since this is capped to only integer resolution, it just will not work properly...
			value *= (1.0f + (70.0f * ft));
			CClientMovementPacket::the().svside_movement_speed_factor((int)(std::floor(value)), true);
			break;
		}
		case 1: // engine
		{
			CEngineSpeedControl::the().classic_cs16_cheating_scene_speedhack(value);
			break;
		}
	}
}
