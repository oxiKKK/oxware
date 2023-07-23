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
// STEAM 3 ID #1:
// 
//	STEAM_X:Y:Z where:
// 
//	-	X represents the "Universe" the steam account belongs to.
//	-	Y is part of the ID number for the account. Y is either 0 or 1.
//	-	Z is the "account number".
// 
//------------------------------------------------------------------------------
// STEAM 3 ID #2:
// 
//	[letter:X:Y] where:
//
//	-	letter represents letter for account type (e.g. for k_EAccountTypePending will be 'P', etc.)
//	-	X is the universe ID.
//	-	Y account id
// 
// NOTE: that there can be more forms of this sid. See code below for various forms.
//

#include "precompiled.h"

std::string CSteamIDUtil::render_steam3_id_variant_1(Steam3ID_t steam3_id)
{
	std::string steam3;
	
	if (hl::k_EAccountTypeAnonGameServer == steam3_id.m_EAccountType)
	{
		steam3 = std::format("[A-{}:{}({})]", (uint8_t)steam3_id.m_EUniverse, (uint32_t)steam3_id.m_unAccountID, (uint32_t)steam3_id.m_unAccountInstance);
	}
	else if (hl::k_EAccountTypeGameServer == steam3_id.m_EAccountType)
	{
		steam3 = std::format("[G-{}:{}]", (uint8_t)steam3_id.m_EUniverse, (uint32_t)steam3_id.m_unAccountID);
	}
	else if (hl::k_EAccountTypeMultiseat == steam3_id.m_EAccountType)
	{
		steam3 = std::format("[{}:{}({}%)]", (uint8_t)steam3_id.m_EUniverse, (uint32_t)steam3_id.m_unAccountID, (uint32_t)steam3_id.m_unAccountInstance);
	}
	else if (hl::k_EAccountTypePending == steam3_id.m_EAccountType)
	{
		steam3 = std::format("[{}:{}(pending)]", (uint8_t)steam3_id.m_EUniverse, (uint32_t)steam3_id.m_unAccountID);
	}
	else if (hl::k_EAccountTypeContentServer == steam3_id.m_EAccountType)
	{
		steam3 = std::format("[C-{}:{}]", (uint8_t)steam3_id.m_EUniverse, (uint32_t)steam3_id.m_unAccountID);
	}
	else
	{
		steam3 = std::format("[{:}:{:}]", (uint8_t)steam3_id.m_EUniverse, (uint32_t)steam3_id.m_unAccountID);
	}

	return steam3;
}

std::string CSteamIDUtil::render_steam3_id_variant_2(Steam3ID_t steam3_id)
{
	std::string steam2;

	bool is_steam = is_steam_user(steam3_id);

	steam2 = std::format("{}_{:01}:{:01}:{:010}", 
						 (is_steam ? "STEAM" : "VALVE"),
						 (uint8_t)steam3_id.m_EUniverse, 
						 (uint32_t)steam3_id.High32bits,
						 (uint32_t)steam3_id.Low32bits);

	return steam2;
}

std::string CSteamIDUtil::render_steam2_id_variant_2(Steam3ID_t steam3_id)
{
	std::string steam2;

	bool is_steam = is_steam_user(steam3_id);
	auto steam2_id = to_steam2(steam3_id);

	steam2 = std::format("{}_{:01}:{:01}:{:010}",
						 (is_steam ? "STEAM" : "VALVE"),
						 (uint16_t)steam2_id.m_SteamInstanceID, // should be always 0 tho
						 (uint32_t)steam2_id.m_SteamLocalUserID.Split.High32bits,
						 (uint32_t)steam2_id.m_SteamLocalUserID.Split.Low32bits);

	return steam2;
}

std::string CSteamIDUtil::render_steam_account(Steam3ID_t steam3_id)
{
	switch (steam3_id.m_EAccountType)
	{
		case hl::k_EAccountTypeInvalid:				return "invalid";
		case hl::k_EAccountTypeIndividual:			return "individual";
		case hl::k_EAccountTypeMultiseat:			return "multiseat";
		case hl::k_EAccountTypeGameServer:			return "game server";
		case hl::k_EAccountTypeAnonGameServer:		return "anon game server";
		case hl::k_EAccountTypePending:				return "pending";
		case hl::k_EAccountTypeContentServer:		return "content server";
		case hl::k_EAccountTypeClan:				return "clan";
		case hl::k_EAccountTypeChat:				return "chat";
		case hl::k_EAccountTypeConsoleUser:			return "console user";
		case hl::k_EAccountTypeAnonUser:			return "anon user";
	}

	return std::to_string(steam3_id.m_EAccountType);
}

std::string CSteamIDUtil::steam_id_to_url(Steam3ID_t steam3_id)
{
	std::string sub_path = sub_path_from_account_type(steam3_id);
	Steam3ID_t community_id = convert_to_steam_community_id(steam3_id);

	return "www.steamcommunity.com/" + sub_path + "/" + std::to_string(community_id.m_unAll64Bits);
}

std::string CSteamIDUtil::sub_path_from_account_type(Steam3ID_t steam3_id)
{
	switch (steam3_id.m_EAccountType)
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

Steam3ID_t CSteamIDUtil::get_steam_id_account_identifier(Steam3ID_t steam3_id)
{
	Steam3ID_t result{};

	switch (steam3_id.m_EAccountType)
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

Steam3ID_t CSteamIDUtil::convert_to_steam_community_id(Steam3ID_t steam3_id)
{
	// https://developer.valvesoftware.com/wiki/SteamID#Steam_ID_as_a_Steam_Community_ID

	Steam3ID_t result;
	Steam2ID_t sid2 = to_steam2(steam3_id);
	Steam3ID_t sid_64_identifier = get_steam_id_account_identifier(steam3_id);

	result.m_unAll64Bits = (sid2.m_SteamLocalUserID.Split.Low32bits * 2 + sid_64_identifier.m_unAll64Bits + sid2.m_SteamLocalUserID.Split.High32bits);

	return result;
}

Steam2ID_t CSteamIDUtil::to_steam2(Steam3ID_t steam3_id)
{
	// see CSteamID::ConvertToSteam2

	Steam2ID_t sid2;

	sid2.m_SteamInstanceID = 0;
	sid2.m_SteamLocalUserID.Split.High32bits = steam3_id.m_unAccountID % 2;
	sid2.m_SteamLocalUserID.Split.Low32bits = steam3_id.m_unAccountID / 2;

	return sid2;
}

bool CSteamIDUtil::is_steam_user(Steam3ID_t steam3_id)
{
	// This is kind of weird way to check, but if the player
	// is a steam player, the first 7 digits will be always
	// the same for every player.
	// 
	// return sid64 / 10000000000 == 7656119;
	return
		steam3_id.m_EAccountType == hl::k_EAccountTypeIndividual &&
		steam3_id.m_EUniverse == hl::k_EUniversePublic &&
		steam3_id.m_unAccountInstance == hl::k_unSteamUserDesktopInstance;
}

bool CSteamIDUtil::is_bot(Steam3ID_t steam3_id)
{
	return steam3_id.m_unAll64Bits == 0ULL;
}

bool CSteamIDUtil::is_valid(Steam3ID_t steam3_id)
{
	hl::CSteamID csid = steam3_id.m_unAll64Bits;
	return csid.IsValid();
}

bool CSteamIDUtil::is_using_sid_changer(Steam3ID_t steam3_id)
{
	// TODO: Note sure about that meh, on some servers they can just disable scoreboard avatars by the new cvar.

	if (!is_steam_user(steam3_id))
	{
		return false;
	}

	// if this fails and the user is steam user, he is using steamid changer
	int image_id = CHLInterfaceHook::the().ISteamFriends()->GetMediumFriendAvatar(steam3_id.m_unAll64Bits);

	return image_id == 0;
}
