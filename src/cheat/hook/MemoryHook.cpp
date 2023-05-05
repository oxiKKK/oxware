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
	if (!cl_funcs().install()) return false;
	if (!cl_enginefuncs().install()) return false;
	if (!pmainwindow().install()) return false;
	// host_initialized gets installed while initialization because it's needed in order to continue initing.
	if (!sv_player().install()) return false;
	if (!cl().install()) return false;
	if (!cls().install()) return false;
	if (!gGlobalVariables().install()) return false;
	if (!scr_fov_value().install()) return false;
	if (CoXWARE::the().get_build_number() < 8684) // old struct
	{
		if (!g_PlayerExtraInfoOld().install()) return false;
	}
	else
	{
		if (!g_PlayerExtraInfo().install()) return false;
	}
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

//-----------------------------------------------------------------------------

bool cl_funcs_MemoryHook::install()
{
	initialize("cl_funcs", L"hw.dll");
	return install_using_bytepattern(1);
}

void cl_funcs_MemoryHook::test_hook()
{
	auto p = get();
	hl::qboolean is_3rdperson;

	CHookTests::the().run_seh_protected_block(
		m_name, 
		[&]()
		{
			// see if we won't crash
			is_3rdperson = p->pfnHUD_CL_IsThirdperson();

			return true;
		});
}

//-----------------------------------------------------------------------------

bool cl_enginefuncs_MemoryHook::install()
{
	initialize("cl_enginefuncs", L"hw.dll");
	return install_using_bytepattern(1);
}

void cl_enginefuncs_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			return p->pfnGetCvarPointer((char*)"fps_max");
		});
}

//-----------------------------------------------------------------------------

bool pmainwindow_MemoryHook::install()
{
	initialize("pmainwindow", L"hw.dll");
	return install_using_bytepattern(1);
}

void pmainwindow_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			HWND* phwnd = *p;
			return phwnd != NULL && *phwnd != NULL;
		});
}

//-----------------------------------------------------------------------------

bool host_initialized_MemoryHook::install()
{
	initialize("host_initialized", L"hw.dll");
	return install_using_bytepattern(1);
}

void host_initialized_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			hl::qboolean engine_initialized = *p;
			if (CGameUtil::the().is_fully_connected())
			{
				return engine_initialized == TRUE;
			}

			return true;
		});
}

//-----------------------------------------------------------------------------

bool sv_player_MemoryHook::install()
{
	initialize("sv_player", L"hw.dll");
	return install_using_bytepattern(1);
}

//-----------------------------------------------------------------------------

bool cl_MemoryHook::install()
{
	initialize("cl", L"hw.dll");
	return install_using_bytepattern(1);
}

void cl_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			if (CGameUtil::the().is_fully_connected())
			{
				return p->time != 0.0;
			}

			return true;
		});
}

//-----------------------------------------------------------------------------

bool cls_MemoryHook::install()
{
	initialize("cls", L"hw.dll");
	return install_using_bytepattern(1);
}

void cls_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			return p->state >= hl::ca_dedicated && p->state <= hl::ca_active;
		});
}

//-----------------------------------------------------------------------------

bool gGlobalVariables_MemoryHook::install()
{
	initialize("gGlobalVariables", L"hw.dll");
	return install_using_bytepattern(1);
}

void gGlobalVariables_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			if (CGameUtil::the().is_fully_connected())
			{
				return p->maxClients && p->maxEntities;
			}

			return true;
		});
}

//-----------------------------------------------------------------------------

bool scr_fov_value_MemoryHook::install()
{
	initialize("scr_fov_value", L"hw.dll");
	return install_using_bytepattern(1);
}

void scr_fov_value_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			float fov = *p;
			return fov >= 0 && fov <= 150;
		});
}

//-----------------------------------------------------------------------------

bool g_PlayerExtraInfo_MemoryHook::install()
{
	initialize("g_PlayerExtraInfo", L"client.dll");
	return install_using_bytepattern(1);
}

void g_PlayerExtraInfo_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			auto& cur = p[48]; // random index, just to see if everything is valid. 
			return cur.deaths == 0 && cur.frags == 0 && cur.has_c4 == false && cur.teamname[0] == 0;
		});
}

//-----------------------------------------------------------------------------

bool g_PlayerExtraInfoOld_MemoryHook::install()
{
	initialize("g_PlayerExtraInfoOld", L"client.dll");
	return install_using_bytepattern(1); 
}

void g_PlayerExtraInfoOld_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			auto& cur = p[48]; // random index, just to see if everything is valid. 
			return cur.deaths == 0 && cur.frags == 0 && cur.has_c4 == false && cur.teamname[0] == 0;
		});
}

//-----------------------------------------------------------------------------

bool engine_studio_api_MemoryHook::install()
{
	initialize("engine_studio_api", L"hw.dll");
	return install_using_bytepattern(1);
}

void engine_studio_api_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			return p->GetCvar((char*)"fps_max");
		});
}

//-----------------------------------------------------------------------------

bool cl_parsefuncs_MemoryHook::install()
{
	initialize("cl_parsefuncs", L"hw.dll");
	return install_using_bytepattern(
		1, 
		[](uintptr_t** address) -> void
		{
			// there isn't a single occurence of the exact base of the svc_func_t table, only at offset +0x4 relative to the base.
			// therefore, we need to manually offset to the base of the array.
			*address -= 0x2; // a dword shift back
		});
}

void cl_parsefuncs_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			return !stricmp(p[svc_filetxferfailed].pszname, "svc_filetxferfailed");
		});
}

//-----------------------------------------------------------------------------

bool pmove_MemoryHook::install()
{
	initialize("pmove", L"hw.dll");
	return install_using_bytepattern(1);
}

void pmove_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			auto pmove = *p;

			if (CGameUtil::the().is_fully_connected())
			{
				return 
					pmove->PM_TraceModel != nullptr && 
					pmove->PM_PointContents != nullptr &&
					pmove->Con_DPrintf != nullptr &&
					pmove->COM_FreeFile != nullptr &&
					pmove->PM_TraceLineEx != nullptr &&
					pmove->PM_Particle != nullptr &&
					pmove->time != 0.0f;
			}

			return pmove != nullptr;
		});
}

//-----------------------------------------------------------------------------

bool gClientUserMsgs_MemoryHook::install()
{
	initialize("gClientUserMsgs", L"hw.dll");
	return install_using_bytepattern(1);
}

void gClientUserMsgs_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			auto umsg = *p; 

			int n = 0, max = 5; // see at least 5 entries
			while (umsg && n++ < max)
			{
				if (!umsg->szName)
					return false;

				umsg = umsg->next;
			}

			return true;
		});
}

//-----------------------------------------------------------------------------

bool g_iShotsFired_MemoryHook::install()
{
	initialize("g_iShotsFired", L"client.dll");
	return install_using_bytepattern(1);
}

//-----------------------------------------------------------------------------

bool r_model_MemoryHook::install()
{
	initialize("r_model", L"hw.dll");
	return install_using_bytepattern(1);
}

//-----------------------------------------------------------------------------

bool pstudiohdr_MemoryHook::install()
{
	initialize("pstudiohdr", L"hw.dll");
	return install_using_bytepattern(1);
}

//-----------------------------------------------------------------------------

bool pStudioAPI_MemoryHook::install()
{
	initialize("pStudioAPI", L"hw.dll");
	return install_using_bytepattern(1);
}

void pStudioAPI_MemoryHook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			auto pStudioAPI = *p;
			return pStudioAPI->StudioDrawModel && pStudioAPI->StudioDrawPlayer && pStudioAPI->version == STUDIO_INTERFACE_VERSION;
		});
}

//-----------------------------------------------------------------------------
