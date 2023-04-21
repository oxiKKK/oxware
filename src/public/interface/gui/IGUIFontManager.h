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

#ifndef IGUIFONTMANAGER_H
#define IGUIFONTMANAGER_H
#pragma once

#include <string>
#include <map>

enum EFontDecoration
{
	FONTDEC_Regular = BIT(0),
	FONTDEC_Bold = BIT(1),
	FONTDEC_Light = BIT(2),
};

struct FontObject_t
{
	FontObject_t() :
		m_precached_font_object(nullptr)
	{
	}
	explicit FontObject_t(ImFont* precached_font) :
		m_precached_font_object(precached_font)
	{
	}

	inline float get_size_px() const { assert(m_precached_font_object); return m_precached_font_object->FontSize; };

	ImFont* m_precached_font_object;
};

enum EFontSize
{
	FONT_SMALLEST, 
	FONT_SMALL, 
	FONT_MEDIUM, 
	FONT_REGULAR, 
	FONT_BIGGER, 
	FONT_BIG, 
	FONT_BIGGEST, 
	FONT_EXTRA, 

	FONT_SIZE_COUNT
};

static constexpr float k_font_scale_min = 10.0f;
static constexpr float k_font_scale_step = 3.0f; // we will build FONT_SIZE_COUNT fonts with this scaling factor. So it will be 10, 13, 16, 19, ... FONT_SIZE_COUNTx

// holds decorated instances of the same font
struct FontPlaceholder_t
{
	FontPlaceholder_t() :
		m_font_name("uninitialized"),
		m_decorated_scaled_variants()
	{
	}

	void add_sized_font(ImFont* precached_font, EFontDecoration decor, EFontSize size)
	{
		m_decorated_scaled_variants[decor][size] = FontObject_t(precached_font);
	}

	inline bool does_have_decoration(EFontDecoration decor)
	{
		for (auto& [key, scaled_fonts] : m_decorated_scaled_variants)
		{
			if (decor == key)
				return true;
		}
		return false;
	}

	inline FontObject_t* get_by_decoration_and_size(EFontDecoration decor, EFontSize size)
	{
		// fuuu, a lot of maps
		for (auto& [key, scaled_fonts] : m_decorated_scaled_variants)
		{
			if (decor == key)
			{
				for (auto& [key1, font1] : scaled_fonts)
				{
					if (key1 == size)
						return &font1;
				}
			}
		}
		assert(0);
		return nullptr;
	}

	const char* m_font_name;

	// holds decorated fonts (bold, light, etc) and each of these has FONT_SIZE_COUNT amount of fonts precached, each having unique size in pixels.
	std::map<EFontDecoration, std::map<EFontSize, FontObject_t>> m_decorated_scaled_variants;
};

struct ImFont;

class IGUIFontManager
{
public:
	virtual void initialize() = 0;

	virtual void push_default_font() = 0;
	virtual void pop_default_font() = 0;

	virtual FontObject_t* get_font(const char* name, EFontSize size = FONT_REGULAR, EFontDecoration decor = FONTDEC_Regular) = 0;
	virtual	ImFont* get_imgui_font(const char* name, EFontSize size = FONT_REGULAR, EFontDecoration decor = FONTDEC_Regular) = 0;
	virtual FontObject_t* get_default_font() = 0;

	virtual Vector2D calc_font_text_size(FontObject_t* font, const char* text) = 0;

	virtual void build_new_font_from_mem(const char* name) = 0;
};

extern IGUIFontManager* g_gui_fontmgr_i;

#define IGUIFONTMANAGER_INTERFACEID "IGUIFontManager"

#endif // IGUIFONTMANAGER_H