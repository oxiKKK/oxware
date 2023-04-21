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

class CEntityMgr
{
public:
	DECL_BASIC_CLASS(CEntityMgr);

public:
	void update();

	void erase();
	void init();

	CGenericPlayer* get_local_player()
	{
		int local_index = CMemoryHookMgr::the().cl().get()->playernum + 1;
		if (local_index < 0 || local_index > m_known_players.size())
		{
			assert(0 && "Trying to get local player with invalid player index!");
			return nullptr;
		}

		return &m_known_players[local_index];
	}

	std::unordered_map<int, CGenericEnt> m_known_entities;
	std::unordered_map<int, CGenericPlayer> m_known_players;
};

#endif // ENTITYMANAGER_H