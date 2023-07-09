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

#ifndef UIKEYBINDING_H
#define UIKEYBINDING_H
#pragma once

// buffers for every key we have
static constexpr int buffer_len = 4096;
struct bound_key_t
{
	std::array<char, buffer_len> cmd_0, cmd_1;
	int state;

	// flags
	bool f_execute_over_ui, f_execute_over_game_ui, f_silent, f_disable_ingame_key;
	bool has_two_cmds;
};

class CUIKeyBinding
{
public:
	DECL_BASIC_CLASS(CUIKeyBinding);

public:
	void render_interactible_bind_list();
	void update();

private:
	void render_list();
	void render_decoration(const Vector2D& last_cursor_pos);

	// command text input box
	void render_command_textinput(bound_key_t* bound_key, int vk, const std::string& key_name);
	static int command_texinput_callback(ImGuiInputTextCallbackData* data);

	void add_keyscan_button(const std::string& label, int vk, const Vector2D& size, bool* force_binding_mode = nullptr);

	void setup_interactible_bind_list();

	void keep_bound_keys_up_to_date();
	std::unordered_map<int, bound_key_t> m_bound_keys;

	bound_key_t* retreive_bound_key_or_insert_new(int vk, const bind_t& bind);
	bound_key_t* insert_new_key_bind(int vk, const bind_t& bind);

	struct key_scan_button_info_t
	{
		std::string label = "<new key>";
		int prev_vk = 0;
	};
	key_scan_button_info_t* m_current_key_bound = nullptr;

	using fnOnKeyBoundCallback = std::function<void(key_scan_button_info_t* info, int vk)>;
	std::unordered_map<std::string, key_scan_button_info_t> m_key_scan_button_info;
	void update_keyscan_button_title(int new_vk);

	void switch_to_key_binding_mode(key_scan_button_info_t* info,
									const fnOnKeyBoundCallback& on_key_bound_callback);
	void end_key_binding_mode(int vk_pressed);

	void insert_new_key_scan_button_info(int vk);

	fnOnKeyBoundCallback m_on_key_bound_callback;
};

#endif // UIKEYBINDING_H