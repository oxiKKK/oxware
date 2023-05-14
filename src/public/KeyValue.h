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

#ifndef KEYVALUE_H
#define KEYVALUE_H
#pragma once

#include <string>
#include <utility>

struct KeyValue
{
public:
	KeyValue() 
		: p("empty", "empty")
	{
	}

	KeyValue(const std::string& key, const std::string& value)
	{
		if (!key.empty())
		{
			p.first = key;
		}
		if (!value.empty())
		{
			p.second = value;
		}
	}

	std::string& key() { return p.first; }
	std::string& value() { return p.second; }
	const std::string& key() const { return p.first; }
	const std::string& value() const { return p.second; }

	inline bool operator==(const KeyValue& other) const
	{
		return (key() == other.key() && value() == other.value());
	}

	inline bool operator!=(const KeyValue& other) const
	{
		return (key() != other.key() || value() != other.value());
	}

	static KeyValue parse_kv_out_of_string(const std::string& kv_string)
	{
		bool dummy;
		return parse_kv_out_of_string(kv_string, dummy);
	}

	// expects [{}: {}]
	static KeyValue parse_kv_out_of_string(const std::string& kv_string, bool& ok)
	{
		KeyValue result = {};

		ok = true;

		size_t semicolon, end;

		try
		{
			semicolon = kv_string.find(":");
			end = kv_string.find("]");
		}
		catch (...)
		{
			ok = false;
			return result;
		}

		if (semicolon == std::string::npos || end == std::string::npos)
		{
			assert("Invalid keyvalue string passed to " __FUNCTION__ ".");
			ok = false;
			return result;
		}

		std::string key, value;

		try
		{
			key = kv_string.substr(1, semicolon - 1);
			value = kv_string.substr(semicolon + 2, end - (semicolon + 2));
		}
		catch (...)
		{
			ok = false;
			return result;
		}

		if (key.empty())
			key = "empty";
		
		if (value.empty())
			value = "empty";

		return KeyValue(key, value);
	}

private:
	std::pair<std::string, std::string> p = { "empty", "empty" };
};

#endif // KEYVALUE_H