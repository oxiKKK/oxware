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

// old steam id, used by goldsrc e.g. in "status" command.
// see TSteamGlobalUserID in SteamCommon.h
struct Steam2ID_t
{
	uint16_t m_SteamInstanceID;

	union
	{
		uint64_t	As64bits;
		struct
		{
			unsigned int	Low32bits;
			unsigned int	High32bits;
		} Split;
	} m_SteamLocalUserID;
};

// new steam id
// see CSteamID in steamclientpublic.h
union Steam3ID_t
{
	Steam3ID_t() :
		m_unAll64Bits(0ULL)
	{
	}

	Steam3ID_t(uint64_t id) :
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
	// converts team id to steam3/2 id: "[letter:1:W]"
	std::string render_steam3_id_variant_1(Steam3ID_t steam3_id);

	// converts team id to steam3/2 id: "STEAM_X:Y:Z"
	std::string render_steam3_id_variant_2(Steam3ID_t steam3_id);
	std::string render_steam2_id_variant_2(Steam3ID_t steam3_id); // NOTE: we need to check for steam through steamid3

	// render steam account
	std::string render_steam_account(Steam3ID_t steam3_id);

	// steam id to steamcommunity url
	std::string steam_id_to_url(Steam3ID_t steam3_id);

	// sub path by account path that is used in steamcommunity url
	std::string sub_path_from_account_type(Steam3ID_t steam3_id);

	// returns steam identifier for account id
	Steam3ID_t get_steam_id_account_identifier(Steam3ID_t steam3_id);

	// converts steam id to community id
	Steam3ID_t convert_to_steam_community_id(Steam3ID_t steam3_id);

	// convert steam id 3 to steam id 2 and vice versa
	Steam2ID_t to_steam2(Steam3ID_t steam3_id);

	// tries to determine if player is using steam or nonsteam depending on their steam id
	// see the function for more details
	bool is_steam_user(Steam3ID_t steam3_id);

	// true if bot
	bool is_bot(Steam3ID_t steam3_id);

	// validates steam id
	bool is_valid(Steam3ID_t steam3_id);

	// true if player used steam id changer.
	bool is_using_sid_changer(Steam3ID_t steam3_id);
};

#endif // STEAMIDUTIL_H