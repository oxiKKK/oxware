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

#include "Console.h"

#include <functional>

static constexpr size_t k_key_range = 0x100;

class UserKey_t;
typedef std::function<void(const std::string&, UserKey_t*)> UserKeyPressCallbackFn;

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
			m_pressed_timestamp_ms = GetTickCount();
			execute_on_pressed_callbacks();
		}
		else if (!m_is_down && m_press_lock == true)
		{
			m_press_lock = false;
			execute_on_unpressed_callbacks();
		}
	}

	//-----------------------------------------------

	// time in seconds
	inline uint32_t time_down() const { return (GetTickCount() - m_pressed_timestamp_ms) * 1000; }
	inline bool is_down() const { return m_is_down; }
	inline const std::string& get_name() const { return m_name; }

	//-----------------------------------------------

	bool on_pressed_callback_exists(const std::string& id)
	{
		return m_on_pressed_callbacks.contains(id);
	}

	bool on_unpressed_callback_exists(const std::string& id)
	{
		return m_on_unpressed_callbacks.contains(id);
	}

	//-----------------------------------------------

	void add_on_pressed_callback(const std::string& id, const UserKeyPressCallbackFn& c)
	{
		m_on_pressed_callbacks.insert({ id, c });
	}

	void add_on_unpressed_callback(const std::string& id, const UserKeyPressCallbackFn& c)
	{
		m_on_unpressed_callbacks.insert({ id, c });
	}

	//-----------------------------------------------
	
	void remove_on_pressed_callback(const std::string& id)
	{
		try
		{
			m_on_pressed_callbacks.erase(id);
		}
		catch (...)
		{
			CConsole::the().error("Couldn't remove on-pressed callback '{}'", id);
		}
	}

	void remove_on_unpressed_callback(const std::string& id)
	{
		try
		{
			m_on_unpressed_callbacks.erase(id);
		}
		catch (...)
		{
			CConsole::the().error("Couldn't remove on-unpressed callback '{}'", id);
		}
	}

	//-----------------------------------------------

private:
	void execute_on_pressed_callbacks()
	{
		for (auto& [id, c] : m_on_pressed_callbacks)
		{
			c(id, this);
		}
	}

	void execute_on_unpressed_callbacks()
	{
		for (auto& [id, c] : m_on_unpressed_callbacks)
		{
			c(id, this);
		}
	}

private:
	bool m_is_down = false;

	std::string m_name = "unknown";

	bool m_toggle_state = false, m_press_lock = false;
	uint32_t m_pressed_timestamp_ms;

	// every callback has an id
	std::unordered_map<std::string, UserKeyPressCallbackFn> m_on_pressed_callbacks;
	std::unordered_map<std::string, UserKeyPressCallbackFn> m_on_unpressed_callbacks;
};

class IUserInput : public IBaseInterface
{
public:
	virtual bool initialize() = 0;
	virtual void destroy() = 0;

	virtual bool is_valid_key(int virtual_key) = 0;

	// cannot return nullptr however, if not found, valid_key is set to false.
	virtual UserKey_t& get_key(int virtual_key, bool* valid_key = NULL) = 0;

	virtual std::string virtual_key_to_string(int virtual_key) = 0;
	virtual int string_to_virtual_key(const std::string& key_name) = 0;
	virtual int engine_key_to_virtual_key(int engine_key) = 0;

	virtual bool add_key_press_callback(const std::string& id, int virtual_key, const UserKeyPressCallbackFn& callback) = 0;
	virtual bool add_key_unpress_callback(const std::string& id, int virtual_key, const UserKeyPressCallbackFn& callback) = 0;

	virtual bool remove_key_press_callback(const std::string& id, int virtual_key) = 0;
	virtual bool remove_key_unpress_callback(const std::string& id, int virtual_key) = 0;

	virtual void activate_key_binding_mode() = 0;
	virtual bool is_in_key_binding_mode() = 0;
	virtual int get_bound_key() const = 0;
	virtual void reset_bound_key() = 0;

	virtual void for_all_user_keys(const std::function<void(UserKey_t*)>& callback) = 0;
};

extern IUserInput* g_user_input_i;

#define IUSERINPUT_INTERFACEID "IUserInput"

#endif // IUSERINPUT_H