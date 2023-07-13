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

IRegistry* g_registry_i = nullptr;

class CRegistry : public IRegistry
{
public:
	CRegistry();
	~CRegistry();

	void initialize();
	void shutdown();

	int read_int(EKeyType type, const char* key, int default_value = 0);
	void write_int(EKeyType type, const char* key, int value);

	const char* read_string(EKeyType type, const char* key, const char* default_value = nullptr);
	void write_string(EKeyType type, const char* key, const char* value);

	// if not found, NULL is returned.
	HKEY get_key(EKeyType type);

private:
	bool add_new_key(HKEY key, const char* subkey, EKeyType type);

private:
	bool m_valid;

	std::array<HKEY, REG_COUNT> m_registry_key_vault;
};

CRegistry g_registry;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CRegistry, IRegistry, IREGISTRY_INTERFACEID, g_registry);

//-------------------------------------------------------------------------------
//
// CRegistry implementation
//
//-------------------------------------------------------------------------------

CRegistry::CRegistry()
{
	g_registry_i = this;
}

CRegistry::~CRegistry()
{
	g_registry_i = nullptr;
}

void CRegistry::initialize()
{
	// default settings subkey for Valve games
	if (!add_new_key(HKEY_CURRENT_USER, "Software\\Valve\\Half-Life\\Settings", REG_VALVE))
	{
		return;
	}

	if (!add_new_key(HKEY_CURRENT_USER, "Software\\oxware", REG_OXWARE))
	{
		return;
	}

	if (!add_new_key(HKEY_CURRENT_USER, "Software\\Valve\\Steam", REG_STEAM))
	{
		return;
	}

	CConsole::the().info("Registry initialized.");
}

void CRegistry::shutdown()
{
	if (!m_valid)
	{
		return;
	}

	CConsole::the().info("Shutting down registry...");

	m_valid = false;
	for (auto& key : m_registry_key_vault)
	{
		LONG st = RegCloseKey(key);
		assert(st == ERROR_SUCCESS);
	}
}

int CRegistry::read_int(EKeyType type, const char* key, int default_value)
{
	if (!m_valid) return default_value;

	LONG st;
	DWORD reg_type;
	int value;
	DWORD size = sizeof(value);

	HKEY hkey = get_key(type);
	if (hkey == NULL)
	{
		return default_value;
	}

	st = RegQueryValueExA(
		hkey,				// handle to key	
		key,				// value name
		NULL,				// reserved
		&reg_type,			// type buffer
		(LPBYTE)&value,		// data buffer
		&size);				// size of data buffer

	if (st != ERROR_SUCCESS)
	{
		CConsole::the().error("Registry: Failed to read integer value from key '{}'. (st=0x{:08X})", key, st);
		return default_value;
	}

	if (reg_type != REG_DWORD)
	{
		return default_value;
	}

	return value;
}

void CRegistry::write_int(EKeyType type, const char* key, int value)
{
	if (!m_valid) return;

	DWORD size = sizeof(DWORD);

	HKEY hkey = get_key(type);
	if (hkey == NULL)
	{
		return;
	}

	LONG st = RegSetValueExA(
		hkey,			// handle to key
		key,			// value name
		0,				// reserved
		REG_DWORD,		// type buffer
		(LPBYTE)&value,	// data buffer
		size);			// size of data buffer

	if (st != ERROR_SUCCESS)
	{
		CConsole::the().error("Registry: Failed to write integer '{}' to key '{}'. (st=0x{:08X})", value, key, st);
	}
}

const char* CRegistry::read_string(EKeyType type, const char* key, const char* default_value)
{
	if (!m_valid) return default_value;

	LONG st;
	static char value[512] = {};
	DWORD size = sizeof(value);
	DWORD reg_type;

	HKEY hkey = get_key(type);
	if (hkey == NULL)
	{
		return default_value;
	}

	st = RegQueryValueExA(
		hkey,					// handle to key	
		key,					// value name
		NULL,					// reserved
		&reg_type,				// type buffer
		(unsigned char*)&value,	// data buffer
		&size);					// size of data buffer

	if (st != ERROR_SUCCESS)
	{
		CConsole::the().error("Registry: Failed to read string value from key '{}'. (st=0x{:08X})", key, st);
		return default_value;
	}

	if (reg_type != REG_SZ)
	{
		return default_value;
	}

	return value;
}

void CRegistry::write_string(EKeyType type, const char* key, const char* value)
{
	if (!m_valid) return;

	DWORD size = strlen(value) + 1;

	HKEY hkey = get_key(type);
	if (hkey == NULL)
	{
		return;
	}

	LONG st = RegSetValueExA(
		hkey,			// handle to key
		key,			// value name
		0,				// reserved
		REG_SZ,			// type buffer
		(LPBYTE)value,	// data buffer
		size);			// size of data buffer

	if (st != ERROR_SUCCESS)
	{
		CConsole::the().error("Registry: Failed to write string '{}' to key '{}'. (st=0x{:08X})", value, key, st);
	}
}

HKEY CRegistry::get_key(EKeyType type)
{
	assert(type >= 0 && type < REG_COUNT);
	return m_registry_key_vault[type];
}

bool CRegistry::add_new_key(HKEY key, const char* subkey, EKeyType type)
{
	LONG st;
	HKEY new_key;

	st = RegCreateKeyExA(
		key,						// handle of open key 
		subkey,						// address of name of subkey to open 
		0,							// reserved 
		NULL,						// Type of value
		REG_OPTION_NON_VOLATILE,	// Store permanently in reg.
		KEY_ALL_ACCESS,				// security access mask 
		NULL,
		&new_key,					// Key we are creating
		NULL);						// Type of creation

	if (st != ERROR_SUCCESS)
	{
		m_valid = false;
		CConsole::the().error("Registry: Failed to create key. (st=0x{:08X})", st);
		return false;
	}

	m_registry_key_vault[type] = new_key;

	m_valid = true;
	return true;
}
