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

#ifndef UIINCOMMANDBINDING_H
#define UIINCOMMANDBINDING_H
#pragma once

// to keep track of the UI element representing a single incommand.
struct ui_incmd_element_t
{
	// flags
	bool f_disable_ingame_key;
};

class CUIInCommandKeyBinding
{
public:
	DECL_BASIC_CLASS(CUIInCommandKeyBinding);

public:
	void update();

	void render_interactible_list();

	void add_keyscan_button(BaseInCommand* in_cmd, const Vector2D& size);

private:
	void setup_interactible_list();

	void end_key_binding_mode(int vk_pressed);

	BaseInCommand* m_in_cmd_to_be_rebound;

	// key is the name of the incommand
	std::unordered_map<std::string, ui_incmd_element_t> m_ui_elements;

	ui_incmd_element_t* retreive_ui_element_or_insert_new(BaseInCommand* in_cmd);
	ui_incmd_element_t* insert_new_ui_element(BaseInCommand* in_cmd);

	void keep_ui_elements_up_to_date();
};

#endif // UIINCOMMANDBINDING_H