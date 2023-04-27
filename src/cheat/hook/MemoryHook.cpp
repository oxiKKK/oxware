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

bool CMemoryHookMgr::install_hooks()
{
	// let's install individual hooks.
	if (!cldllfunc().install()) return false;
	if (!cl_enginefuncs().install()) return false;
	if (!pmainwindow().install()) return false;
	// host_initialized gets installed while initialization because it's needed in order to continue initing.
	if (!sv_player().install()) return false;
	if (!cl().install()) return false;
	if (!cls().install()) return false;
	if (!gGlobalVariables().install()) return false;
	if (!scr_fov_value().install()) return false;
	if (!g_PlayerExtraInfo().install()) return false;
	if (!engine_studio_api().install()) return false;
	if (!cl_parsefuncs().install()) return false;
	if (!pmove().install()) return false;
	if (!gClientUserMsgs().install()) return false;
	if (!g_iShotsFired().install()) return false;
	if (!r_model().install()) return false;
	if (!pstudiohdr().install()) return false;
	if (!pStudioAPI().install()) return false;

	return true;
}

void CMemoryHookMgr::uninstall_hooks()
{
	// now, uninstall everything and restore original memory.
	cldllfunc().uninstall();
	cl_enginefuncs().uninstall();
	pmainwindow().uninstall();
	host_initialized().uninstall();
	sv_player().uninstall();
	cl().uninstall();
	cls().uninstall();
	gGlobalVariables().uninstall();
	scr_fov_value().uninstall();
	g_PlayerExtraInfo().uninstall();
	engine_studio_api().uninstall();
	cl_parsefuncs().uninstall();
	pmove().uninstall();
	gClientUserMsgs().uninstall();
	g_iShotsFired().uninstall();
	r_model().uninstall();
	pstudiohdr().uninstall();
	pStudioAPI().uninstall();
}

//-----------------------------------------------------------------------------

bool CClDllFuncHook::install()
{
	initialize("cldll_func_t", L"hw.dll", false);
	return generic_bytepattern_installer({ "\xFF\x15\x00\x00\x00\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83", 0x2 });
}

bool cl_enginefuncsHook::install()
{
	initialize("cl_enginefuncs", L"hw.dll", false);
	return generic_bytepattern_installer({ "\x68\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x1C\xE8\x00\x00\x00\x00\x8B\xE5\x5D\xC3", 0x1 });
}

bool pmainwindowHook::install()
{
	initialize("pmainwindow", L"hw.dll", false);
	return generic_bytepattern_installer({ "\xA1\x00\x00\x00\x00\x3B\xC3\x74\x49\xC6\x45\xC8\x02\x88\x5D\xC9\x88\x5D\xCA\x8B\x10\x8D\x4D\xC8\x51\x52\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x8B\x55\xD0\x83\xC4\x08\x8B\x08\x6A\x03\x52\x50\xFF\x51\x18\x85\xC0\x74\x1B\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x04\xE8\x00\x00\x00\x00\x8B\xC7\x5F\x5E\x5B\x8B\xE5\x5D\xC3\xBE\x00\x00\x00\x00\x8D\x45\xD4\x56\x53\x50", 0x1 });
}

bool host_initializedHook::install()
{
	initialize("host_initialized", L"hw.dll", false);
	return generic_bytepattern_installer({ "\xA1\x00\x00\x00\x00\x56\x85\xC0\xC7", 0x1 });
}

bool sv_playerHook::install()
{
	initialize("sv_player", L"hw.dll", false);
	return generic_bytepattern_installer({ "\xA1\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x83\xF2", 0x1 }, 1);
}

bool clHook::install()
{
	initialize("cl", L"hw.dll", false);
	return generic_bytepattern_installer({ "\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x83\xC4\x0C", 0x1 });
}

bool clsHook::install()
{
	initialize("cls", L"hw.dll", false);
	return generic_bytepattern_installer({ "\xA1\x00\x00\x00\x00\x83\xC4\x0C\x83\xF8", 0x1 });
}

bool gGlobalVariablesHook::install()
{
	initialize("gGlobalVariables", L"hw.dll", false);
	return generic_bytepattern_installer({ "\xD9\x1D\x00\x00\x00\x00\x57\x8B\x7D\x08\x33\xF6\x33\xDB\x57", 0x2 });
}

bool scr_fov_valueHook::install()
{
	initialize("scr_fov_value", L"hw.dll", false);
	return generic_bytepattern_installer({ "\xA1\x00\x00\x00\x00\x56\x89\x45\xFC\xD9\x5D\xF8\xDB\x05\x00\x00\x00\x00\x8B\x4D\xF8", 0x1 });
}

bool g_PlayerExtraInfoHook::install()
{
	initialize("g_PlayerExtraInfo", L"client.dll", false);
	return generic_bytepattern_installer({ "\x0F\xBF\x00\x00\x00\x00\x00\x8B\x16\x50\x68\x00\x00\x00\x00\x8B\xCE\xFF\x52\x48\x8B\xCB", 0x3 });
}

bool engine_studio_apiHook::install()
{
	initialize("engine_studio_api", L"hw.dll", false);
	return generic_bytepattern_installer({ "\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\x6A\x01\xFF\xD0\x83\xC4\x0C", 0x1 });
}

bool cl_parsefuncsHook::install()
{
	initialize("cl_parsefuncs", L"hw.dll", false);
	return generic_bytepattern_installer(
		{ "\x8B\x00\x00\x00\x00\x00\x85\xFF\x74\x00\xD9", 0x2 }, 1, 
		[](uintptr_t** address) -> void
		{
			// there isn't a single occurence of the exact base of the svc_func_t table, only at offset +0x4 relative to the base.
			// therefore, we need to manually offset to the base of the array.
			*address -= 0x2; // a dword shift back
		});
}

bool pmoveHook::install()
{
	initialize("pmove", L"hw.dll", false);
	return generic_bytepattern_installer({ "\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x1C\xE8", 0x1 });
}

bool gClientUserMsgsHook::install()
{
	initialize("gClientUserMsgs", L"hw.dll", false);
	return generic_bytepattern_installer({ "\x8B\x00\x00\x00\x00\x00\x85\xF6\x74\x00\x39", 0x2 });
}

bool g_iShotsFiredHook::install()
{
	initialize("g_iShotsFired", L"client.dll", false);
	return generic_bytepattern_installer({ "\xA1\x00\x00\x00\x00\x3D\x00\x00\x00\x00\x7E\x00\xB8", 0x1 });
}

bool r_modelHook::install()
{
	initialize("r_model", L"hw.dll", false);
	return generic_bytepattern_installer({ "\xA1\x00\x00\x00\x00\x50\xC7\x45\xD8\x00\x00\x00\x00", 0x1 });
}

bool pstudiohdrHook::install()
{
	initialize("pstudiohdr", L"hw.dll", false);
	return generic_bytepattern_installer({ "\x8B\x0D\x00\x00\x00\x00\x83\xC4\x08\xC7\x45\xC0\x00\x00\x00\x00", 0x2 });
}

bool pStudioAPIHook::install()
{
	initialize("pStudioAPI", L"hw.dll", false);
	return generic_bytepattern_installer({ "\x8B\x0D\x00\x00\x00\x00\x8B\x35\x00\x00\x00\x00\xD1\xEA", 0x2 });
}
