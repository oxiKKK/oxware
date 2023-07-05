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

#include <set>

static BaseFontContainer_t* find_static_font_container(const char* name)
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

//----------------------------------------------------------------------------------------------

using FontIdentifier_t = std::tuple<EFontId, EFontDecoration, float>;

MAKE_HASHABLE(FontIdentifier_t, std::get<0>(t), std::get<1>(t), std::get<1>(t));

//----------------------------------------------------------------------------------------------

IGUIFontManager* g_gui_fontmgr_i = nullptr;

BaseCommand ui_reload_fonts(
	"ui_reload_fonts", "Rebuilds all of the fonts.",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		g_gui_fontmgr_i->rebuild_fonts();
	}
);

class CGUIFontManager : public IGUIFontManager
{
public:
	CGUIFontManager();
	~CGUIFontManager();

	bool pre_newframe();

	void initialize();

	void push_default_font();
	void pop_default_font();

	ImFont* get_font(EFontId id, float size_px, EFontDecoration decor = FDC_Regular);
	ImFont* get_default_font();

	Vector2D calc_font_text_size(ImFont* font, const char* text);

	void rebuild_fonts()
	{
		m_needs_font_rebuild = true;
	}

	void add_freetype_builder_flags(FreeTypeBuilderFlags f, bool set)
	{
		if (set)
		{
			m_freetype_builder_flags |= f;
		}
		else
		{
			m_freetype_builder_flags &= ~f;
		}

		rebuild_fonts();
	}

	FreeTypeBuilderFlags get_freetype_builder_flags() { return m_freetype_builder_flags; }

private:
	std::unordered_map<FontIdentifier_t, ImFont*> m_precached_fonts;

	ImFont* m_default_font = nullptr;

	ImFont* precache_font(EFontId id, float size_px, EFontDecoration decor);

	bool m_needs_font_rebuild = false;
	std::set<FontIdentifier_t> m_fonts_that_needs_to_be_precached;

	FreeTypeBuilderFlags m_freetype_builder_flags;

	void rebuild_font_atlas();
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

bool CGUIFontManager::pre_newframe()
{
	for (const auto& [id, decor, px] : m_fonts_that_needs_to_be_precached)
	{
		precache_font(id, px, decor);

		if (!m_needs_font_rebuild)
		{
			m_needs_font_rebuild = true;
		}
	}

	m_fonts_that_needs_to_be_precached.clear();

	if (m_needs_font_rebuild)
	{
		rebuild_font_atlas();
		m_needs_font_rebuild = false;
		return true;
	}

	return false;
}

void CGUIFontManager::rebuild_font_atlas()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	ImFontAtlas* atlas = ImGui::GetIO().Fonts;
	atlas->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
	atlas->FontBuilderFlags = m_freetype_builder_flags;

	atlas->Build();

	auto t2 = std::chrono::high_resolution_clock::now();

	CConsole::the().info("Took {} seconds to rebuild all fonts.",
						 std::chrono::duration<float, std::ratio<1, 1>>(t2 - t1).count());
}

void CGUIFontManager::initialize()
{
	m_default_font = precache_font(FID_SegoeUI, 16.0f, FDC_Bold);
}

void CGUIFontManager::push_default_font()
{
	ImGui::PushFont(m_default_font);
}

void CGUIFontManager::pop_default_font()
{
	ImGui::PopFont();
}

ImFont* CGUIFontManager::get_font(EFontId id, float size_px, EFontDecoration decor)
{
	//
	// see if already precached
	//
	auto iter = m_precached_fonts.find(std::make_tuple(id, decor, size_px));
	if (iter != m_precached_fonts.end())
	{
		return (*iter).second;
	}

	//
	// not found, schedule precache and meanwhile return the default font
	//
	m_fonts_that_needs_to_be_precached.insert(std::make_tuple(id, decor, size_px));
	
	return m_default_font;
}

ImFont* CGUIFontManager::get_default_font()
{
	return m_default_font;
}

Vector2D CGUIFontManager::calc_font_text_size(ImFont* font, const char* text)
{
	return font->CalcTextSizeA(font->FontSize, FLT_MAX, 0, text);
}

ImFont* CGUIFontManager::precache_font(EFontId id, float size_px, EFontDecoration decor)
{
	auto& io = ImGui::GetIO();

	auto container = find_static_font_container(g_font_filenames[id]);
	assert(container && "The container for this font doesn't exist!");

	ImFont* added_font = nullptr;

	ImFontConfig cfg;
	cfg.SizePixels = size_px;

	// For smaller fonts, setting this throws out better results.
	if (size_px <= 12)
	{
		cfg.GlyphBuildFlags |= ImGuiFreeTypeGlyphBuildFlags_ForceAutoHint;
	}

	static const ImWchar glyph_ranges[] =
	{
		0x0020,  0x00FF, // ASCII
		0x0400,  0x052F, // Cyrillic + Cyrillic Supplement
		0x2000,  0x206F, // General Punctuation
		0x2DE0,  0x2DFF, // Cyrillic Extended-A
		0xFF00,  0xFFEF, // Half-width characters
		0x4e00,  0x9FAF, // CJK Ideograms
		0x2010,  0x205E, // Punctuations
		0,
	};

	switch (decor)
	{
		case FDC_Regular:
		{
			assert(container->m_regular.raw_data && "The font specified doesn't have regular decoration available!");

			added_font = io.Fonts->AddFontFromMemoryCompressedTTF(container->m_regular.raw_data, container->m_regular.size, size_px, &cfg, glyph_ranges);
			break;
		}
		case FDC_Bold:
		{
			assert(container->m_bold.raw_data && "The font specified doesn't have bold decoration available!");

			//cfg.GlyphBuildFlags = ImGuiFreeTypeGlyphBuildFlags_Italic;
			added_font = io.Fonts->AddFontFromMemoryCompressedTTF(container->m_bold.raw_data, container->m_bold.size, size_px, &cfg, glyph_ranges);
			break;
		}
		case FDC_Light:
		{
			assert(container->m_light.raw_data && "The font specified doesn't have light decoration available!");

			//cfg.GlyphBuildFlags = ImGuiFreeTypeGlyphBuildFlags_Bold;
			added_font = io.Fonts->AddFontFromMemoryCompressedTTF(container->m_light.raw_data, container->m_light.size, size_px, &cfg, glyph_ranges);
			break;
		}
	}

	//
	// load emojis and merge with previous font
	//
	cfg.OversampleH = cfg.OversampleV = 1;
	cfg.MergeMode = true;
	cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;

	// glyph ranges for standard emojis such as: "😀😃🤗😌😨🥺" etc...
	// see https://en.wikipedia.org/wiki/Emoticons_(Unicode_block)
	// and https://unicode.org/charts/PDF/U1F600.pdf
	//
	// NOTE that building this font is _EXTREMELY_ performance expensive, even on retail builds.
	//		Increasing the glyph range only slightly will have a major impact on performance.
	//
	// NOTE that the standard range of emojis doesn't include other emojis such as "🤐🤑🤒🤓🤔"
	// see https://en.wikipedia.org/wiki/Supplemental_Symbols_and_Pictographs
	static const ImWchar glyph_ranges_emoji[] =
	{
		0x1F600, 0x1F64F, // emojis
		0,
	};

	io.Fonts->AddFontFromMemoryCompressedTTF(g_seguiemj_compressed_data, g_seguiemj_compressed_size, size_px, &cfg, glyph_ranges_emoji);

	// the function shouldn't fail, but anyways
	assert(added_font && "AddFontFromMemoryCompressedTTF failed while adding font.");

	m_precached_fonts[std::make_tuple(id, decor, size_px)] = added_font;

	CConsole::the().info("Added font '{}' ({}) of size {}px to the list.", g_font_filenames[id], g_font_decors[decor], size_px);

	return added_font;
}
