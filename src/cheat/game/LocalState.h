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

enum ESpectatingMode
{
	SPECT_NOT_SPECTATING,
	SPECT_THIRD_PERSON_LOCKED,
	SPECT_THIRD_PERSON_FREE,
	SPECT_FREE_LOOK,
	SPECT_FIRST_PERSON,
	SPECT_MAP_FREE,
	SPECT_MAP_CHASE,
};

class CLocalState
{
public:
	DECL_BASIC_CLASS(CLocalState);

public:
	void update_pre_clientmove(float frametime, hl::usercmd_t *cmd);
	void update_clientmove(float frametime, hl::usercmd_t *cmd);

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

	float get_fov();

	// sometimes, when playing on a local server with latency, the server playermove code doesn't set
	// the FL_ONGROUND flag I assume? so we better also check for ground distance.
	bool is_on_ground_safe();

	// get local player velocity calculated by playermove code
	float get_local_velocity_2d();
	float get_local_velocity_3d();
	Vector get_local_velocity_vec();

	hl::clientdata_t* get_current_frame_clientdata();

	Vector get_origin();
	float get_fall_velocity();
	float get_fall_velocity_abs();
	float get_maxspeed();
	float get_gravity();
	float get_ground_angle();
	float get_ground_dist();
	float get_edge_dist();

	bool is_surfing();
	bool is_on_ladder();

	int get_movetype();
	int get_waterlevel();

	EPlayerHull get_current_hull();
	// on some servers the hull is being set to 2 and remainsant, idk why.
	// resolve the hull for ourselfs, otherwise tracing code will fail
	EPlayerHull get_current_hull_tracing();

	Vector2D get_viewangle_delta();

	hl::frame_t* get_current_frame();

	// only set when local player object is valid and alive.
	CGenericPlayer* local_player();

	int get_spectating_player();
	ESpectatingMode get_spectating_mode();

	bool is_in_messagemode();

	// true when specating and when in the spectator mode where 3d overview is visible
	bool is_in_spectator_mapview();

	double get_engine_frametime();
	hl::CSteamID get_local_steamid();

private:
	hl::frame_t* m_current_frame = nullptr;

	int m_player_flags = 0;
	Vector m_moving_velocity;
	Vector m_origin;

	float m_ground_angle = 0.0f;
	float m_ground_dist = 0.0f;
	float m_edge_dist = 0.0f;

	hl::playermove_t* m_pmove = nullptr;

	EPlayerHull m_tracing_hull = HULL_STANDING;

	bool m_is_surfing = false;

	Vector2D m_vieangle_delta;
	Vector m_last_viewangles; // used to calc the delta

	// set if alive, otherwise nullptr
	CGenericPlayer* m_local_player;

	bool m_in_messagemode = false;

	double m_engine_frametime = 0.0;

	// the m_nSteamID field inside player_info_t doesn't actually get set for local player.. heh
	hl::CSteamID m_local_steamid;
};

#endif // LOCALPLAYERSTATE_H