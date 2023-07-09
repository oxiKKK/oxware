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

#ifndef HLCOMMANDDETOUR_H
#define HLCOMMANDDETOUR_H
#pragma once

using hl_cmd_fn = void(__cdecl*)();

struct HLCmd_FnDetour_t final : public GenericMemoryFnDetour_cdecl<>
{
public:
	bool install_svc(hl_cmd_fn pfn, const char* cmd_name, const wchar_t* module_name)
	{
		m_pfn = pfn;
		m_cmd_name = cmd_name;

		initialize(cmd_name, module_name);

		return install();
	}

public:
	//
	// testing
	//

	virtual void add_to_test() override
	{
		CHookTests::the().add_for_testing("HLCmd_FnDetour", this);
	}

private:
	bool install();

	hl_cmd_fn m_pfn;

	std::string m_cmd_name;
};

//---------------------------------------------------------------------------------

class CHLCommandsDetourMgr
{
public:
	DECL_BASIC_CLASS(CHLCommandsDetourMgr)

public:
	bool install_hooks();
	void uninstall_hooks();

	// detoured commands
	inline auto& in_speed_down_fn() { static HLCmd_FnDetour_t fnhook; return fnhook; }
	inline auto& in_speed_up_fn() { static HLCmd_FnDetour_t fnhook; return fnhook; }
	inline auto& screenshot_fn() { static HLCmd_FnDetour_t fnhook; return fnhook; }
	inline auto& snapshot_fn() { static HLCmd_FnDetour_t fnhook; return fnhook; }

	bool in_speed = false;

private:
	static void in_speed_down_f();
	static void in_speed_up_f();
	static void screenshot_f();
	static void snapshot_f();
};

#endif // HLCOMMANDDETOUR_H
