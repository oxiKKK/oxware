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

#ifndef COLOR_H
#define COLOR_H
#pragma once

#include <cstdint>
#include <string>
#include <cassert>

#pragma warning(push)
#pragma warning(disable : 4244) // conversion from 'float' to 'int', possible loss of data

//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class CColor
{
public:
	constexpr CColor()
	{
		set(0.0f, 0.0f, 0.0f, 0.0f);
	}
	constexpr CColor(uint32_t hex)
	{
		set((int)(uint8_t)(hex & 0x000000FF), 
			(int)(uint8_t)((hex & 0x0000FF00) >> 8),
			(int)(uint8_t)((hex & 0x00FF0000) >> 16),
			(int)(uint8_t)((hex & 0xFF000000) >> 24));
	}
	constexpr CColor(int _r, int _g, int _b)
	{
		set(_r, _g, _b, 0);
	}
	constexpr CColor(int _r, int _g, int _b, int _a)
	{
		set(_r, _g, _b, _a);
	}
	constexpr CColor(float _r, float _g, float _b)
	{
		set(_r, _g, _b, 0.0f);
	}
	constexpr CColor(float _r, float _g, float _b, float _a)
	{
		set(_r, _g, _b, _a);
	}

	inline bool operator==(const CColor& other) const
	{
		return (r == other.r && g == other.g && b == other.b && a == other.a);
	}

	inline bool operator!=(const CColor& other) const
	{
		return (r != other.r || g != other.g || b != other.b || a != other.a);
	}

	constexpr void set(int _r, int _g, int _b, int _a)
	{
		r = (float)_r / 255.0f;
		g = (float)_g / 255.0f;
		b = (float)_b / 255.0f;
		a = (float)_a / 255.0f;
	}

	constexpr void set(float _r, float _g, float _b, float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	constexpr uint32_t as_u32() const
	{
		uint32_t out;
		out = ((uint32_t)f32_to_int8_sat(r)) << 0;
		out |= ((uint32_t)f32_to_int8_sat(g)) << 8;
		out |= ((uint32_t)f32_to_int8_sat(b)) << 16;
		out |= ((uint32_t)f32_to_int8_sat(a)) << 24;
		return out;
	}

	CColor as_255_based() const
	{
		return CColor(r * 255.0f, g * 255.0f, b * 255.0f, a * 255.0f);
	}

	CColor as_1_based() const
	{
		return CColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}

	float* get_base()
	{
		return &r;
	}

	static CColor parse_color_out_of_string(const std::string& color_string)
	{
		bool dummy;
		return parse_color_out_of_string(color_string, dummy);
	}

	// expects [r: {} g: {} b: {} a: {}]
	static CColor parse_color_out_of_string(const std::string& color_string, bool& ok)
	{
		CColor result = {};

		ok = true;

		auto parse_color = [&color_string, &ok](const std::string& token) -> float
		{
			auto pos = color_string.find(token);
			if (pos == std::string::npos)
			{
				assert("Invalid colors string passed to " __FUNCTION__ ".");
				ok = false;
				return 0.0f;
			}

			try
			{
				return std::stof(color_string.substr(pos + token.length(), color_string.find(" ", pos + token.length())));
			}
			catch (...)
			{
				ok = false;
				return 0.0f;
			}
		};

		return { parse_color("r: "), parse_color("g: "), parse_color("b: "), parse_color("a: "), };
	}

	bool is_nonzero() const
	{
		return r != 0.0f && g != 0.0f && b != 0.0f && a != 0.0f;
	}

	bool is_nonzero_rgb() const
	{
		return r != 0.0f && g != 0.0f && b != 0.0f;
	}

private:
	// Saturated, always output 0..255
	constexpr uint8_t f32_to_int8_sat(float clr) const
	{
		return ((int)(saturate(clr) * 255.0f + 0.5f));
	}

	constexpr float saturate(float f) const
	{
		return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f;
	}

public:
	float r, g, b, a;
};

#pragma warning(pop) 

#endif // COLOR_H