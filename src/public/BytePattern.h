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
// BytePattern.h -- A simple utility class that enables byte pattern searching in loaded address space.
//

#ifndef BYTEPATTERN_H
#define BYTEPATTERN_H

#pragma once

// opcode representing runtine that can be relocated.
#define RELOCABLE_ADDRESS_WILDCARD (uint8_t)('\xCC')

static constexpr size_t k_max_pattern_length = 128; // had to increase once.

class CBaseBytePattern
{
public:
	CBaseBytePattern() :
		m_pattern(""), m_mask(""),
		m_offset(0)
	{
	}

public:
	std::string pattern_as_string() const
	{
		std::string result;
		for (size_t k = 0; k < m_pattern.length(); k++)
		{
			const uint8_t b = m_pattern[k];
			result += std::format("\\x{:02X}", b);
		}
		return result;
	}

	inline std::string get_mask() const { return m_mask; }
	inline std::string get_pattern_raw() const { return m_pattern; }
	inline uintptr_t get_offset() const { return m_offset; }

	uintptr_t* search_in_loaded_address_space(uintptr_t start_addr, uintptr_t end_addr) const
	{
		bool found = false;
		for (uintptr_t i = start_addr; i < end_addr - m_pattern.length(); i++)
		{
			found = true;
			for (size_t k = 0; k < m_pattern.length(); k++)
			{
				if (m_mask[k] != 'x')
				{
					continue;
				}

				uint8_t pat = m_pattern[k];
				uint8_t mem = *(uint8_t*)(i + k);

				if (pat != mem)
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				return reinterpret_cast<uintptr_t*>(i + m_offset);
			}
		}

		return nullptr;
	}

	bool empty() const
	{
		return m_pattern.empty() || m_mask.empty();
	}

protected:
	std::string m_pattern, m_mask; // the mask is automatically generated
	uintptr_t m_offset;
};

template <size_t MaxLen>
class CBytePatternWithLengthConstexpr : public CBaseBytePattern
{
public:
	CBytePatternWithLengthConstexpr() : CBaseBytePattern()
	{
	}

	template<size_t N> requires(N < MaxLen)
	CBytePatternWithLengthConstexpr(const char(&pattern)[N], uintptr_t offset = 0x00000000)
	{
		m_offset = offset;

		m_mask = generate_mask_from_pattern_constexpr(pattern);

		// Doesn't end the nullcharacter if we specify the length however,
		// the sequence of characters we pass to this function is NULL TERMINATED!
		m_pattern = std::string(pattern, N - 1);
	}

public:
	template<size_t N> requires(N < k_max_pattern_length)
	static inline auto generate_mask_from_pattern_constexpr(const char(&pattern)[N])
	{
		std::string a;
		for (size_t i = 0; i < N - 1; i++)
		{
			uint8_t b = (uint8_t)pattern[i];
			a.push_back(b == RELOCABLE_ADDRESS_WILDCARD ? '?' : 'x');
		}
		return a;
	}
};

class CBytePatternRuntime : public CBaseBytePattern
{
public:
	CBytePatternRuntime() : CBaseBytePattern()
	{
	}

	// ASSUME LENGTH IS NOT INLCUDING THE NULL TERMINATOR!
	CBytePatternRuntime(const char* pattern, size_t length, uintptr_t offset = 0x00000000)
	{
		m_offset = offset;
		m_mask = generate_mask_from_pattern_runtime(pattern, length);

		// Doesn't end the nullcharacter if we specify the length.
		m_pattern = std::string(pattern, length);
	}

public:
	static inline std::string generate_mask_from_pattern_runtime(const char* pattern, size_t length)
	{
		std::string a;
		for (size_t i = 0; i < length; i++)
		{
			uint8_t b = (uint8_t)(pattern[i]);
			a.push_back(b == RELOCABLE_ADDRESS_WILDCARD ? '?' : 'x');
		}
		return a;
	}
};

using CBytePattern = CBytePatternWithLengthConstexpr<k_max_pattern_length>;

#endif // BYTEPATTERN_H