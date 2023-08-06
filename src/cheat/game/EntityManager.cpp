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

VarBoolean debug_render_player_info("debug_render_player_info", "Displays information about players", false);

void CEntityMgr::entity_state_update(hl::cl_entity_t* ent)
{
	if (ent->player)
	{
		m_known_players[ent->index].update(ent);
	}
	else
	{
		m_known_entities[ent->index].update(ent);
	}
}

void CEntityMgr::player_info_update(int index)
{
	// see if the player is actually connected on a server
	auto cl = CMemoryHookMgr::the().cl().get();
	hl::player_info_t* player_info = &cl->players[index];

	if (!player_info->name[0])
	{
		return;
	}

	// can create a "new" player here with only player info present
	// NOTE: this index is 0-based and keys inside known_players are 1-based...!
	auto& player = m_known_players[index + 1];

	player.update_player_info(index, player_info);
}

void CEntityMgr::update_screen()
{
	if (!debug_render_player_info.get_value())
	{
		return;
	}

	CEngineFontRendering::the().push_render_side(CEngineFontRendering::RIGHT);

	CEngineFontRendering::the().render_debug("--- Player info ---");

	for (const auto& [id, p] : m_known_players)
	{
		if (!p.is_valid())
		{
			CEngineFontRendering::the().render_debug("{:<2}: invalid", id);
			continue;
		}

		CEngineFontRendering::the().render_debug("{:<2}: {}", id, p.get_playerinfo()->name);
	}

	CEngineFontRendering::the().pop_render_side();
}

void CEntityMgr::init()
{
	CConsole::the().info("Initializing entity and reserving:");

	auto cl = CMemoryHookMgr::the().cl().get();

	m_known_players.reserve(cl->maxclients);
	CConsole::the().info("  {} for players.", m_known_players.bucket_count());

	m_known_entities.reserve(cl->max_edicts - m_known_players.bucket_count());
	CConsole::the().info("  {} for entities.", m_known_entities.bucket_count());
}

std::optional<CGenericPlayer*> CEntityMgr::get_local_player()
{
	int local_index = CMemoryHookMgr::the().cl().get()->playernum + 1;

	try
	{
		return std::make_optional(&m_known_players.at(local_index));
	}
	catch (...)
	{
		CConsole::the().derror("Non-existent local player index: {}", local_index);
		return std::nullopt;
	}
}

std::optional<CGenericPlayer*> CEntityMgr::get_player_by_id(int index)
{
	try
	{
		return std::make_optional(&m_known_players.at(index));
	}
	catch (...)
	{
		CConsole::the().derror("Non-existent player index: {}", index);
		return std::nullopt;
	}
}


void CEntityMgr::update_bomb_info(const Vector& origin, uint8_t flags)
{
	m_bomb_info.m_origin = origin;
	m_bomb_info.m_flag = (EBombInfoFlags)flags;
	m_bomb_info.m_update_timestamp = GetTickCount();
}

void CEntityMgr::erase()
{
	CConsole::the().info("Erasing entity list...");
	
	m_known_players.clear();
	m_known_entities.clear();
	m_bomb_info = {};
}
