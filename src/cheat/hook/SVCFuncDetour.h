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

#ifndef SVCFUNCDETOUR_H
#define SVCFUNCDETOUR_H
#pragma once

struct SVC_FnDetour_t final : public GenericMemoryFnDetour_cdecl<>
{
public:
	bool install_svc(hl::svc_parsefn_t pfn, const char* svc_name, int id)
	{
		assert(id > svc_bad && id < SVC_LASTMSG);
		
		m_pfn = pfn;
		m_svc_name = svc_name;
		m_svc_id = id;

		initialize(svc_name, L"hw.dll");

		return install();
	}

public:
	//
	// testing
	//

	virtual void add_to_test() override
	{
		CHookTests::the().add_for_testing("SVC_FnDetour", this);
	}

private:
	bool install()
	{
		CConsole::the().info("Searching for SVC: '{}'", m_svc_name);

		// find the right entry
		auto svc_func = &CMemoryHookMgr::the().cl_parsefuncs().get()[m_svc_id];

		if (detour_using_memory_address((uintptr_t*)m_pfn, (uintptr_t*)svc_func->pfnParse))
		{
			return true;
		}

		CMessageBox::display_error("Couldn't find SVC: '{}'.", m_svc_name);
		return false;
	}

	hl::svc_parsefn_t m_pfn;

	std::string m_svc_name;
	int m_svc_id;
};

//-------------------------------------------------------------------------------------------

class CSVCFuncDetourMgr
{
public:
	DECL_BASIC_CLASS(CSVCFuncDetourMgr);

public:
	bool install_hooks();
	void uninstall_hooks();

	// detoured SVC hooks
	inline auto& svc_sound_fn() { static SVC_FnDetour_t fnhook; return fnhook; }
	inline auto& svc_time_fn() { static SVC_FnDetour_t fnhook; return fnhook; }
	inline auto& svc_sendcvarvalue_fn() { static SVC_FnDetour_t fnhook; return fnhook; }
	inline auto& svc_sendcvarvalue2_fn() { static SVC_FnDetour_t fnhook; return fnhook; }
	inline auto& svc_stufftext_fn() { static SVC_FnDetour_t fnhook; return fnhook; }

private:
	static void svc_sound_f();
	static void svc_time_f();
	static void svc_sendcvarvalue_f();
	static void svc_sendcvarvalue2_f();
	static void svc_stufftext_f();
};

#endif // SVCFUNCDETOUR_H
