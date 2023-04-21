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

IUserInput* g_user_input_i = nullptr;

class CUserInput : public IUserInput
{
public:
	CUserInput();
	~CUserInput();

	bool initialize();
	void destroy();

	UserKey_t& get_key(int virtual_key, bool* valid_key = NULL);

	bool add_key_press_callback(int virtual_key, const UserKeyPressCallbackFn& callback);
	bool add_key_unpress_callback(int virtual_key, const UserKeyPressCallbackFn& callback);

private:
	void update_keys(UINT uMsg, WPARAM wParam);

private:
	std::map<int, UserKey_t> m_userkeys;
};

CUserInput g_user_input;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CUserInput, IUserInput, IUSERINPUT_INTERFACEID, g_user_input);

//-------------------------------------------------------------------------------
//
// CUserInput implementation
//
//-------------------------------------------------------------------------------

CUserInput::CUserInput()
{
	g_user_input_i = this;
}

CUserInput::~CUserInput()
{
	g_user_input_i = nullptr;
}

bool CUserInput::initialize()
{
	// add user input handler to the window proc
	g_window_msg_handler_i->add_msg_callback(
		[this](UINT uMsg, WPARAM wParam)
		{
			if (wParam < k_key_range)
			{
				update_keys(uMsg, wParam);
			}
		});

	for (size_t i = 0; i < k_key_range; i++)
	{
		m_userkeys.insert(std::make_pair(i, CGenericUtil::the().virtual_key_to_string(i)));
	}

	CConsole::the().info("Registered {} keys.", k_key_range);

	CConsole::the().info("Initialized User Input.");
	return true;
}

void CUserInput::destroy()
{
	CConsole::the().info("Destroying User Input...");

	// TODO: Stuff
}

UserKey_t& CUserInput::get_key(int virtual_key, bool* valid_key)
{
	assert(virtual_key >= 0 && virtual_key < k_key_range);

	if (valid_key)
		*valid_key = false;

	for (auto& [num, key] : m_userkeys)
	{
		if (num == virtual_key)
		{
			if (valid_key)
				*valid_key = true;
			return key;
		}
	}

	// this gets returned if it gets called before initialization. This is fine, since we have to initialize this after
	// we have the window handle..
	static UserKey_t dummy;
	return dummy;
}

bool CUserInput::add_key_press_callback(int virtual_key, const UserKeyPressCallbackFn& callback)
{
	bool found;
	auto& key = get_key(virtual_key, &found);
	if (!found)
	{
		return false;
	}

	key.add_on_pressed_callback(callback);
	CConsole::the().info("Added key press callback to virtual key {}", virtual_key);
	return true;
}

bool CUserInput::add_key_unpress_callback(int virtual_key, const UserKeyPressCallbackFn& callback)
{
	bool found;
	auto& key = get_key(virtual_key, &found);
	if (!found)
	{
		return false;
	}

	key.add_on_unpressed_callback(callback);
	CConsole::the().info("Added key unpress callback to virtual key {}", virtual_key);
	return true;
}

void CUserInput::update_keys(UINT uMsg, WPARAM wParam)
{
	if (wParam == NULL)
	{
		return;
	}

	bool is_down = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);

	m_userkeys[wParam].update(is_down);
}

