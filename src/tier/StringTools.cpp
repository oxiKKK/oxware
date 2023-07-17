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

std::string CStringTools::utf16_to_utf8(const std::wstring& wstr)
{
	size_t buf_len = wstr.length();
	std::string result(buf_len, ' ');
	::WideCharToMultiByte(CP_UTF8, NULL, wstr.c_str(), buf_len, (LPSTR)result.c_str(), buf_len, nullptr, nullptr);
	return result;
}

std::wstring CStringTools::utf8_to_utf16(const std::string& str)
{
	size_t buf_len = str.length();
	std::wstring result(buf_len, ' ');
	::MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), buf_len, (LPWSTR)result.c_str(), buf_len);
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

std::string& CStringTools::rtrim(std::string& s, const char* t)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

std::string & CStringTools::ltrim(std::string& s, const char* t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

std::string& CStringTools::trim(std::string& s, const char* t)
{
	return ltrim(rtrim(s, t), t);
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
								   now_tm->tm_mday, now_tm->tm_mon + 1, now_tm->tm_year + 1900, 
								   now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);

	return date;
}

std::string CStringTools::pretify_file_size(float value)
{
	return pretify_value(value, 2, true, true);
}

std::string CStringTools::pretify_value(float value, int digits_after_decimal, bool use_binary_base, bool mebibytes)
{
	static char output[8][32];
	static int  current;

	float		onekb = use_binary_base ? 1024.0f : 1000.0f;
	float		onemb = onekb * onekb;

	char *out = output[current];
	current = (current + 1) & (8 - 1);

	char suffix[8];

	// First figure out which bin to use
	if (value > onemb)
	{
		value /= onemb;
		snprintf(suffix, sizeof(suffix), mebibytes ? " MiB" : " MB");
	}
	else if (value > onekb)
	{
		value /= onekb;
		snprintf(suffix, sizeof(suffix), mebibytes ? " KiB" : " KB");
	}
	else
	{
		snprintf(suffix, sizeof(suffix), " bytes");
	}

	char val[32];

	// Clamp to >= 0
	digits_after_decimal = std::max(digits_after_decimal, 0);

	// If it's basically integral, don't do any decimals
	if (fabs(value - (int)value) < 0.00001)
	{
		snprintf(val, sizeof(val), "%i%s", (int)value, suffix);
	}
	else
	{
		char fmt[32];

		// Otherwise, create a format string for the decimals
		snprintf(fmt, sizeof(fmt), "%%.%if%s", digits_after_decimal, suffix);
		snprintf(val, sizeof(val), fmt, value);
	}

	// Copy from in to out
	char *i = val;
	char *o = out;

	// Search for decimal or if it was integral, find the space after the raw number
	char *dot = strstr(i, ".");
	if (!dot)
	{
		dot = strstr(i, " ");
	}

	// Compute position of dot
	int pos = dot - i;
	// Don't put a comma if it's <= 3 long
	pos -= 3;

	while (*i)
	{
		// If pos is still valid then insert a comma every third digit, except if we would be
		//  putting one in the first spot
		if (pos >= 0 && !(pos % 3))
		{
			// Never in first spot
			if (o != out)
			{
				*o++ = ',';
			}
		}

		// Count down comma position
		pos--;

		// Copy rest of data as normal
		*o++ = *i++;
	}

	// Terminate
	*o = 0;

	return out;
}

// https://stackoverflow.com/questions/3152241/case-insensitive-stdstring-find
bool CStringTools::case_insensitive_string_search(const std::string& haystack, const std::string& needle)
{
	auto it = std::search(
		haystack.begin(), haystack.end(),
		needle.begin(), needle.end(),
		[](unsigned char ch1, unsigned char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
	);
	return (it != haystack.end());
}
