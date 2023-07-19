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

#ifndef UIMENUSTYLE_H
#define UIMENUSTYLE_H
#pragma once

//
// constants that describe some of the paddings and margins used in the menu
//
class MenuStyle
{
public:
	static const float		rounding_factor;
	static const Vector2D	menu_rect_size;
	static const Vector2D	menu_rect_size_max;

	static const float		tab_select_width;
	static const Vector2D	tab_select_pos;

	static const float		top_region_size_h;

	// menu contents paddings
	static const float		menu_contents_padding;

	// menu tab sections
	static const Vector2D 	tab_section_button_size;
	static const Vector2D 	tab_section_button_padding;
	static const Vector2D 	tab_section_button_inner_padding;

	// bottom portion of the menu
	static const float		bottom_reserved_rect_h;
	static const float		bottom_right_timestamp_rightside_padding;
	static const float		bottom_right_timestamp_bottomside_padding;
	static const Vector2D	bottom_left_desc_padding;

	// menu children
	static const float		child_rounding;
	static const Vector2D	child_contents_padding;
	static const float		child_width;

	// buttons
	static const Vector2D	unload_button_padding;
	static const Vector2D	unload_button_size;
	static const float		about_button_spacing_x;
	static const Vector2D	about_button_size;
	static const float		top_rightside_decoration_width; // just a helper

	static const float		search_dialog_padding_width;
	static const float		search_dialog_spacing_right;

	// children headers
	static const float		childhdr_text_padding_x;
	static const float		childhdr_text_padding_y;
	static const Vector2D	childhdr_line_padding;
	static const float		childhdr_contents_padding_y;

	// utility functions to calculate child size based on height. Width is a constant.
	static Vector2D			calc_child_size(float height);
	static Vector2D			calc_child_size_n(float n, float height);

	static float			get_child_width_w_padding();
};

#endif // UIMENUSTYLE_H