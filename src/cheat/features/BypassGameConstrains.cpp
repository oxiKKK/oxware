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
std::optional<GLdouble> CForceEnableDisabled::force_max_viewable_renderdistance()
{
	if (!bypass_constrain_renderdist_enable.get_value())
	{
		return std::nullopt; // let engine decide
	}

	return (GLdouble)bypass_constrain_renderdist_value.get_value();
}

VarBoolean bypass_constrain_renderer_cvars("bypass_constrain_renderer_cvars", "Re-enables setting cvars such as gl_wireframe, chase_active, or gl_clear in singleplayer (and mp).", false);
VarBoolean bypass_constrain_renderer_glclear("bypass_constrain_renderer_glclear", "Enables opengl background clearning", false);
bool CForceEnableDisabled::disable_renderer_cvar_constrain()
{
	if (!bypass_constrain_renderer_cvars.get_value())
	{
		return false;
	}

	if (CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		if (!m_just_disabled_renderer_cvars)
		{
			// save settings and later restore
			save_renderer_cvars();
			m_just_disabled_renderer_cvars = true;
		}

		return false;
	}
	else
	{
		if (m_just_disabled_renderer_cvars)
		{
			restore_renderer_cvars();
			m_just_disabled_renderer_cvars = false;
		}
	}

	return true;
}

VarBoolean bypass_constrain_sponly_cvars("bypass_constrain_sponly_cvars", "Makes singleplayer only cvars also possible to be set in multiplayer.", false);
void CForceEnableDisabled::update_disable_sponly_cvars()
{
	static float last = bypass_constrain_sponly_cvars.get_value();
	
	if (CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		if (!m_just_disabled_sponly_cvars)
		{
			if (bypass_constrain_sponly_cvars.get_value())
			{
				last = bypass_constrain_sponly_cvars.get_value();
				bypass_constrain_sponly_cvars.set_value(0);
				m_just_disabled_sponly_cvars = true;
			}
		}
	}
	else
	{
		if (m_just_disabled_sponly_cvars)
		{
			bypass_constrain_sponly_cvars.set_value(last);
			m_just_disabled_sponly_cvars = false;
		}
	}

	if (bypass_constrain_sponly_cvars.get_value() != 0)
	{
		if (m_saved_sponly_cvars.empty())
		{
			auto cvar = CMemoryHookMgr::the().cl_enginefuncs()->pfnGetFirstCvarPtr();

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
				auto cvar = CGoldSrcCommandMgr::the().get_cvar(disabled_sponly_cvar);
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

void CForceEnableDisabled::save_renderer_cvars()
{
	prev_r_lightmap = CGoldSrcCommandMgr::the().get_cvar("r_lightmap")->value;
	prev_gl_clear = CGoldSrcCommandMgr::the().get_cvar("gl_clear")->value;
	prev_r_novis = CGoldSrcCommandMgr::the().get_cvar("r_novis")->value;
	prev_r_fullbright = CGoldSrcCommandMgr::the().get_cvar("r_fullbright")->value;
	prev_snd_show = CGoldSrcCommandMgr::the().get_cvar("snd_show")->value;
	prev_chase_active = CGoldSrcCommandMgr::the().get_cvar("chase_active")->value;
	prev_gl_monolights = CGoldSrcCommandMgr::the().get_cvar("gl_monolights")->value;
	prev_gl_wireframe = CGoldSrcCommandMgr::the().get_cvar("gl_wireframe")->value;
	prev_r_dynamic = CGoldSrcCommandMgr::the().get_cvar("r_dynamic")->value;
	prev_gl_alphamin = CGoldSrcCommandMgr::the().get_cvar("gl_alphamin")->value;
	prev_gl_max_size = CGoldSrcCommandMgr::the().get_cvar("gl_max_size")->value;
	prev_gl_polyoffset = CGoldSrcCommandMgr::the().get_cvar("gl_polyoffset")->value;
	prev_r_drawentities = CGoldSrcCommandMgr::the().get_cvar("r_drawentities")->value;
	prev_lightgamma = CGoldSrcCommandMgr::the().get_cvar("lightgamma")->value;
}

void CForceEnableDisabled::restore_renderer_cvars()
{
	CGoldSrcCommandMgr::the().get_cvar("r_lightmap")->value = prev_r_lightmap;
	CGoldSrcCommandMgr::the().get_cvar("gl_clear")->value = prev_gl_clear;
	CGoldSrcCommandMgr::the().get_cvar("r_novis")->value = prev_r_novis;
	CGoldSrcCommandMgr::the().get_cvar("r_fullbright")->value = prev_r_fullbright;
	CGoldSrcCommandMgr::the().get_cvar("snd_show")->value = prev_snd_show;
	CGoldSrcCommandMgr::the().get_cvar("chase_active")->value = prev_chase_active;
	CGoldSrcCommandMgr::the().get_cvar("gl_monolights")->value = prev_gl_monolights;
	CGoldSrcCommandMgr::the().get_cvar("gl_wireframe")->value = prev_gl_wireframe;
	CGoldSrcCommandMgr::the().get_cvar("r_dynamic")->value = prev_r_dynamic;
	CGoldSrcCommandMgr::the().get_cvar("gl_alphamin")->value = prev_gl_alphamin;
	CGoldSrcCommandMgr::the().get_cvar("gl_max_size")->value = prev_gl_max_size;
	CGoldSrcCommandMgr::the().get_cvar("gl_polyoffset")->value = prev_gl_polyoffset;
	CGoldSrcCommandMgr::the().get_cvar("r_drawentities")->value = prev_r_drawentities;
	CGoldSrcCommandMgr::the().get_cvar("lightgamma")->value = prev_lightgamma;
}
