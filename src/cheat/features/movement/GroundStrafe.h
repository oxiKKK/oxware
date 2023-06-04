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

#ifndef GROUNDSTRAFE_H
#define GROUNDSTRAFE_H
#pragma once

extern VarInteger movement_gs_mode;
extern VarBoolean movement_gs_mode_noslowdown;
extern VarInteger movement_gs_mode_noslowdown_method;
extern VarInteger movement_gs_mode_noslowdown_factor;
extern VarInteger movement_gs_legit_ground_dist_min;
extern VarInteger movement_gs_legit_ground_dist_max;
extern VarInteger movement_gs_legit_efficiency;
extern VarInteger movement_gs_legit_scroll_density;

enum EGSEfficiency
{
	GSEFF_Helper,
	GSEFF_Normal,
	GSEFF_RandomFOGBased,
};

enum EGSMode
{
	GSMODE_Legit,
	GSMODE_Rage,
};

enum EGSNoSlowDownMethod
{
	GSNSDN_ServerSpeed,
	GSNSDN_EngineSpeed,
};

class CMovementGroundStrafe
{
public:
	DECL_BASIC_CLASS(CMovementGroundStrafe);

public:
	void update(float frametime);

private:
	void legit_gs(float frametime);
	void rage_gs(float frametime);

	void simulate_duck(float frametime);

	float m_simul_start_gnddist;
	void determine_random_gnddist_for_duck_simul(bool on_ground);

	bool duck_helper(float frametime);

	bool perfect_condition_for_duck();

	bool randomize_duck_pattern(bool is_onground);

	bool m_remained_in_duck;

	bool predicted_nextframe_on_ground(float frametime);

	void nsdn_speedhack();
};

#endif // GROUNDSTRAFE_H