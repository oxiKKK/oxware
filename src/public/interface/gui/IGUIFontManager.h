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

enum EFontDecoration
{
	FDC_Regular,
	FDC_Bold,
	FDC_Light,

	NUM_FONT_DECORATIONS
};

static const char* g_font_decors[NUM_FONT_DECORATIONS] =
{
	"regular",	// FDC_Regular
	"bold",		// FDC_Bold
	"light",	// FDC_Light
};

enum EFontId
{
	FID_SegoeUI,
	FID_ProggyClean,
	// if you add more fonts, don't forget to modify g_font_filenames[], too!

	NUM_FONTS,
};

static const char* g_font_filenames[NUM_FONTS] =
{
	"segoeui",			// FID_SegoeUI
	"proggyclean",		// FID_ProggyClean
};

#ifdef small
#undef small // bruh
#endif

inline static constexpr int k_min_font_size = 5;
inline static constexpr int k_max_font_size = 60;
//inline static constexpr int k_step = 3;
// we'll precache k_max_font_size - k_min_font_size at startup for each decoration and for each font

struct ft_size
{
	consteval ft_size(int _l) : l(_l) { }
	consteval operator int() const { return l; }

	inline consteval int smallest(float scale = 1.0f) const		{ return constexpr_floor(((float)l * 0.65f) * scale); }
	inline consteval int small(float scale = 1.0f) const		{ return constexpr_floor(((float)l * 0.75f) * scale); }
	inline consteval int medium(float scale = 1.0f) const		{ return constexpr_floor(((float)l * 0.85f) * scale); }
	inline consteval int regular(float scale = 1.0f) const		{ return constexpr_floor(((float)l * 1.00f) * scale); }
	inline consteval int bigger(float scale = 1.0f) const		{ return constexpr_floor(((float)l * 1.25f) * scale); }
	inline consteval int big(float scale = 1.0f) const			{ return constexpr_floor(((float)l * 1.45f) * scale); }
	inline consteval int large(float scale = 1.0f) const		{ return constexpr_floor(((float)l * 1.65f) * scale); }
	inline consteval int extra(float scale = 1.0f) const		{ return constexpr_floor(((float)l * 1.85f) * scale); }

	int l;

private:
	constexpr int constexpr_floor(float f) const
	{
		const int i = static_cast<int>(f);
		return f < i ? i - 1 : i;
	}
};

struct FontSize
{
private:
	template<int size>
	inline static consteval int validate()
	{
		// see if the size is within bounds.
		static_assert((int)((float)size * 0.65f) >= k_min_font_size && (int)((float)size * 1.85f) <= k_max_font_size,
					  "the font size is too big.");
		return size;
	}

public:

	// UI
	inline static constexpr ft_size UIText = validate<16>();

	// Debug
	inline static constexpr ft_size Debug = validate<14>();
};

// wrapper over the enum in imgui so that we don't have to include imgui outside of UI code.
enum FreeTypeBuilderFlags
{
	FreeTypeBuilderFlags_None = 0,
	FreeTypeBuilderFlags_NoHinting = 1 << 0,		// Disable hinting. This generally generates 'blurrier' bitmap glyphs when the glyph are rendered in any of the anti-aliased modes.
	FreeTypeBuilderFlags_NoAutoHint = 1 << 1,		// Disable auto-hinter.
	FreeTypeBuilderFlags_ForceAutoHint = 1 << 2,	// Indicates that the auto-hinter is preferred over the font's native hinter.
	FreeTypeBuilderFlags_LightHinting = 1 << 3,		// A lighter hinting algorithm for gray-level modes. Many generated glyphs are fuzzier but better resemble their original shape. This is achieved by snapping glyphs to the pixel grid only vertically (Y-axis), as is done by Microsoft's ClearType and Adobe's proprietary font renderer. This preserves inter-glyph spacing in horizontal text.
	FreeTypeBuilderFlags_MonoHinting = 1 << 4,		// Strong hinting algorithm that should only be used for monochrome output.
	FreeTypeBuilderFlags_Bold = 1 << 5,				// Styling: Should we artificially embolden the font?
	FreeTypeBuilderFlags_Oblique = 1 << 6,			// Styling: Should we slant the font, emulating italic style?
	FreeTypeBuilderFlags_Monochrome = 1 << 7,		// Disable anti-aliasing. Combine this with MonoHinting for best results!
	FreeTypeBuilderFlags_LoadColor = 1 << 8,		// Enable FreeType color-layered glyphs
	FreeTypeBuilderFlags_Bitmap = 1 << 9			// Enable FreeType bitmap glyphs
};
DEFINE_ENUM_BITWISE_OPERATORS(FreeTypeBuilderFlags);

class IGUIFontManager
{
public:
	virtual void initialize() = 0;

	// call before ImGui::NewFrame(). Returns true if atlas was rebuilt
	virtual bool pre_newframe() = 0;

	virtual void push_default_font() = 0;
	virtual void pop_default_font() = 0;

	virtual ImFont* get_font(EFontId id, float size_px, EFontDecoration decor = FDC_Regular) = 0;
	virtual ImFont* get_default_font() = 0;

	virtual Vector2D calc_font_text_size(ImFont* font, const char* text) = 0;

	virtual void rebuild_fonts() = 0;

	virtual void add_freetype_builder_flags(FreeTypeBuilderFlags f, bool set) = 0;
	virtual FreeTypeBuilderFlags get_freetype_builder_flags() = 0;
};

extern IGUIFontManager* g_gui_fontmgr_i;

#define IGUIFONTMANAGER_INTERFACEID "IGUIFontManager"

#endif // IGUIFONTMANAGER_H