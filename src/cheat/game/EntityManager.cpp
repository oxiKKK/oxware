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
	auto& player = m_known_players[index];

	if (player.is_alive())
	{
		m_known_players[index].update_player_info(index);
	}
}

void CEntityMgr::update_screen()
{
	return; // TODO: Keep?
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
