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
	m_playerinfo_index = index;

	// now the extra info
	if (COxWare::the().get_build_number() < 8684)
	{
		m_extra_playerinfo = (hl::extra_player_info_t*)&(CMemoryHookMgr::the().g_PlayerExtraInfoOld().get()[index + 1]);
	}
	else
	{
		m_extra_playerinfo = (hl::extra_player_info_t*)&(CMemoryHookMgr::the().g_PlayerExtraInfo().get()[index + 1]);
	}
}

bool CGenericPlayer::is_alive() const
{
	if (!m_extra_playerinfo)
	{
		return false;
	}

	return m_extra_playerinfo->dead == false;
}

CColor CGenericPlayer::get_color_based_on_team() const
{
	if (is_local_player())
	{
		return CColor(0, 252, 230, 230);
	}

	switch (get_team())
	{
		default:
		case hl::UNASSIGNED: 
			return CColor(50, 100, 50, 230);
		case hl::TERRORIST: 
			return CColor(230, 0, 0, 230);
		case hl::CT: 
			return CColor(0, 50, 230, 230);
		case hl::SPECTATOR:
			return CColor(230, 230, 230, 230);
	}
}

std::string CGenericPlayer::get_team_as_string() const
{
	if (!m_extra_playerinfo)
	{
		return "invalid";
	}

	if (!m_extra_playerinfo->teamname || !m_extra_playerinfo->teamname[0])
	{
		return "empty";
	}

	return m_extra_playerinfo->teamname;
}

bool CGenericPlayer::is_local_player() const
{
	return CGameUtil::the().is_local_player(m_playerinfo_index + 1);
}

bool CGenericPlayer::is_local_spectating() const
{
	return m_playerinfo_index == CLocalState::the().get_spectating_player() && CLocalState::the().get_spectating_mode() == SPECT_FIRST_PERSON;
}

uint64_t CGenericPlayer::get_steam_id() const
{
	if (!m_playerinfo)
	{
		return 0ull;
	}

	return is_local_player() ? CLocalState::the().get_local_steamid().ConvertToUint64() : m_playerinfo->m_nSteamID;
}

bool CGenericPlayer::is_bot() const
{
	return CSteamIDUtil::the().is_bot(get_steam_id());
}

bool CGenericPlayer::is_steam_user() const
{
	return CSteamIDUtil::the().is_steam_user(get_steam_id());
}

bool CGenericPlayer::is_local_friend_with() const
{
	if (COxWare::the().get_build_number() < 8684)
	{
		return false;
	}

	// cannot be a friend with ourselfs
	if (is_local_player())
	{
		return false;
	}

	if (!m_playerinfo)
	{
		return false;
	}

	auto relationship = CHLInterfaceHook::the().ISteamFriends()->GetFriendRelationship(m_playerinfo->m_nSteamID);
	return relationship == hl::k_EFriendRelationshipFriend;
}

std::string CGenericPlayer::get_steam_persona_name() const
{
	if (COxWare::the().get_build_number() < 8684)
	{
		return "";
	}

	if (is_local_player())
	{
		return CHLInterfaceHook::the().ISteamFriends()->GetPersonaName();
	}

	if (!m_playerinfo)
	{
		return "";
	}

	return CHLInterfaceHook::the().ISteamFriends()->GetFriendPersonaName(m_playerinfo->m_nSteamID);
}

hl::EPersonaState CGenericPlayer::get_persona_state() const
{
	if (COxWare::the().get_build_number() < 8684)
	{
		return hl::k_EPersonaStateMax;
	}

	if (is_local_player())
	{
		return CHLInterfaceHook::the().ISteamFriends()->GetPersonaState();
	}

	if (!m_playerinfo)
	{
		return hl::k_EPersonaStateMax;
	}

	return CHLInterfaceHook::the().ISteamFriends()->GetFriendPersonaState(m_playerinfo->m_nSteamID);
}
