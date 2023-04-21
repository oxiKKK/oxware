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

std::string CStringTools::unicode_to_utf8(const std::wstring& wstr)
{
	auto buf_len = wstr.length();
	std::string result(buf_len, ' ');
	WideCharToMultiByte(CP_UTF8, NULL, wstr.c_str(), buf_len, (LPSTR)result.c_str(), buf_len, nullptr, nullptr);
	return result;
}

std::wstring CStringTools::utf8_to_unicode(const std::string& str)
{
	auto buf_len = str.length();
	std::wstring result(buf_len, ' ');
	MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), buf_len, (LPWSTR)result.c_str(), buf_len);
	return result;
}

std::wstring CStringTools::fix_trailing_slashes(const std::wstring& wstr)
{
	std::wstring corrected = wstr;
	for (size_t i = 0; i < wstr.length(); i++)
	{
		if (corrected[i] == '/')
			corrected[i] = '\\';
	}
	return corrected;
}

std::string CStringTools::format_timestamp_log(const std::chrono::system_clock::time_point& point)
{
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(point.time_since_epoch()) % 1000;
	auto now_c = std::chrono::system_clock::to_time_t(point);
	std::tm* now_tm = std::localtime(&now_c);
	std::string timestamp = std::format("{:02}:{:02}:{:02}.{:03}", now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec, ms.count());

	return timestamp;
}

std::string CStringTools::format_date(const std::chrono::system_clock::time_point & point)
{
	auto now_c = std::chrono::system_clock::to_time_t(point);
	std::tm* now_tm = std::localtime(&now_c);
	std::string date = std::format("{}/{}/{} | {:02}:{:02}:{:02}", 
								   now_tm->tm_mday, now_tm->tm_mon, now_tm->tm_year + 1900, 
								   now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);

	return date;
}
