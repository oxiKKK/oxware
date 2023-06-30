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

VarFloat movement_eb_intensity("movement_eb_intensity", "How intense the edgebug will be", 0.1f, 0.1f, 1.0f);
VarBoolean movement_eb_enable_on_ramps("movement_eb_enable_on_ramps", "Whenever to enable edgebug on ramped surfaces or not", false);
VarInteger movement_eb_min_fall_velocity("movement_eb_min_fall_velocity", "Fall velocity at which the EdgeBugging will start speeding", 200, 100, 500);
VarInteger movement_eb_edge_dist("movement_eb_edge_dist", "Edge distance at which the EdgeBugging will start speeding", 10, 5, 25);
VarInteger movement_eb_ground_dist("movement_eb_ground_dist", "Ground distance at which the EdgeBugging will start speeding", 4, 1, 20);

void CMovementEdgeBug::update(float frametime)
{
	bool is_surfing = CLocalState::the().is_surfing();
	if (is_surfing)
	{
		return;
	}

	bool is_on_slope = CLocalState::the().get_ground_angle() > 0.0f;
	if (is_on_slope && !movement_eb_enable_on_ramps.get_value())
	{
		return;
	}

	float fall_vel = CLocalState::the().get_fall_velocity();
	if (fall_vel < 0.0f || fall_vel < movement_eb_min_fall_velocity.get_value())
	{
		// don't execute while falling upwards.
		return;
	}

	float edge_dist = CLocalState::the().get_edge_dist();
	if (edge_dist > MAXIMAL_DESIRED_EDGE_DIST || edge_dist < movement_eb_edge_dist.get_value())
	{
		// don't exceed the tracing limig at which we compute the acutal edge distance.
		return;
	}
	
	float gnd_dist = CLocalState::the().get_ground_dist();
	if (gnd_dist > movement_eb_ground_dist.get_value())
	{
		return;
	}

	//
	// alright, we have passed everything, let's speed up.
	//

	float intensity = movement_eb_intensity.get_value();
	CEngineSpeedControl::the().classic_cs16_cheating_scene_speedhack(35.0f * intensity);
}

