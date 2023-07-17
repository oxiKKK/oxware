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

#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H
#pragma once

#include "commondefs.h"
#include <string>

class CStringTools
{
public:
	DECL_BASIC_CLASS(CStringTools);

public:
	std::string utf16_to_utf8(const std::wstring& wstr);
	std::wstring utf8_to_utf16(const std::string& str);

	std::wstring fix_trailing_slashes(const std::wstring& wstr);

	// trim
	std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v");
	std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");
	std::string& trim(std::string& s, const char* t = " \t\n\r\f\v");

	std::string format_timestamp_log(const std::chrono::system_clock::time_point& point);
	std::string format_date(const std::chrono::system_clock::time_point& point);

	std::string pretify_file_size(float value);
	std::string pretify_value(float value, int digits_after_decimal = 2, bool use_binary_base = false, bool mebibytes = false);

	bool case_insensitive_string_search(const std::string& haystack, const std::string& needle);
};

#endif // STRINGTOOLS_H