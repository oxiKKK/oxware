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

#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#pragma once

enum EBombInfoFlags
{
	BOMB_FLAG_DROPPED = 0, // if the bomb was dropped due to voluntary dropping or death/disconnect
	BOMB_FLAG_PLANTED = 1, // if the bomb has been planted will also trigger the round timer to hide will also show where the dropped bomb on the Terrorist team's radar.
};

class BombInfo
{
public:
	Vector m_origin;
	EBombInfoFlags m_flag;

	uint32_t m_update_timestamp;

	inline bool is_out_of_update_for(uint32_t ms) const
	{
		uint32_t now = GetTickCount();
		return now - m_update_timestamp > ms;
	}

	inline bool is_valid() const
	{
		return m_origin.IsZero() == false && !is_out_of_update_for(1000);
	}
};

extern VarBoolean debug_render_player_info;

class CEntityMgr
{
public:
	DECL_BASIC_CLASS(CEntityMgr);

public:
	void entity_state_update(hl::cl_entity_t* ent);
	void player_info_update(int index);

	void update_screen();

	void erase();
	void init();

	// getters
	std::optional<CGenericPlayer*> get_local_player();
	std::optional<CGenericPlayer*> get_player_by_id(int index);

	std::unordered_map<int, CGenericEntity> m_known_entities;
	std::unordered_map<int, CGenericPlayer> m_known_players;

	// bomb info
	void update_bomb_info(const Vector& origin, uint8_t flags);
	inline const BombInfo& get_bomb_info() const
	{
		return m_bomb_info;
	}

private:
	BombInfo m_bomb_info;
};

#endif // ENTITYMANAGER_H