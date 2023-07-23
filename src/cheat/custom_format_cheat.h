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
// custom_format.h -- custom types for std::format for our cheat
//

// mod_type
template <>
struct std::formatter<hl::modtype_t> : std::formatter<std::string> {
	auto format(hl::modtype_t type, std::format_context& ctx) {
		switch (type)
		{
			case hl::mod_bad:		return std::formatter<string>::format("mod_bad", ctx);
			case hl::mod_brush:		return std::formatter<string>::format("mod_brush", ctx);
			case hl::mod_sprite:	return std::formatter<string>::format("mod_sprite", ctx);
			case hl::mod_alias:		return std::formatter<string>::format("mod_alias", ctx);
			case hl::mod_studio:	return std::formatter<string>::format("mod_studio", ctx);
			default:				return std::formatter<string>::format("unknown", ctx);
		}
	}
};


// FORCE_TYPE
template <>
struct std::formatter<hl::FORCE_TYPE> : std::formatter<std::string> {
	auto format(hl::FORCE_TYPE type, std::format_context& ctx) {

		switch (type)
		{
			case hl::force_exactfile:						return std::formatter<string>::format("force_exactfile", ctx);
			case hl::force_model_samebounds:				return std::formatter<string>::format("force_model_samebounds", ctx);
			case hl::force_model_specifybounds:				return std::formatter<string>::format("force_model_specifybounds", ctx);
			case hl::force_model_specifybounds_if_avail:	return std::formatter<string>::format("force_model_specifybounds_if_avail", ctx);
			default:										return std::formatter<string>::format("unknown", ctx);
		}
	}
};

// hl::EUniverse
template <>
struct std::formatter<hl::EUniverse> : std::formatter<std::string> {
	auto format(hl::EUniverse type, std::format_context& ctx) {

		switch (type)
		{
			case hl::k_EUniverseInvalid:		return std::formatter<string>::format("invalid", ctx);
			case hl::k_EUniversePublic:			return std::formatter<string>::format("public", ctx);
			case hl::k_EUniverseBeta:			return std::formatter<string>::format("beta", ctx);
			case hl::k_EUniverseInternal:		return std::formatter<string>::format("internal", ctx);
			case hl::k_EUniverseDev:			return std::formatter<string>::format("dev", ctx);
		}
		return std::formatter<string>::format("invalid (" + std::to_string((int)type) + ")", ctx);
	}
};


// hl::EAccountType
template <>
struct std::formatter<hl::EAccountType> : std::formatter<std::string> {
	auto format(hl::EAccountType type, std::format_context& ctx) {

		switch (type)
		{
			case hl::k_EAccountTypeInvalid:			return std::formatter<string>::format("invalid", ctx);
			case hl::k_EAccountTypeIndividual:		return std::formatter<string>::format("individual", ctx);
			case hl::k_EAccountTypeMultiseat:		return std::formatter<string>::format("multiseat", ctx);
			case hl::k_EAccountTypeGameServer:		return std::formatter<string>::format("gameserver", ctx);
			case hl::k_EAccountTypeAnonGameServer:	return std::formatter<string>::format("anongameserver", ctx);
			case hl::k_EAccountTypePending:			return std::formatter<string>::format("pedning", ctx);
			case hl::k_EAccountTypeContentServer:	return std::formatter<string>::format("contentserver", ctx);
			case hl::k_EAccountTypeClan:			return std::formatter<string>::format("clan", ctx);
			case hl::k_EAccountTypeChat:			return std::formatter<string>::format("chat", ctx);
			case hl::k_EAccountTypeConsoleUser:		return std::formatter<string>::format("consoleuser", ctx);
			case hl::k_EAccountTypeAnonUser:		return std::formatter<string>::format("anonuser", ctx);
		}

		return std::formatter<string>::format("invalid (" + std::to_string((int)type) + ")", ctx);
	}
};


// hl::EPersonaState
template <>
struct std::formatter<hl::EPersonaState> : std::formatter<std::string> {
	auto format(hl::EPersonaState type, std::format_context& ctx) {

		switch (type)
		{
			case hl::k_EPersonaStateOffline:		return std::formatter<string>::format("offline", ctx);
			case hl::k_EPersonaStateOnline:			return std::formatter<string>::format("online", ctx);
			case hl::k_EPersonaStateBusy:			return std::formatter<string>::format("busy", ctx);
			case hl::k_EPersonaStateAway:			return std::formatter<string>::format("away", ctx);
			case hl::k_EPersonaStateSnooze:			return std::formatter<string>::format("snooze", ctx);
			case hl::k_EPersonaStateLookingToTrade: return std::formatter<string>::format("lookingtotrade", ctx);
			case hl::k_EPersonaStateLookingToPlay:	return std::formatter<string>::format("lookingtoplay", ctx);
		}

		return std::formatter<string>::format("invalid (" + std::to_string((int)type) + ")", ctx);
	}
};

