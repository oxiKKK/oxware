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

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H
#pragma once

// note that this isn't the standard's source_location.
#include "source_location.h"

#include "Console.h"

enum EMessageBoxType
{
	MB_Message,
	MB_Error,
	MB_Warning,

	MB_Count
};

struct messagebox_parameters_t
{
	char caption[64];
	DWORD properties;
};

static const messagebox_parameters_t mbox_parameters[MB_Count] =
{
	/*MB_Message	*/ { "Message", MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1 },
	/*MB_Error		*/ { "Error", MB_OK | MB_ICONERROR | MB_DEFBUTTON1 },
	/*MB_Warning	*/ { "Warning", MB_OK | MB_ICONWARNING | MB_DEFBUTTON1 },
};

class CMessageBox
{
	// use this class as pure static class.

public:

	//
	// All of this template-sugar because of optional SourceLocation parameter at the end.
	//

private:
	template <EMessageBoxType type, class... _Types>
	struct display_generic_t
	{
		display_generic_t(const std::format_string<_Types...> _Fmt, _Types&&... _Args, const SourceLocation& location = SourceLocation::current())
		{
			display_generic(type, std::vformat(_Fmt.get(), std::make_format_args(_Args...)), location);
		}
	};

	template <EMessageBoxType type, class... _Types>
	display_generic_t(const std::format_string<_Types...> _Fmt, _Types&&... _Args) -> display_generic_t<type, _Types...>;

	static void display_generic(EMessageBoxType type, const std::string& text, const SourceLocation& location);

	static std::string format_text(const std::string& input_text, const SourceLocation& location);

public:
	template <class... _Types>
	using display_message = display_generic_t<MB_Message, _Types...>;

	template <class... _Types>
	using display_error = display_generic_t<MB_Error, _Types...>;

	template <class... _Types>
	using display_warning = display_generic_t<MB_Warning, _Types...>;
};

#endif // MESSAGEBOX_H