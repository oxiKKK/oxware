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

#ifndef USERMSGDETOUR_H
#define USERMSGDETOUR_H
#pragma once

struct UserMSG_FnDetour_t final : public GenericMemoryFnDetour_cdecl<int, const char*, int, void*>
{
public:
	inline int __cdecl call_usermsg(int iSize, void* pbuf) const { return call(m_usermsg_name.c_str(), iSize, pbuf); }

	bool install_usermsg(hl::pfnUserMsgHook pfn, const char* usermsg_name)
	{
		m_pfn = pfn;
		m_usermsg_name = usermsg_name;

		initialize(usermsg_name, L"hw.dll");

		return install();
	}

public:
	//
	// testing
	//

	virtual void add_to_test() override
	{
		CHookTests::the().add_for_testing("UserMSG_FnDetour", this);
	}

private:
	bool install()
	{
		CConsole::the().info("Searching for UserMSG: '{}'", m_usermsg_name);

		// find the right entry
		hl::UserMsg* entry = *CMemoryHookMgr::the().gClientUserMsgs().get();
		while (entry)
		{
			if (!stricmp(entry->szName, m_usermsg_name.c_str()))
			{
				return detour_using_memory_address((uintptr_t*)m_pfn, (uintptr_t*)entry->pfn);
			}

			entry = entry->next;
		}

		CMessageBox::display_error("Couldn't find UserMSG: '{}'.", m_usermsg_name);
		return false;
	}

	hl::pfnUserMsgHook m_pfn;

	std::string m_usermsg_name;
};

//-------------------------------------------------------------------------------------------

class CUserMSGDetourMgr
{
public:
	DECL_BASIC_CLASS(CUserMSGDetourMgr);

public:
	bool install_hooks();
	void uninstall_hooks();

	//
	// detoured UserMSG hooks
	//

	inline auto& HideWeapon_fn() { static UserMSG_FnDetour_t fnhook; return fnhook; }
	inline auto& MOTD_fn() { static UserMSG_FnDetour_t fnhook; return fnhook; }
	inline auto& BombDrop_fn() { static UserMSG_FnDetour_t fnhook; return fnhook; }
	inline auto& ReceiveW_fn() { static UserMSG_FnDetour_t fnhook; return fnhook; }

private:
	//
	// detourands :D
	//

	static int HideWeapon_f(const char* pszName, int iSize, void* pbuf);
	static int MOTD_f(const char* pszName, int iSize, void* pbuf);
	static int BombDrop_f(const char* pszName, int iSize, void* pbuf);
	static int ReceiveW_f(const char* pszName, int iSize, void* pbuf);
};

#endif // USERMSGDETOUR_H
