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

#ifndef IREGISTRY_H
#define IREGISTRY_H
#pragma once

enum EKeyType
{
	REG_OXWARE,		// "Software\\oxware"
	REG_VALVE,		// "Software\\Valve\\Half-Life\\Settings"
	REG_STEAM,		// "Software\\Valve\\Steam"

	REG_COUNT, 
};

class IRegistry : public IBaseInterface
{
public:
	virtual void initialize() = 0;
	virtual void shutdown() = 0;

	virtual int read_int(EKeyType type, const char* key, int default_value = 0) = 0;
	virtual void write_int(EKeyType type, const char* key, int value) = 0;

	virtual const char* read_string(EKeyType type, const char* key, const char* default_value = nullptr) = 0;
	virtual void write_string(EKeyType type, const char* key, const char* value) = 0;

	// if not found, NULL is returned.
	virtual HKEY get_key(EKeyType type) = 0;
};

extern IRegistry* g_registry_i;

#define IREGISTRY_INTERFACEID "IRegistry"

#endif // IREGISTRY_H