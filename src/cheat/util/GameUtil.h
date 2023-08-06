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

#define MAXIMAL_DESIRED_EDGE_DIST 36.0f

// Spectator Movement modes (stored in pev->iuser1, so the physics code can get at them)
#define OBS_NONE				0 // not spectator
#define OBS_CHASE_LOCKED		1
#define OBS_CHASE_FREE			2 // third-person on the player
#define OBS_ROAMING				3 // free look
#define OBS_IN_EYE				4 // first person
#define OBS_MAP_FREE			5
#define OBS_MAP_CHASE			6

enum EAmbientSoundSpawnFlags
{
	AMBIENT_DEFAULT, 
	AMBIENT_EVERYWHERE, 
	AMBIENT_SMALL_RADIUS, 
	AMBIENT_MEDIUM_RADIUS, 
	AMBIENT_LARGE_RADIUS, 
};

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

	// -3 -> player is specator/unassigned
	// -2 -> local or player is specator/unassigned
	// -1 -> could not get local or player
	// 0  -> isn't
	// 1  -> is
	int is_player_on_enemy_team(int index);

	// accepts 1-based index
	bool is_local_player(int index);

	// returns current appid based on the -game launch parameter.
	unsigned int get_current_app_id();

	// returns __DATE__ strings from Host_Version_f()
	const std::string& get_engine_compile_date() { locate_engine_compile_timestamp(); return m_engine_compile_date; }

	// copy of the original engine's function 'build_num()'
	int get_build_number();

	bool is_spectator();

	hl::SCREENINFO get_engine_screen_info();

	std::string parse_viewmodel_name(hl::model_t* model);

	float compute_ground_angle_for_origin(const Vector& origin, float trace_distance = 4096.0f);
	float compute_distance_to_ground(const Vector& origin, float trace_distance = 4096.0f);
	float compute_edge_distance(const Vector& origin, float edge_trace_distance = MAXIMAL_DESIRED_EDGE_DIST);

	// replica of CL_RecordHUDCommand
	void record_hud_command(const char* cmdname);

	// equivalent to Cmd_TokenizeString()
	std::vector<std::string> tokenize_goldsrc_command(const char* text);

	Vector2D get_window_pos();

private:
	void locate_engine_compile_timestamp();

	std::string m_engine_compile_date;
};

#endif // GAMEUTIL_H