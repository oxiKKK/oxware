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

bool HLCmd_FnDetour_t::install()
{
	CConsole::the().info("Searching for HL command: '{}'", m_cmd_name);

	// find the right entry
	auto cmd = CGoldSrcCommandMgr::the().get_cmd(m_cmd_name);
	if (cmd)
	{
		if (detour_using_memory_address((uintptr_t*)m_pfn, (uintptr_t*)cmd->function))
		{
			return true;
		}
	}

	CMessageBox::display_error("Couldn't find HL command: '{}'.", m_cmd_name);
	return false;
}

//---------------------------------------------------------------------------------

bool CHLCommandsDetourMgr::install_hooks()
{
	// let's install and detour individual hooks.
	if (!in_speed_down_fn().install_svc(in_speed_down_f, "+speed", L"hw.dll")) return false;
	if (!in_speed_up_fn().install_svc(in_speed_up_f, "-speed", L"hw.dll")) return false;
	if (!screenshot_fn().install_svc(screenshot_f, "screenshot", L"hw.dll")) return false;
	if (!snapshot_fn().install_svc(snapshot_f, "snapshot", L"hw.dll")) return false;

	return true;
}

void CHLCommandsDetourMgr::uninstall_hooks()
{
	// now, uninstall the detour from each function.
	in_speed_down_fn().uninstall();
	in_speed_up_fn().uninstall();
	screenshot_fn().uninstall();
	snapshot_fn().uninstall();
}

//---------------------------------------------------------------------------------

void CHLCommandsDetourMgr::in_speed_down_f()
{
	CHLCommandsDetourMgr::the().in_speed = true;
	CHLCommandsDetourMgr::the().in_speed_down_fn().call();
}

void CHLCommandsDetourMgr::in_speed_up_f()
{
	CHLCommandsDetourMgr::the().in_speed = false;
	CHLCommandsDetourMgr::the().in_speed_up_fn().call();
}

void CHLCommandsDetourMgr::screenshot_f()
{
	//if (CAntiScreen::the().someone_requested_screenshot(ANTISCREEN_Screenshot))
	//{
	//	return;
	//}

	CHLCommandsDetourMgr::the().screenshot_fn().call();
}

void CHLCommandsDetourMgr::snapshot_f()
{
	//if (CAntiScreen::the().someone_requested_screenshot(ANTISCREEN_Snapshot))
	//{
	//	return;
	//}

	CHLCommandsDetourMgr::the().snapshot_fn().call();
}
