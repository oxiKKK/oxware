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

#ifndef PLAYERLIST_H
#define PLAYERLIST_H
#pragma once

struct cached_player_info_t
{
	cached_player_info_t()
	{}

	BaseTexture m_avatar_picture;

	SteamID_t m_steam_id;
};

class CUIPlayerList
{
public:
	DECL_BASIC_CLASS(CUIPlayerList);

public:
	void render_ui();

private:
	void initialize();

	void render_players();
	void render_selected_player_info();

	void render_player_info(const cached_player_info_t& cached_player);
	void render_player_avatar(const cached_player_info_t& cached_player, float avatar_size);

	void manage_timers();
	
	int m_selected_plr_idx;

	// cached player info
	std::array<cached_player_info_t, MAX_CLIENTS> m_cached_players;
	void create_cached_player_info(int id, const CGenericPlayer& player);
	void update_avatars(int id, cached_player_info_t& cached_player);
	bool m_update_player_cache = false;
	uint32_t m_last_cache_update_time = 0;

	// avatar pictures
	bool m_precache_avatar_picture = false;
	uint32_t m_last_avatar_precache_time = 0;

	BaseTexture m_unknown_profile_picture;

	std::string get_avatar_texture_playerid(int id);
};

#endif // PLAYERLIST_H