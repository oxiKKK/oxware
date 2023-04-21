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

#ifndef SOURCE_LOCATION_H
#define SOURCE_LOCATION_H
#pragma once

#include <cstdint>

class SourceLocation
{
public:
	static consteval SourceLocation current(const uint_least32_t line = __builtin_LINE(),
		const uint_least32_t column = __builtin_COLUMN(), const char* const file = __builtin_FILE(),
		const char* const function = __builtin_FUNCSIG()) noexcept
	{
		SourceLocation result;
		result.m_line = line;
		result.m_column = column;
		result.m_file = file;
		result.m_function = function;
		return result;
	}
	constexpr SourceLocation() noexcept = default;

	inline bool operator==(const SourceLocation& other)
	{
		return m_line == other.m_line && m_column == other.m_column;
	}

	inline bool operator!=(const SourceLocation& other)
	{
		return m_line != other.m_line || m_column != other.m_column;
	}

	constexpr uint_least32_t line() const noexcept { return m_line; }
	constexpr uint_least32_t column() const noexcept { return m_column; }
	constexpr const char* file_name() const noexcept { return m_file; }
	constexpr const char* function_name() const noexcept { return m_function; }

private:
	uint_least32_t m_line{};
	uint_least32_t m_column{};
	const char* m_file = "";
	const char* m_function = "";
};

#endif // SOURCE_LOCATION_H