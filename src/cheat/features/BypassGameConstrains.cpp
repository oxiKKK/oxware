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

VarBoolean bypass_constrain_noclip("bypass_constrain_noclip", "Enforces noclip in singleplayer. sv_cheats must be set to 1", false);
void CForceEnableDisabled::force_enable_noclip_pre()
{
	prev_dm = CMemoryHookMgr::the().gGlobalVariables().get()->deathmatch;

	if (bypass_constrain_noclip.get_value())
	{
		CMemoryHookMgr::the().gGlobalVariables().get()->deathmatch = 0;
	}
}

void CForceEnableDisabled::force_enable_noclip_post()
{
	CMemoryHookMgr::the().gGlobalVariables().get()->deathmatch = prev_dm;
}

VarBoolean bypass_constrain_renderdist_enable("bypass_constrain_renderdist_enable", "Enforces max viweable render distance.", false);
VarInteger bypass_constrain_renderdist_value("bypass_constrain_renderdist_value", "Enforces max viweable render distance.", 8192, 512, 8192);
GLdouble CForceEnableDisabled::force_max_viewable_renderdistance()
{
	if (!bypass_constrain_renderdist_enable.get_value())
	{
		return -1.0; // let engine decide
	}

	return (GLdouble)bypass_constrain_renderdist_value.get_value();
}

VarBoolean bypass_constrain_renderer_cvars("bypass_constrain_renderer_cvars", "Re-enables setting cvars such as gl_wireframe, chase_active, or gl_clear in singleplayer (and mp).", false);
bool CForceEnableDisabled::disable_renderer_cvar_constrain()
{
	return bypass_constrain_renderer_cvars.get_value() != 0;
}

VarBoolean bypass_constrain_sponly_cvars("bypass_constrain_sponly_cvars", "Makes singleplayer only cvars also possible to be set in multiplayer.", false);
void CForceEnableDisabled::update_disable_sponly_cvars()
{	
	if (bypass_constrain_sponly_cvars.get_value() != 0)
	{
		if (m_saved_sponly_cvars.empty())
		{
			auto cvar = CMemoryHookMgr::the().cl_enginefuncs().get()->pfnGetFirstCvarPtr();

			while (cvar)
			{
				if (FBitSet(cvar->flags, FCVAR_SPONLY))
				{
					FClrBit(cvar->flags, FCVAR_SPONLY);

					m_saved_sponly_cvars.push_back(cvar->name);

					CConsole::the().info("Disabled SP-ONLY flag on cvar {}", cvar->name);
				}

				cvar = cvar->next;
			}
		}
	}
	else
	{
		if (!m_saved_sponly_cvars.empty())
		{
			for (auto& disabled_sponly_cvar : m_saved_sponly_cvars)
			{
				// find the cvar we have disabled
				auto cvar = CMemoryHookMgr::the().cl_enginefuncs().get()->pfnGetCvarPointer((char*)disabled_sponly_cvar.c_str());
				if (!cvar)
				{
					continue;
				}

				FSetBit(cvar->flags, FCVAR_SPONLY);
				CConsole::the().info("Enabled back SP-ONLY flag on cvar {}", cvar->name);
			}

			m_saved_sponly_cvars.clear();
		}
	}
}
