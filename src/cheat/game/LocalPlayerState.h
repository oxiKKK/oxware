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

#ifndef LOCALPLAYERSTATE_H
#define LOCALPLAYERSTATE_H
#pragma once


class CLocalPlayerState
{
public:
	DECL_BASIC_CLASS(CLocalPlayerState);

public:
	void update_clientmove(float frametime, hl::usercmd_t *cmd);

	hl::clientdata_t* get_current_frame_clientdata();

	// returns g_iPlayerFlags (in original cstrike client dll)
	// aka playermove flags (FL_ONGROUND etc - FL_* macros)
	int get_player_flags();

	// iuser3 cstrike flags
	bool player_can_shoot();
	bool player_freeze_time_over();
	bool player_in_bomb_zone();
	bool player_holding_shield();

	bool is_ducking();
	bool is_on_ground();

	// get local player velocity calculated by playermove code
	float get_local_velocity_2d();
	float get_local_velocity_3d();

	inline Vector get_origin() const { return m_pmove->origin; }
	inline float get_fall_velocity() const { return m_pmove->flFallVelocity; }
	inline float get_maxspeed() const { return m_pmove->maxspeed; }
	inline float get_ground_angle() const { return m_ground_angle; }
	inline float get_ground_dist() const { return m_ground_dist; }

private:
	hl::frame_t* m_current_frame = nullptr;

	int m_player_flags = 0;
	Vector m_moving_velocity;
	Vector m_origin;

	float m_ground_angle = 0.0f;
	float m_ground_dist = 0.0f;

	hl::playermove_t* m_pmove = nullptr;
};

#endif // LOCALPLAYERSTATE_H