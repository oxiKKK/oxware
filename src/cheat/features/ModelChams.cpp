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

VarBoolean mdlchams_enable("mdlchams_enable", "Enables model chams", false);

VarBoolean mdlchams_flatshaded("mdlchams_flatshaded", "Enables flat-shaded chams", false);
VarBoolean mdlchams_blend("mdlchams_blend", "Enables chams blend", false);

VarBoolean mdlchams_viewmodel_enable("mdlchams_viewmodel_enable", "Enables viewmodel chams", false);
VarColor mdlchams_viewmodel_color("mdlchams_viewmodel_color", "Viewmodel chams color", CColor(230, 0, 170, 255));
VarInteger mdlchams_viewmodel_type("mdlchams_viewmodel_type", "Viewmodel chams type", CHAMS_Flat, CHAMS_Flat, CHAMS_FlatTercial);

VarBoolean mdlchams_players_t_enable("mdlchams_players_t_enable", "Enables players T chams", false);
VarColor mdlchams_players_t_color("mdlchams_players_t_color", "Players chams T color", CColor(230, 50, 50, 255));
VarInteger mdlchams_players_t_type("mdlchams_players_t_type", "Players chams T type", CHAMS_Flat, CHAMS_Flat, CHAMS_FlatTercial);

VarBoolean mdlchams_players_ct_enable("mdlchams_players_ct_enable", "Enables players CT chams", false);
VarColor mdlchams_players_ct_color("mdlchams_players_ct_color", "Players chams CT color", CColor(50, 50, 230, 255));
VarInteger mdlchams_players_ct_type("mdlchams_players_ct_type", "Players chams CT type", CHAMS_Flat, CHAMS_Flat, CHAMS_FlatTercial);

void CModelChams::initialize()
{
	m_chammed_models.reserve(64);

	intitialize_chammed_model(
		&m_viewmodel, &mdlchams_viewmodel_enable, &mdlchams_viewmodel_color, &mdlchams_viewmodel_type, 
		[]() -> bool
		{
			auto current_ent = CMemoryHookMgr::the().engine_studio_api().get()->GetCurrentEntity();

			return current_ent == &CMemoryHookMgr::the().cl().get()->viewent;
		});

	intitialize_chammed_model(
		&m_players_t, &mdlchams_players_t_enable, &mdlchams_players_t_color, &mdlchams_players_t_type,
		[]() -> bool
		{
			auto current_ent = CMemoryHookMgr::the().engine_studio_api().get()->GetCurrentEntity();
			if (current_ent->player)
			{
				auto& player = CEntityMgr::the().m_known_players[current_ent->index];
				return player.get_team() == hl::TERRORIST;
			}

			return false;
		});

	intitialize_chammed_model(
		&m_players_ct, &mdlchams_players_ct_enable, &mdlchams_players_ct_color, &mdlchams_players_ct_type,
		[]() -> bool
		{
			auto current_ent = CMemoryHookMgr::the().engine_studio_api().get()->GetCurrentEntity();
			if (current_ent->player)
			{
				auto& player = CEntityMgr::the().m_known_players[current_ent->index];
				return player.get_team() == hl::CT;
			}

			return false;
		});
}

void CModelChams::executeall_studio_pre()
{
	OX_PROFILE_SCOPE("studio_points_pre");
	
	if (!mdlchams_enable.get_value())
	{
		return;
	}

	for (auto chams : m_chammed_models)
	{
		if (chams->is_enabled() && chams->should_render())
		{
			chams->process_studio_pre();
		}
	}
}

void CModelChams::executeall_studio_post()
{
	OX_PROFILE_SCOPE("studio_points_post");
	
	if (!mdlchams_enable.get_value())
	{
		return;
	}

	for (auto chams : m_chammed_models)
	{
		if (chams->is_enabled() && chams->should_render())
		{
			chams->process_studio_post();
		}
	}
}

void CModelChams::executeall_color(float* lambert)
{
	OX_PROFILE_SCOPE("modelchams_color");
	
	if (!mdlchams_enable.get_value())
	{
		return;
	}

	for (auto chams : m_chammed_models)
	{
		if (chams->is_enabled() && chams->should_render())
		{
			chams->process_color(lambert);
		}
	}
}

void CModelChams::intitialize_chammed_model(ChammedModel* model, VarBoolean* is_enabled, VarColor* color, VarInteger* type, const std::function<bool()>& should_render)
{
	*model = ChammedModel(is_enabled, color, type, should_render);
	m_chammed_models.push_back(model);
}

//---------------------------------------------------------------------------------

void ChammedModel::process_studio_pre()
{
	if (mdlchams_flatshaded.get_value())
	{
		glShadeModel(GL_FLAT);
	}

	if (mdlchams_blend.get_value())
	{
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
	}

	switch (get_type())
	{
		case CHAMS_Flat:
		{
			glDisable(GL_TEXTURE_2D);
			break;
		}
		case CHAMS_FlatTercial:
		{
			glDisable(GL_TEXTURE_2D);
			break;
		}
	}
}

void ChammedModel::process_studio_post()
{
	if (mdlchams_blend.get_value())
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	switch (get_type())
	{
		case CHAMS_Flat:
		{
			glEnable(GL_TEXTURE_2D);
			break;
		}
		case CHAMS_FlatTercial:
		{
			glEnable(GL_TEXTURE_2D);
			break;
		}
	}
}

void ChammedModel::process_color(float* lambert)
{
	auto color = m_color->get_value();
	
	switch (get_type())
	{
		case CHAMS_Flat:
		{
			lambert[0] = color.r;
			lambert[1] = color.g;
			lambert[2] = color.b;
			
			break;
		}
		case CHAMS_FlatTercial:
		{
			lambert[0] = color.r * lambert[0];
			lambert[1] = color.g * lambert[1];
			lambert[2] = color.b * lambert[2];

			break;
		}
	}
}
