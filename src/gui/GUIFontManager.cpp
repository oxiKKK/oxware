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

#include "assets/fonts/compressed_font_data.h"

#include <imgui_internal.h>

#include <set>

// defines glypth ranges for fonts
class CGlyphRanges
{
public:
	DECL_BASIC_CLASS(CGlyphRanges);

public:
	inline ImWchar* get_range_for_regular_font()
	{
		return s_regular_font_range;
	}

	inline ImWchar* get_range_for_emojis()
	{
		return s_emojis_range;
	}

private:
	static ImWchar s_regular_font_range[];
	static ImWchar s_emojis_range[];
};

ImWchar CGlyphRanges::s_regular_font_range[] =
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

// glyph ranges for standard emojis such as: "😀😃🤗😌😨🥺" etc...
// see https://en.wikipedia.org/wiki/Emoticons_(Unicode_block)
// and https://unicode.org/charts/PDF/U1F600.pdf
//
// NOTE that building this font is _EXTREMELY_ performance expensive, even on retail builds.
//		Increasing the glyph range only slightly will have a major impact on performance.
//
// NOTE that the standard range of emojis doesn't include other emojis such as "🤐🤑🤒🤓🤔"
// see https://en.wikipedia.org/wiki/Supplemental_Symbols_and_Pictographs
ImWchar CGlyphRanges::s_emojis_range[] =
{
	0x1F600, 0x1F64F, // standard emoticons
//	0x1F900, 0x1F9FF, // supplemental symbols
	0x1F550, 0x1F55B, // clock emojis (🕐, 🕑, 🕕, 🕗)
	U'🔥', U'🔥',	  // fire
	U'⏳', U'⏳',	  // sand clock
	U'🌈', U'🌈',	  // rainbow
	U'🥳', U'🥳',	  // celebration
	U'⌨', U'⌨',	  // keyboard
	U'🏃', U'🏃',	  // man running
	U'🌎', U'🌎',	  // globe
	U'⛈', U'⛈',	  // thunderstorm
	U'❌', U'❌',	  // rotated cross
	U'❔', U'❔',	  // question mark
	U'🔎', U'🔎',	  // right-facing magnifier glass
	U'🖥', U'🖥',	  // computer screen
	U'📼', U'📼',	  // recording tape
	U'💀', U'💀',	  // skull
	U'🔳', U'🔳',	  // empty rectangle with outline
	U'➕', U'➕',	  // crosshair
	U'⚙', U'⚙',	  // cog
	U'📂', U'📂',	  // file
	U'👈', U'👈',	  // left-pointing finger
	U'👆', U'👆',	  // up-pointing finger
	U'🧐', U'🧐',	  // 
	U'🐇', U'🐇',	  // bunny
	U'👥', U'👥',	  // 2x gray person
	U'🐞', U'🐞',	  // beetle
	U'🕶', U'🕶',	  // sunglasses
	0,
};

//----------------------------------------------------------------------------------------------

// font identifier that is unique for this set of parameters such as id, decoration, or size.
using FontIdentifier_t = std::tuple<EFontId, EFontDecoration, EFontSize>;

MAKE_HASHABLE(FontIdentifier_t, std::get<0>(t), std::get<1>(t), std::get<1>(t));

//----------------------------------------------------------------------------------------------

IGUIFontManager* g_gui_fontmgr_i = nullptr;

class CGUIFontManager : public IGUIFontManager
{
public:
	CGUIFontManager();
	~CGUIFontManager();

	bool pre_newframe();

	void initialize();

	void push_default_font();
	void pop_default_font();

	ImFont* get_font(EFontId id, EFontSize size, EFontDecoration decor = FDC_Regular);
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

	void render_debug_ui();

private:
	void precache_all_fonts();
	ImFont* precache_font(EFontId id, EFontSize size_id, EFontDecoration decor);
	void merge_emoji_font(ImFontConfig* cfg);
	
	void rebuild_font_atlas();
	
private:
	std::unordered_map<FontIdentifier_t, ImFont*> m_precached_fonts;

	ImFont* m_default_font = nullptr;

	bool m_needs_font_rebuild = false;
	FreeTypeBuilderFlags m_freetype_builder_flags;
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
	// someone requested font atlas rebuild
	if (m_needs_font_rebuild)
	{
		rebuild_font_atlas();
		m_needs_font_rebuild = false;
		return true;
	}

	return false;
}

void CGUIFontManager::initialize()
{
	precache_all_fonts();

	rebuild_font_atlas();

	m_default_font = get_font(FID_SegoeUI, FSZ_16px, FDC_Bold);
}

void CGUIFontManager::rebuild_font_atlas()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	// re-build the atlas
	// NOTE that this is very performance expensive.
	ImFontAtlas* atlas = ImGui::GetIO().Fonts;
	atlas->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
	atlas->FontBuilderFlags = m_freetype_builder_flags;
	atlas->Build();

	auto t2 = std::chrono::high_resolution_clock::now();

	CConsole::the().info("Took {} seconds to rebuild all fonts.",
						 std::chrono::duration<float, std::ratio<1, 1>>(t2 - t1).count());
}

void CGUIFontManager::render_debug_ui()
{
	if (g_gui_widgets_i->add_tree_node("Info"))
	{
		g_gui_widgets_i->add_text(std::format("Fonts precached: {}", m_precached_fonts.size()));
		
		g_gui_widgets_i->pop_tree_node();
	}

	if (g_gui_widgets_i->add_tree_node("Sizes"))
	{
		for (auto [id, font] : m_precached_fonts)
		{
			g_gui_widgets_i->add_text(std::format("{:<2}px: 😑", font->FontSize), TEXTPROP_None, font);
		}

		g_gui_widgets_i->pop_tree_node();
	}

	if (g_gui_widgets_i->add_tree_node("Atlas"))
	{
		g_gui_widgets_i->show_font_atlas();
		g_gui_widgets_i->pop_tree_node();
	}
}

void CGUIFontManager::push_default_font()
{
	ImGui::PushFont(m_default_font);
}

void CGUIFontManager::pop_default_font()
{
	ImGui::PopFont();
}

ImFont* CGUIFontManager::get_font(EFontId id, EFontSize size, EFontDecoration decor)
{
	auto iter = m_precached_fonts.find(std::make_tuple(id, decor, size));

	assert(iter != m_precached_fonts.end() && "You tried to get a font that doesn't exist.");
	
	// if the font hasn't been found, return the default font instead. While debugging, we will catch this.
	return iter->second == nullptr ? m_default_font : iter->second;
}

ImFont* CGUIFontManager::get_default_font()
{
	return m_default_font;
}

Vector2D CGUIFontManager::calc_font_text_size(ImFont* font, const char* text)
{
	return font->CalcTextSizeA(font->FontSize, FLT_MAX, 0, text);
}

static BaseFontContainer_t* find_static_font_container(const char* name)
{
	for (auto& f : g_static_fonts)
	{
		if (!stricmp(name, f.m_name))
		{
			return &f;
		}
	}
	assert(0 && "Font not found inside static container!");
	return nullptr;
}

void CGUIFontManager::precache_all_fonts()
{
	CConsole::the().info("Precaching fonts...");

	auto t1 = std::chrono::high_resolution_clock::now();
	
	// This stuff is really performance expensive.

#if 0
	//
	// HARDCORE APPROACH: precache all fonts, one by one, including all of their decorations and sizes.
	//
	for (int id = 0; id < NUM_FONTS; id++)
	{
		for (int decor = 0; decor < NUM_FONT_DECORATIONS; decor++)
		{
			for (int size = 0; size < FSZ_COUNT; size++)
			{
				precache_font((EFontId)id, s_EFontSizeToFloat[size], (EFontSize)size, (EFontDecoration)decor);
			}
		}
	}
#else
	//
	// MANUAL APPROACH: manually precache fonts only that we use in code.
	// 
	// How to search for font variations that are used in code:
	//	- find all occurences of a font size (e.g. FSZ_13px) in the code.
	//	- see which variations (font id and decoration) is being used together in
	//	  combination with this font.
	//	- add all combinations here.
	//
	// NOTE: If you forget to add something, you'll get assertion anyways.
	//

	// segoe
	precache_font(FID_SegoeUI, FSZ_10px, FDC_Bold);
	precache_font(FID_SegoeUI, FSZ_10px, FDC_Regular);

	precache_font(FID_SegoeUI, FSZ_13px, FDC_Bold);
	precache_font(FID_SegoeUI, FSZ_13px, FDC_Regular);

	precache_font(FID_SegoeUI, FSZ_14px, FDC_Regular);
	precache_font(FID_SegoeUI, FSZ_14px, FDC_Bold);

	precache_font(FID_SegoeUI, FSZ_16px, FDC_Bold);
	precache_font(FID_SegoeUI, FSZ_16px, FDC_Regular);

	precache_font(FID_SegoeUI, FSZ_18px, FDC_Regular);
	precache_font(FID_SegoeUI, FSZ_18px, FDC_Bold);
	precache_font(FID_SegoeUI, FSZ_18px, FDC_Light);

	precache_font(FID_SegoeUI, FSZ_21px, FDC_Light);

	precache_font(FID_SegoeUI, FSZ_24px, FDC_Regular);
	precache_font(FID_SegoeUI, FSZ_24px, FDC_Light);

	precache_font(FID_SegoeUI, FSZ_27px, FDC_Regular);

	precache_font(FID_SegoeUI, FSZ_30px, FDC_Bold);
	precache_font(FID_SegoeUI, FSZ_30px, FDC_Regular);

	precache_font(FID_SegoeUI, FSZ_33px, FDC_Bold);

	precache_font(FID_SegoeUI, FSZ_45px, FDC_Bold);

	// proggy
	precache_font(FID_ProggyClean, FSZ_13px, FDC_Bold);
	precache_font(FID_ProggyClean, FSZ_13px, FDC_Regular);

	precache_font(FID_ProggyClean, FSZ_16px, FDC_Regular);
#endif

	auto t2 = std::chrono::high_resolution_clock::now();

	CConsole::the().info("Precached all {} fonts. Took {} seconds.",
						 m_precached_fonts.size(),
						 std::chrono::duration<float, std::ratio<1, 1>>(t2 - t1).count());
}

ImFont* CGUIFontManager::precache_font(EFontId id, EFontSize size_id, EFontDecoration decor)
{
	auto& io = ImGui::GetIO();

	auto container = find_static_font_container(g_font_filenames[id]);
	assert(container && "The container for this font doesn't exist!");

	float size_px = s_EFontSizeToFloat[size_id];

	ImFontConfig cfg;
	cfg.SizePixels = size_px;

	// For smaller fonts, setting this throws out better results.
	if (size_px <= 12)
	{
		cfg.GlyphBuildFlags |= ImGuiFreeTypeGlyphBuildFlags_ForceAutoHint;
	}

	auto normal_ranges = CGlyphRanges::the().get_range_for_regular_font();

	ImFont* added_font = nullptr;
	switch (decor)
	{
		case FDC_Regular:
		{
			if (!container->m_regular.raw_data)
			{
				return NULL;
			}
			added_font = io.Fonts->AddFontFromMemoryCompressedTTF(container->m_regular.raw_data, container->m_regular.size, size_px, &cfg, normal_ranges);
			break;
		}
		case FDC_Bold:
		{
			if (!container->m_bold.raw_data)
			{
				return NULL;
			}
			added_font = io.Fonts->AddFontFromMemoryCompressedTTF(container->m_bold.raw_data, container->m_bold.size, size_px, &cfg, normal_ranges);
			break;
		}
		case FDC_Light:
		{
			if (!container->m_light.raw_data)
			{
				return NULL;
			}
			added_font = io.Fonts->AddFontFromMemoryCompressedTTF(container->m_light.raw_data, container->m_light.size, size_px, &cfg, normal_ranges);
			break;
		}
	}

	// load emojis and merge with previous font
	merge_emoji_font(&cfg);

	m_precached_fonts[std::make_tuple(id, decor, size_id)] = added_font;

	CConsole::the().info("Font added: '{}', decoration: {}, size: {} px", g_font_filenames[id], g_font_decors[decor], size_px);

	return added_font;
}

// merge emoji font into current one
void CGUIFontManager::merge_emoji_font(ImFontConfig* cfg)
{
	auto emojis_ranges = CGlyphRanges::the().get_range_for_emojis();
	auto& io = ImGui::GetIO();

	// setup config
	cfg->OversampleH = cfg->OversampleV = 1;
	cfg->MergeMode = true; // merge into existing font
	cfg->FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;

	// make emojis slightly smaller, because usually they're bigger than acutal text.
	cfg->SizePixels *= 0.75f;

	io.Fonts->AddFontFromMemoryCompressedTTF(g_seguiemj_compressed_data, g_seguiemj_compressed_size, cfg->SizePixels, cfg, emojis_ranges);
}

//----------------------------------------------------------------------------------------------

BaseCommand ui_reload_fonts(
	"ui_reload_fonts", "Rebuilds all of the fonts.",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		g_gui_fontmgr_i->rebuild_fonts();
	}
);
