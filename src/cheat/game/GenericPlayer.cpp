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

void CGenericPlayer::update(hl::cl_entity_t* ent)
{
	CGenericEntity::update(ent);
}

void CGenericPlayer::update_player_info(int index, hl::player_info_t* pinfo)
{
	m_playerinfo = pinfo;

	// now the extra info
	if (CoXWARE::the().get_build_number() < 8684)
	{
		m_extra_playerinfo = (hl::extra_player_info_t*)&(CMemoryHookMgr::the().g_PlayerExtraInfoOld().get()[index]);
	}
	else
	{
		m_extra_playerinfo = (hl::extra_player_info_t*)&(CMemoryHookMgr::the().g_PlayerExtraInfo().get()[index]);
	}
}

bool CGenericPlayer::is_alive() const
{
	if (!m_extra_playerinfo)
		return false;

	return m_extra_playerinfo->dead == false;
}

CColor CGenericPlayer::get_color_based_on_team() const
{
	if (CGameUtil::the().is_local_player(m_ent->index))
	{
		return CColor(0, 252, 230, 230);;
	}

	switch (get_team())
	{
		default:
		case hl::UNASSIGNED: 
			return CColor(0, 230, 230, 230);
		case hl::TERRORIST: 
			return CColor(230, 0, 0, 230);
		case hl::CT: 
			return CColor(0, 50, 230, 230);
		case hl::SPECTATOR:
			return CColor(230, 230, 230, 230);
	}
}

bool CGenericPlayer::is_local_player() const
{
	return m_ent->index == CMemoryHookMgr::the().cl().get()->playernum + 1;
}
