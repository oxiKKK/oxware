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
// MessageBox.cpp -- Simple wrapper around Windows MessageBox API.
//

#include "precompiled.h"

void CMessageBox::display_generic(EMessageBoxType type, const std::string& text, const SourceLocation& location)
{
	MessageBoxA(NULL, format_text(text, location).c_str(), mbox_parameters[type].caption, mbox_parameters[type].properties);
}

std::string CMessageBox::format_text(const std::string& input_text, const SourceLocation& location)
{
#ifdef _RETAIL
	return std::format("{}", input_text);
#else
	return std::format("{} in\n{}:{}:{}\n\n{}", location.function_name(), CGenericUtil::the().get_filename_from_path(location.file_name()), location.line(), location.column(), input_text);
#endif
}
