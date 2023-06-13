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

extern VarInteger movement_bhop_mode;
extern VarBoolean movement_bhop_jump_on_ladder;
extern VarBoolean movement_bhop_jump_in_water;
extern VarInteger movement_bhop_repeat_ms;
extern VarBoolean movement_bhop_mode_noslowdown;
extern VarInteger movement_bhop_mode_noslowdown_method;
extern VarInteger movement_bhop_mode_noslowdown_factor;
extern VarInteger movement_bhop_legit_ground_dist_min;
extern VarInteger movement_bhop_legit_ground_dist_max;
extern VarInteger movement_bhop_legit_efficiency;
extern VarInteger movement_bhop_legit_scroll_density;

enum EBhopEfficiency
{
	BHOPEFF_Helper,
	BHOPEFF_Normal,
	BHOPEFF_RandomFOGBased,
};

enum EBhopMode
{
	BHOPMODE_Legit,
	BHOPMODE_Rage,
};

enum EBhopNoSlowDownMethod
{
	BHOPNSDN_ServerSpeed,
	BHOPNSDN_EngineSpeed,
};

class CMovementBunnyHop
{
public:
	DECL_BASIC_CLASS(CMovementBunnyHop);

public:
	void update(float frametime);

	void render_debug();

private:
	void legit_bhop(float frametime);
	void rage_bhop(float frametime);

	void simulate_jump(float frametime);

	float m_simul_start_gnddist;
	void determine_random_gnddist_for_jump_simul(bool on_ground);

	bool hop_helper(float frametime);

	bool perfect_condition_for_jump();

	bool randomize_jump_pattern(bool is_onground);

	bool m_remained_in_jump;

	bool predicted_nextframe_on_ground(float frametime);

	int m_fog_counter = 0;

	void nsdn_speedhack();

	int m_jump_timer_ms = 0;

	bool timer_allows_jump();
	void reset_jump_time();
};

#endif // BUNNYHOP_H