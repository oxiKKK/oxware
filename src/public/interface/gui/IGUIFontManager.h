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

#include <array>

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

// how big is the smallest font, this serves as a base when we're calculating font sizes
inline static constexpr int k_font_size_base = 10;
enum EFontSize
{
	FSZ_10px,
	FSZ_13px,
	FSZ_14px,
	FSZ_16px,
	FSZ_18px,
	FSZ_21px,
	FSZ_24px,
	FSZ_27px,
	FSZ_30px,
	FSZ_33px,
	FSZ_45px,

	FSZ_COUNT
};

inline static constexpr std::array<float, FSZ_COUNT> s_EFontSizeToFloat =
{
	/*FSZ_10px*/ 10.0f,
	/*FSZ_13px*/ 13.0f,
	/*FSZ_14px*/ 14.0f,
	/*FSZ_16px*/ 16.0f,
	/*FSZ_20px*/ 20.0f,
	/*FSZ_21px*/ 21.0f,
	/*FSZ_24px*/ 24.0f,
	/*FSZ_27px*/ 27.0f,
	/*FSZ_30px*/ 30.0f,
	/*FSZ_33px*/ 33.0f,
	/*FSZ_45px*/ 45.0f,
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

	virtual ImFont* get_font(EFontId id, EFontSize size, EFontDecoration decor = FDC_Regular) = 0;
	virtual ImFont* get_default_font() = 0;

	virtual Vector2D calc_font_text_size(ImFont* font, const char* text) = 0;

	virtual void rebuild_fonts() = 0;

	virtual void add_freetype_builder_flags(FreeTypeBuilderFlags f, bool set) = 0;
	virtual FreeTypeBuilderFlags get_freetype_builder_flags() = 0;

	virtual void render_debug_ui() = 0;
};

extern IGUIFontManager* g_gui_fontmgr_i;

#define IGUIFONTMANAGER_INTERFACEID "IGUIFontManager"

#endif // IGUIFONTMANAGER_H