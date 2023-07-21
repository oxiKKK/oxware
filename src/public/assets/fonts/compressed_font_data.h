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

#ifndef COMPRESSED_FONT_DATA_H
#define COMPRESSED_FONT_DATA_H
#pragma once

#include <vector>

struct FontPair_t
{
	unsigned int size;
	unsigned int* raw_data;
};

struct BaseFontContainer_t;
void add_font_to_static_container(BaseFontContainer_t* basefont);

struct BaseFontContainer_t
{
	BaseFontContainer_t(const char* name, FontPair_t regular, FontPair_t bold, FontPair_t light) :
		m_name(name), m_regular(regular), m_bold(bold), m_light(light)
	{
		add_font_to_static_container(this); // HACKHACK
	}

	FontPair_t m_regular, m_bold, m_light;
	const char* m_name;
};

extern std::vector<BaseFontContainer_t> g_static_fonts;

//
// ProggyClean monospace
//
extern unsigned int g_proggyclean_compressed_size;
extern unsigned int g_proggyclean_compressed_data[9584 / 4];
struct ProggyCleanFontContainer_t : public BaseFontContainer_t
{
public:
	ProggyCleanFontContainer_t(const char* name, FontPair_t regular, FontPair_t bold, FontPair_t light) :
		BaseFontContainer_t(name, regular, bold, light)
	{
	}
};

//
// SegoeUI
//
extern unsigned int g_segoeui_compressed_size;
extern unsigned int g_segoeui_compressed_data[322720 / 4];
extern unsigned int g_segoeuib_compressed_size;
extern unsigned int g_segoeuib_compressed_data[688080 / 4];
extern unsigned int g_segoeuil_compressed_size;
extern unsigned int g_segoeuil_compressed_data[642404 / 4];
extern unsigned int g_seguiemj_compressed_size;
extern unsigned int g_seguiemj_compressed_data[1405400 / 4];
struct SegoeUIFontContainer_t : public BaseFontContainer_t
{
public:
	SegoeUIFontContainer_t(const char* name, FontPair_t regular, FontPair_t bold, FontPair_t light) :
		BaseFontContainer_t(name, regular, bold, light)
	{
	}
};

#endif