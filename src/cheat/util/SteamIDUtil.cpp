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

//
// SteamIDUtil.cpp -- various steam id utilities.
// 
//------------------------------------------------------------------------------
// STEAM ID 2:
// 
//	STEAM_X:Y:Z where:
// 
//	-	X represents the "Universe" the steam account belongs to.
//	-	Y is part of the ID number for the account. Y is either 0 or 1.
//	-	Z is the "account number".
// 
//------------------------------------------------------------------------------
// STEAM ID 3:
// 
//	[letter:X:Y] where:
//
//	-	letter represents letter for account type (e.g. for k_EAccountTypePending will be 'P', etc.)
//	-	X is the universe ID.
//	-	Y account id
// 
// NOTE: that there can be more forms of sid3. See code below for various forms.
//

#include "precompiled.h"

std::string CSteamIDUtil::render_steam3_id(SteamID_t steam_id)
{
	std::string steam3;
	
	if (hl::k_EAccountTypeAnonGameServer == steam_id.m_EAccountType)
	{
		steam3 = std::format("[A-{}:{}({})]", (int)steam_id.m_EUniverse, (unsigned int)steam_id.m_unAccountID, (unsigned int)steam_id.m_unAccountInstance);
	}
	else if (hl::k_EAccountTypeGameServer == steam_id.m_EAccountType)
	{
		steam3 = std::format("[G-{}:{}]", (int)steam_id.m_EUniverse, (unsigned int)steam_id.m_unAccountID);
	}
	else if (hl::k_EAccountTypeMultiseat == steam_id.m_EAccountType)
	{
		steam3 = std::format("[{}:{}({}%)]", (int)steam_id.m_EUniverse, (unsigned int)steam_id.m_unAccountID, (unsigned int)steam_id.m_unAccountInstance);
	}
	else if (hl::k_EAccountTypePending == steam_id.m_EAccountType)
	{
		steam3 = std::format("[{}:{}(pending)]", (int)steam_id.m_EUniverse, (unsigned int)steam_id.m_unAccountID);
	}
	else if (hl::k_EAccountTypeContentServer == steam_id.m_EAccountType)
	{
		steam3 = std::format("[C-{}:{}]", (int)steam_id.m_EUniverse, (unsigned int)steam_id.m_unAccountID);
	}
	else
	{
		steam3 = std::format("[{}:{}]", (int)steam_id.m_EUniverse, (unsigned int)steam_id.m_unAccountID);
	}

	return steam3;
}

std::string CSteamIDUtil::render_steam2_id(SteamID_t steam_id)
{
	std::string steam2;

	bool is_steam = is_steam_user(steam_id);

	steam2 = std::format("{}_{:1}:{:1}:{:010}", 
						 (is_steam ? "STEAM" : "VALVE"),
						 (int)steam_id.m_EUniverse, 
						 (unsigned int)steam_id.High32bits,
						 (unsigned int)steam_id.Low32bits);

	return steam2;
}

std::string CSteamIDUtil::steam_id_to_url(SteamID_t steam_id)
{
	std::string sub_path = sub_path_from_account_type(steam_id);
	SteamID_t community_id = convert_to_steam_community_id(steam_id);

	return "www.steamcommunity.com/" + sub_path + "/" + std::to_string(community_id.m_unAll64Bits);
}

std::string CSteamIDUtil::sub_path_from_account_type(SteamID_t steam_id)
{
	switch (steam_id.m_EAccountType)
	{
		case hl::k_EAccountTypeIndividual:
		{
			return "profiles";
		}
		case hl::k_EAccountTypeClan:
		{
			return "groups";
		}
		default:
		{
			return "";
		}
	}
}

SteamID_t CSteamIDUtil::get_steam_id_account_identifier(SteamID_t steam_id)
{
	SteamID_t result{};

	switch (steam_id.m_EAccountType)
	{
		case hl::k_EAccountTypeIndividual:
		{
			result.m_unAccountID = 0UL;
			result.m_unAccountInstance = hl::k_unSteamUserDesktopInstance;
			result.m_EAccountType = hl::k_EAccountTypeIndividual;
			result.m_EUniverse = hl::k_EUniversePublic;
			break;
		}
		case hl::k_EAccountTypeClan:
		{
			result.m_unAccountID = 0UL;
			result.m_unAccountInstance = 0;
			result.m_EAccountType = hl::k_EAccountTypeClan;
			result.m_EUniverse = hl::k_EUniversePublic;
			break;
		}
	}

	return result;
}

SteamID_t CSteamIDUtil::convert_to_steam_community_id(SteamID_t steam_id)
{
	// https://developer.valvesoftware.com/wiki/SteamID#Steam_ID_as_a_Steam_Community_ID

	SteamID_t result;
	SteamID_t sid2 = to_steam_id2(steam_id);
	SteamID_t sid_64_identifier = get_steam_id_account_identifier(steam_id);

	result.m_unAll64Bits = (sid2.Low32bits * 2 + sid_64_identifier.m_unAll64Bits + sid2.High32bits);

	return result;
}

SteamID_t CSteamIDUtil::to_steam_id2(SteamID_t steam_id)
{
	SteamID_t sid2;

	sid2.High32bits = steam_id.m_unAccountID % 2;
	sid2.Low32bits = steam_id.m_unAccountID / 2;

	return sid2;
}

bool CSteamIDUtil::is_steam_user(SteamID_t steam_id)
{
	//	This is kind of weird way to check, but if the player
	//	is a steam player, the first 7 digits will be always
	//	the same for every player.
	//
	//	return sid64 / 10000000000 == 7656119;
	return
		steam_id.m_EAccountType == hl::k_EAccountTypeIndividual &&
		steam_id.m_EUniverse == hl::k_EUniversePublic &&
		steam_id.m_unAccountInstance == hl::k_unSteamUserDesktopInstance;
}
