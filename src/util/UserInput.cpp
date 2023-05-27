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

struct caseinsens_string_hash
{
	size_t operator()(const std::string& str) const
	{
		const char* s = str.c_str();
		size_t h = 2166136261u;
		for (size_t i = 0; i < str.length(); i++)
		{
			h ^= std::tolower(*s++);
			h *= 16777619u;
		}
		return h;
	}
};

struct caseinsens_string_compare
{
	bool operator()(const std::string& Left, const std::string& Right) const
	{
		return Left.size() == Right.size()
			&& std::equal(Left.begin(), Left.end(), Right.begin(),
			[](char a, char b)
			{
				return tolower(a) == tolower(b);
			}
		);
	}
};

BaseCommand list_registered_key_names(
	"list_registered_key_names", "Prints out all the keys that the cheat subsystems are currently aware of",
	[&](BaseCommand* cmd, const CmdArgs& args)
	{
		size_t n = 0;
		g_user_input_i->for_all_user_keys(
			[&n](UserKey_t* key)
			{
				n++;
				if (!key->get_name().empty())
				{
					CConsole::the().info("0x{:<03X}: {}", n, key->get_name());
				}
			});
	}
);

IUserInput* g_user_input_i = nullptr;

class CUserInput : public IUserInput
{
public:
	CUserInput();
	~CUserInput();

	bool initialize();
	void destroy();

	bool is_valid_key(int virtual_key);

	UserKey_t& get_key(int virtual_key, bool* valid_key = NULL);

	std::string virtual_key_to_string(int virtual_key);
	int string_to_virtual_key(const std::string& key_name);
	int engine_key_to_virtual_key(int engine_key);

	bool add_key_press_callback(const std::string& id, int virtual_key, const UserKeyPressCallbackFn& callback);
	bool add_key_unpress_callback(const std::string& id, int virtual_key, const UserKeyPressCallbackFn& callback);

	bool remove_key_press_callback(const std::string& id, int virtual_key);
	bool remove_key_unpress_callback(const std::string& id, int virtual_key);

	void activate_key_binding_mode()
	{
		reset_bound_key();
		m_key_binding_mode = true;
	}
	bool is_in_key_binding_mode() { return m_key_binding_mode; }
	int get_bound_key() const { return m_any_key_pressed; }
	void reset_bound_key() { m_any_key_pressed = NULL; }

	void for_all_user_keys(const std::function<void(UserKey_t*)>& callback);

private:
	void update_keys(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	std::unordered_map<int, UserKey_t> m_userkeys;

	std::unordered_map<std::string, int, caseinsens_string_hash, caseinsens_string_compare> m_translation_table_string_to_vk;
	std::unordered_map<int, std::string> m_translation_table_vk_to_string;
	std::unordered_map<int, int> m_translation_table_engine_key_to_vk;
	void initialize_key_translation_tables();

	int m_any_key_pressed = NULL;
	bool m_key_binding_mode = false;
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

	initialize_key_translation_tables();
}

CUserInput::~CUserInput()
{
	g_user_input_i = nullptr;
}

bool CUserInput::initialize()
{
	// add user input handler to the window proc
	g_window_msg_handler_i->add_msg_callback(
		[this](UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			if (wParam < k_key_range || 
				// mouse scroll
				uMsg & WM_MOUSEWHEEL || 
				// mouse buttons
				uMsg & WM_LBUTTONDOWN || uMsg & WM_MBUTTONDOWN || uMsg & WM_RBUTTONDOWN || uMsg & WM_XBUTTONDOWN ||
				uMsg & WM_LBUTTONUP || uMsg & WM_MBUTTONUP || uMsg & WM_RBUTTONUP || uMsg & WM_XBUTTONUP
				)
			{
				update_keys(uMsg, wParam, lParam);
			}
		});

	for (size_t i = 0; i < k_key_range; i++)
	{
		m_userkeys.insert(std::make_pair(i, CGenericUtil::the().virtual_key_to_string(i)));
	}

	m_userkeys[VK_MWHEELDOWN] = UserKey_t("mwheeldown");
	m_userkeys[VK_MWHEELUP] = UserKey_t("mwheelup");

	CConsole::the().info("Registered {} keys.", k_key_range);

	CConsole::the().info("Initialized User Input.");
	return true;
}

void CUserInput::destroy()
{
	CConsole::the().info("Destroying User Input...");

	// TODO: Stuff
}

bool CUserInput::is_valid_key(int virtual_key)
{
	return virtual_key >= 0 && virtual_key < k_key_range;
}

UserKey_t& CUserInput::get_key(int virtual_key, bool* valid_key)
{
	assert(is_valid_key(virtual_key) && "tried to get invalid key");

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

std::string CUserInput::virtual_key_to_string(int virtual_key)
{
	try
	{
		return m_translation_table_vk_to_string.at(virtual_key);
	}
	catch (const std::exception&)
	{
		CConsole::the().error("Unrecognized virtual key! '{}'", CGenericUtil::the().virtual_key_to_string(virtual_key));
		return "";
	}
}

int CUserInput::string_to_virtual_key(const std::string& key_name)
{	
	try
	{
		return m_translation_table_string_to_vk.at(key_name);
	}
	catch (const std::exception&)
	{
		CConsole::the().error("Unrecognized key name! '{}'", key_name);
		return NULL;
	}
}

int CUserInput::engine_key_to_virtual_key(int engine_key)
{
	try
	{
		return m_translation_table_engine_key_to_vk.at(engine_key);
	}
	catch (const std::exception&)
	{
		CConsole::the().error("Non-existent engine key! '{}'", engine_key);
		return NULL;
	}
}

bool CUserInput::add_key_press_callback(const std::string& id, int virtual_key, const UserKeyPressCallbackFn& callback)
{
	bool found;
	auto& key = get_key(virtual_key, &found);
	if (!found)
	{
		return false;
	}

	key.add_on_pressed_callback(id, callback);
	CConsole::the().info("Added key press callback to virtual key {}", virtual_key);
	return true;
}

bool CUserInput::add_key_unpress_callback(const std::string& id, int virtual_key, const UserKeyPressCallbackFn& callback)
{
	bool found;
	auto& key = get_key(virtual_key, &found);
	if (!found)
	{
		return false;
	}

	key.add_on_unpressed_callback(id, callback);
	CConsole::the().info("Added key unpress callback to virtual key {}", virtual_key);
	return true;
}

bool CUserInput::remove_key_press_callback(const std::string& id, int virtual_key)
{
	bool found;
	auto& key = get_key(virtual_key, &found);
	if (!found)
	{
		return false;
	}

	key.remove_on_pressed_callback(id);
	CConsole::the().info("Removed key press callback to virtual key {}", virtual_key);
	return true;
}

bool CUserInput::remove_key_unpress_callback(const std::string& id, int virtual_key)
{
	bool found;
	auto& key = get_key(virtual_key, &found);
	if (!found)
	{
		return false;
	}

	key.remove_on_unpressed_callback(id);
	CConsole::the().info("Removed key unpress callback to virtual key {}", virtual_key);
	return true;
}

void CUserInput::for_all_user_keys(const std::function<void(UserKey_t*)>& callback)
{
	for (auto& [vk, key] : m_userkeys)
	{
		callback(&key);
	}
}

void CUserInput::update_keys(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MOUSEWHEEL)
	{
		// mouse wheel event

		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		int vk = delta < 0 ? VK_MWHEELDOWN : VK_MWHEELUP;
		if (m_key_binding_mode)
		{
			m_any_key_pressed = vk;
			m_key_binding_mode = false;
		}
		else
		{
			m_userkeys[vk].update(true);
		}
	}
	else
	{
		// keyboard & mouse key event

		bool is_down = false;

		int vk = NULL;
		switch (uMsg)
		{
			// regular keys
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				vk = wParam;
				is_down = true;
				break;
			}
			case WM_SYSKEYUP:
			case WM_KEYUP:
			{
				vk = wParam;
				is_down = false;
				break;
			}
			// mouse buttons
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			{
				vk = VK_LBUTTON;
				is_down = (uMsg == WM_LBUTTONDOWN);
				break;
			}
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			{
				vk = VK_MBUTTON;
				is_down = (uMsg == WM_MBUTTONDOWN);
				break;
			}
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			{
				vk = VK_RBUTTON;
				is_down = (uMsg == WM_RBUTTONDOWN);
				break;
			}
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			{
				switch (GET_XBUTTON_WPARAM(wParam))
				{
					case XBUTTON1:
					{
						vk = VK_XBUTTON1;
						is_down = (uMsg == WM_XBUTTONDOWN);
						break;
					}
					case XBUTTON2:
					{
						vk = VK_XBUTTON2;
						is_down = (uMsg == WM_XBUTTONDOWN);
						break;
					}
				}
				break;
			}
		}

		if (vk != NULL)
		{
			if (m_key_binding_mode && is_down)
			{
				m_any_key_pressed = vk;
				m_key_binding_mode = false;
			}
			else
			{
				m_userkeys[vk].update(is_down);
			}
		}
	}
}

void CUserInput::initialize_key_translation_tables()
{
	// reserve, for speedup
	m_translation_table_string_to_vk.reserve(k_key_range);
	m_translation_table_vk_to_string.reserve(k_key_range);
	m_translation_table_engine_key_to_vk.reserve(k_key_range);

	//
	// string -> vk
	//
	for (auto& [name, vk, ek] : g_virtual_key_translation)
	{
		if (!name.empty() && vk)
		{
			m_translation_table_string_to_vk[name] = vk;
		}
	}

	//
	// vk -> string
	//
	for (auto& [name, vk, ek] : g_virtual_key_translation)
	{
		if (!name.empty() && vk)
		{
			m_translation_table_vk_to_string[vk] = name;
		}
	}

	//
	// ek -> vk
	//
	for (int i = 0; i < k_key_range; i++)
		m_translation_table_engine_key_to_vk[i] = i; // init first full list
	for (auto& [name, vk, ek] : g_virtual_key_translation)
	{
		if (vk && ek)
		{
			m_translation_table_engine_key_to_vk[ek] = vk;
		}
	}
}

