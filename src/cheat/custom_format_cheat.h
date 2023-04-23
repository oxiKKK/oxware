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
		assert((type >= hl::mod_bad && type <= hl::mod_studio) && "invalid modtype specified in formatter.");

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

