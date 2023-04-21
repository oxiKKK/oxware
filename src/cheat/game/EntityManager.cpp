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

void CEntityMgr::update()
{
	OX_PROFILE_SCOPE("entity_mgr");
	
	if (!CGameUtil::the().is_fully_connected())
	{
		return;
	}

	auto cl = CMemoryHookMgr::the().cl().get();

	auto t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < cl->num_entities; i++)
	{
		auto ent = CMemoryHookMgr::the().cl_enginefuncs().get()->pfnGetEntityByIndex(i);
		if (!ent)
		{
			continue;
		}

		if (i > 0 && i < cl->maxclients)
		{
			m_known_players[i].update(ent);
		}
		else
		{
			m_known_entities[i].update(ent);
		}
	}

#if 0
	CConsole::the().info("Entity manager update: {} us for {} entities", 
						 std::chrono::duration<float, std::micro>(std::chrono::high_resolution_clock::now() - t1).count(), 
						 m_known_players.size() + m_known_entities.size());
#endif
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

void CEntityMgr::erase()
{
	CConsole::the().info("Erasing entity list...");
	
	m_known_players.clear();
	m_known_entities.clear();
}
