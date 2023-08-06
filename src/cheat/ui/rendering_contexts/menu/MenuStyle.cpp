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

// Note: moved here from the header file for faster compile times
const float MenuStyle::rounding_factor = 20.0f;

const Vector2D MenuStyle::menu_rect_size = { 600, 420 };
const Vector2D MenuStyle::menu_rect_size_max = menu_rect_size * Vector2D(1.77f, 1.75f); // sized so that four child columns fit inside contents

const float MenuStyle::tab_select_width = 120.0f;
const Vector2D MenuStyle::tab_select_pos = { 10.0f, 50.0f };

const float MenuStyle::top_region_size_h = 50.0f;

// menu contents paddings
const float MenuStyle::menu_contents_padding = 5.0f;

// menu tab sections
const Vector2D MenuStyle::tab_section_button_size = { 130.0f, 24.0f };
const Vector2D MenuStyle::tab_section_button_padding = { 5.0f, 3.0f };
const Vector2D MenuStyle::tab_section_button_inner_padding = { 10.0f, 2.0f };

// bottom portion of the menu
const float MenuStyle::bottom_reserved_rect_h = 15.0f;
const float MenuStyle::bottom_right_timestamp_rightside_padding = 5.0f;
const float MenuStyle::bottom_right_timestamp_bottomside_padding = 2.0f;
const Vector2D MenuStyle::bottom_left_desc_padding = { 5.0f, 2.0f };

// menu children
const float MenuStyle::child_rounding = 5.0f;
const Vector2D MenuStyle::child_contents_padding = { 5.0f, 5.0f };

const float MenuStyle::child_width = 225.0f;

const Vector2D MenuStyle::unload_button_padding = { 15.0f, 15.0f };
const Vector2D MenuStyle::unload_button_size = { 30.0f, 20.0f };
const float MenuStyle::about_button_spacing_x = 5.0f;
const Vector2D MenuStyle::about_button_size = { 30.0f, 20.0f };
const float MenuStyle::top_rightside_decoration_width = about_button_size.x + unload_button_size.x + unload_button_padding.x + about_button_spacing_x;

const float MenuStyle::search_dialog_padding_width = 230.0f;
const float MenuStyle::search_dialog_spacing_right = 10.0f; // spacing from the right side

const float MenuStyle::childhdr_text_padding_x = 5.0f;
const float MenuStyle::childhdr_text_padding_y = 1.0f;
const Vector2D MenuStyle::childhdr_line_padding = { 5.0f, 2.0f };
const float MenuStyle::childhdr_contents_padding_y = 5.0f;

Vector2D MenuStyle::calc_child_size(float height)
{
	return { child_width, height };
}

Vector2D MenuStyle::calc_child_size_n(float n, float height)
{
	return { child_width * n + child_contents_padding.x, height };
}

float MenuStyle::get_child_width_w_padding()
{
	return MenuStyle::child_width - MenuStyle::child_contents_padding.x * 2;
}
