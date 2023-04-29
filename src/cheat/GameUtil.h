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

#ifndef GAMEUTIL_H
#define GAMEUTIL_H
#pragma once

class CGameUtil
{
public:
	DECL_BASIC_CLASS(CGameUtil);

public:
	// returns true if we're active and if we're singleplayer/multiplayer
	bool is_fully_connected();

	// true if in singleplayer
	bool is_sp();

	// stop moving, shooting, etc.
	void reset_all_in_states();

	// convert world 3d coordinates to 2d screen coordinates
	// retuns true if coordinates are clipped within main viewsport
	bool world_to_screen(Vector world, Vector2D& screen);

	// an equivalent to CL_IsPlayerIndex().
	bool is_player_index(int index);

	bool is_player_on_enemy_team(int index);

	// accepts 1-based index
	bool is_local_player(int index);

	// returns current appid based on the -game launch parameter.
	unsigned int get_current_app_id();

	// returns __DATE__ strings from Host_Version_f()
	const std::string& get_engine_compile_date() { locate_engine_compile_timestamp(); return m_engine_compile_date; }

	// copy of the original engine's function 'build_num()'
	int get_build_number();

	bool is_spectator();

	double get_engine_frametime();

	hl::SCREENINFO get_engine_screen_info();

	std::string parse_viewmodel_name(hl::model_t* model);

	// get local player velocity calculated by playermove code
	float get_local_velocity_2d();
	float get_local_velocity_3d();

	// check if nullptr!
	hl::CBasePlayerWeapon* get_current_weapon();

	int get_weapon_accuracy_flags(int weapon_id, int weapon_flags);

	hl::clientdata_t* get_current_frame_clientdata();

	// returns g_iPlayerFlags (in original cstrike client dll)
	// aka playermove flags (FL_ONGROUND etc - FL_* macros)
	int get_player_flags();

	void render_circle_opengl(float cx, float cy, float radius, int num_segments, float width, bool blend, int r, int g, int b, int a);
	void render_line_opengl(const Vector2D& from, const Vector2D& to, float width, bool blend, int r, int g, int b, int a);

	// iuser3 cstrike flags
	bool player_can_shoot();
	bool player_freeze_time_over();
	bool player_in_bomb_zone();
	bool player_holding_shield();

private:
	void locate_engine_compile_timestamp();

	std::string m_engine_compile_date;
};

#endif // GAMEUTIL_H