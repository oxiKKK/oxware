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

#ifndef STEAMIDUTIL_H
#define STEAMIDUTIL_H
#pragma once

union SteamID_t
{
	SteamID_t() :
		m_unAll64Bits(0ULL)
	{
	}

	SteamID_t(uint64_t id) :
		m_unAll64Bits(id)
	{
	}

	struct
	{
		uint32_t		m_unAccountID : 32; // unique account identifier
		uint32_t		m_unAccountInstance : 20; // dynamic instance ID
		uint32_t		m_EAccountType : 4;  // type of account - can't show as EAccountType, due to signed / unsigned difference
		hl::EUniverse	m_EUniverse : 8;  // universe this account belongs to
	};

	struct
	{
		uint32_t		Low32bits : 32;
		uint32_t		High32bits : 32;
	};

	uint64_t m_unAll64Bits;
};

class CSteamIDUtil
{
public:
	DECL_BASIC_CLASS(CSteamIDUtil);

public:
	// converts team id to steam3 id: "[letter:1:W]"
	std::string render_steam3_id(SteamID_t steam_id);

	// converts team id to steam2 id: "STEAM_X:Y:Z"
	std::string render_steam2_id(SteamID_t steam_id);

	// steam id to steamcommunity url
	std::string steam_id_to_url(SteamID_t steam_id);

	// sub path by account path that is used in steamcommunity url
	std::string sub_path_from_account_type(SteamID_t steam_id);

	// returns steam identifier for account id
	SteamID_t get_steam_id_account_identifier(SteamID_t steam_id);

	// converts steam id to community id
	SteamID_t convert_to_steam_community_id(SteamID_t steam_id);

	// convert steam id 3 to steam id 2
	SteamID_t to_steam_id2(SteamID_t steam_id);

	// tries to determine if player is using steam or nonsteam depending on their steam id
	// see the function for more details
	bool is_steam_user(SteamID_t steam_id);
};

#endif // STEAMIDUTIL_H