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

#ifndef IUSERINPUT_H
#define IUSERINPUT_H
#pragma once

#include <functional>

static constexpr size_t k_key_range = 0x100;

class UserKey_t;
typedef std::function<void(UserKey_t*)> UserKeyPressCallbackFn;

class UserKey_t
{
public:
	UserKey_t()
	{
	}

	UserKey_t(const std::string name)
		: m_name(name)
	{
	}

	void update(bool dn)
	{
		m_is_down = dn;

		if (m_is_down && m_press_lock == false)
		{
			m_press_lock = true;
			m_pressed_timestamp = std::chrono::high_resolution_clock::now();
			execute_on_pressed_callbacks();
		}

		if (!m_is_down && m_press_lock == true)
		{
			m_press_lock = false;
			execute_on_unpressed_callbacks();
		}
	}

	// time in seconds
	inline float time_down() const { return std::chrono::duration<float, std::ratio<1, 1>>(std::chrono::high_resolution_clock::now() - m_pressed_timestamp).count(); }

	inline bool is_down() const { return m_is_down; }

	void add_on_pressed_callback(const UserKeyPressCallbackFn& c)
	{
		m_on_pressed_callbacks.push_back(c);
	}

	void add_on_unpressed_callback(const UserKeyPressCallbackFn& c)
	{
		m_on_unpressed_callbacks.push_back(c);
	}

private:
	void execute_on_pressed_callbacks()
	{
		for (auto& c : m_on_pressed_callbacks)
		{
			c(this);
		}
	}

	void execute_on_unpressed_callbacks()
	{
		for (auto& c : m_on_unpressed_callbacks)
		{
			c(this);
		}
	}

private:
	bool m_is_down = false;

	std::string m_name = "unknown";

	bool m_toggle_state = false, m_press_lock = false;
	std::chrono::high_resolution_clock::time_point m_pressed_timestamp;

	std::vector<UserKeyPressCallbackFn> m_on_pressed_callbacks;
	std::vector<UserKeyPressCallbackFn> m_on_unpressed_callbacks;
};

class IUserInput : public IBaseInterface
{
public:
	virtual bool initialize() = 0;
	virtual void destroy() = 0;

	// cannot return nullptr however, if not found, valid_key is set to false.
	virtual UserKey_t& get_key(int virtual_key, bool* valid_key = NULL) = 0;

	virtual bool add_key_press_callback(int virtual_key, const UserKeyPressCallbackFn& callback) = 0;
	virtual bool add_key_unpress_callback(int virtual_key, const UserKeyPressCallbackFn& callback) = 0;
};

extern IUserInput* g_user_input_i;

#define IUSERINPUT_INTERFACEID "IUserInput"

#endif // IUSERINPUT_H