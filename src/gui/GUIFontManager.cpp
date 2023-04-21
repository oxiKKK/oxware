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

#include "assets/compressed_font_data.h"

#include <imgui_internal.h>

IGUIFontManager* g_gui_fontmgr_i = nullptr;

class CGUIFontManager : public IGUIFontManager
{
public:
	CGUIFontManager();
	~CGUIFontManager();

	void initialize();

	void push_default_font();
	void pop_default_font();

	FontObject_t* get_font(const char* name, EFontSize size = FONT_REGULAR, EFontDecoration decor = FONTDEC_Regular);
	ImFont* get_imgui_font(const char* name, EFontSize size = FONT_REGULAR, EFontDecoration decor = FONTDEC_Regular);
	FontObject_t* get_default_font();

	Vector2D calc_font_text_size(FontObject_t* font, const char* text);

	void build_new_font_from_mem(const char* name);

private:
	BaseFontContainer_t* find_static_font_container(const char* name)
	{
		for (auto& f : g_static_fonts)
		{
			if (!stricmp(name, f.m_name))
			{
				return &f;
			}
		}
		assert(0);
		return nullptr;
	}

private:
	std::vector<FontPlaceholder_t> m_precached_fonts;

	FontObject_t* m_default_font = nullptr;
};

CGUIFontManager g_gui_fontmgr;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGUIFontManager, IGUIFontManager, IGUIFONTMANAGER_INTERFACEID, g_gui_fontmgr);

//-------------------------------------------------------------------------------
//
// CGUIThemeManager implementation
// 
//-------------------------------------------------------------------------------

CGUIFontManager::CGUIFontManager()
{
	g_gui_fontmgr_i = this;
}

CGUIFontManager::~CGUIFontManager()
{
	g_gui_fontmgr_i = nullptr;
}

void CGUIFontManager::initialize()
{
	CConsole::the().info("Initializing GUI FontManager");
	CConsole::the().info("There will be {} instances of the same font with size beginning from {} up to {} (step is {})", 
						 (int)FONT_SIZE_COUNT, k_font_scale_min, k_font_scale_min + (k_font_scale_step * (float)FONT_SIZE_COUNT), k_font_scale_step);

	build_new_font_from_mem("segoeui");
	build_new_font_from_mem("proggyclean");

	m_default_font = get_font("segoeui", FONT_MEDIUM, FONTDEC_Bold);
}

void CGUIFontManager::push_default_font()
{
	ImGui::PushFont(m_default_font->m_precached_font_object);
}

void CGUIFontManager::pop_default_font()
{
	ImGui::PopFont();
}

FontObject_t* CGUIFontManager::get_font(const char* name, EFontSize size, EFontDecoration decor)
{
	for (auto& f : m_precached_fonts)
	{
		if (!stricmp(f.m_font_name, name))
		{
			return f.get_by_decoration_and_size(decor, size);
		}
	}

	assert(0);
	return nullptr;
}

ImFont* CGUIFontManager::get_imgui_font(const char* name, EFontSize size, EFontDecoration decor)
{
	auto font = get_font(name, size, decor);
	if (!font)
	{
		return nullptr;
	}

	return font->m_precached_font_object;
}

FontObject_t* CGUIFontManager::get_default_font()
{
	return m_default_font;
}

Vector2D CGUIFontManager::calc_font_text_size(FontObject_t* font, const char* text)
{
	assert(font && font->m_precached_font_object);
	return font->m_precached_font_object->CalcTextSizeA(font->get_size_px(), FLT_MAX, 0, text);
}

void CGUIFontManager::build_new_font_from_mem(const char* name)
{
	// Note that the name must exist somewhere in the "assets" static library. See compressed_font_data.h

	auto& io = ImGui::GetIO();

	FontPlaceholder_t font_placeholder;
	font_placeholder.m_font_name = name;

	float scale_step = 0.0f;
	for (size_t n = FONT_SMALLEST; n < FONT_SIZE_COUNT; n++)
	{
		auto static_font = find_static_font_container(name);
		assert(static_font);

		//
		// regular
		//
		if (static_font->m_regular.raw_data)
		{
			auto font_regular = io.Fonts->AddFontFromMemoryCompressedTTF(static_font->m_regular.raw_data, static_font->m_regular.size, k_font_scale_min + scale_step);
			assert(font_regular);
			font_placeholder.add_sized_font(font_regular, FONTDEC_Regular, (EFontSize)n);
		}

		//
		// bold
		//
		if (static_font->m_bold.raw_data)
		{
			auto font_bold = io.Fonts->AddFontFromMemoryCompressedTTF(static_font->m_bold.raw_data, static_font->m_bold.size, k_font_scale_min + scale_step);
			assert(font_bold);
			font_placeholder.add_sized_font(font_bold, FONTDEC_Bold, (EFontSize)n);
		}

		//
		// light
		//
		if (static_font->m_light.raw_data)
		{
			auto font_light = io.Fonts->AddFontFromMemoryCompressedTTF(static_font->m_light.raw_data, static_font->m_light.size, k_font_scale_min + scale_step);
			assert(font_light);
			font_placeholder.add_sized_font(font_light, FONTDEC_Light, (EFontSize)n);
		}

		scale_step += k_font_scale_step;
	}

	m_precached_fonts.push_back(font_placeholder);

	CConsole::the().info("Added font '{}' to the list.", name);
}
