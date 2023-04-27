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

VarBoolean mdlchams_player_skeleton("mdlchams_player_skeleton", "Disables playermodels but instead renders their skeleton", false);

VarBoolean mdlchams_head_box_enable("mdlchams_head_box_enable", "Renders box at players heads", false);
VarColor mdlchams_head_box_color("mdlchams_head_box_color", "Color of the player head box", CColor(0, 255, 0));

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

bool CModelChams::studio_draw_skeleton()
{
	if (!mdlchams_player_skeleton.get_value())
	{
		return false; // render points instead.
	}

	auto engine_studio_api = CMemoryHookMgr::the().engine_studio_api().get();

	auto current_ent = engine_studio_api->GetCurrentEntity();
	if (current_ent->player == false)
	{
		return false;
	}

	auto pstudiohdr = get_currently_rendered_model_header();
	if (!pstudiohdr)
	{
		// our hooks aren't initialized yet. Strange...
		return false;
	}

	auto pbones = (hl::mstudiobone_t*)((byte*)pstudiohdr + pstudiohdr->boneindex);
	auto pbboxes = (hl::mstudiobbox_t*)((byte*)pstudiohdr + pstudiohdr->hitboxindex);

	// light or bone, doesn't matter.
	float(*couple_of_pointers)[MAXSTUDIOBONES][3][4] = (float(*)[MAXSTUDIOBONES][3][4])engine_studio_api->StudioGetBoneTransform();

	auto player = CEntityMgr::the().get_player_by_id(current_ent->index);

	CColor color_based_on_team = CColor(170, 170, 170); // gray by default
	if (player && player->is_valid())
	{
		color_based_on_team = player->get_color_based_on_team();
	}

	glDisable(GL_TEXTURE_2D);

	//
	// Loop through all parent bones and render concatenated lines.
	//
	for (int i = 0; i < pstudiohdr->numbones; i++)
	{
		if (pbones[i].parent == -1) // non-existent parent bone
			continue;

		int k = pbones[i].parent;

		Vector bone_position = {
			(*couple_of_pointers)[i][0][3],
			(*couple_of_pointers)[i][1][3],
			(*couple_of_pointers)[i][2][3] };

		Vector parent_bone_position = {
			(*couple_of_pointers)[k][0][3],
			(*couple_of_pointers)[k][1][3],
			(*couple_of_pointers)[k][2][3] };

		glLineWidth(1);
		glColor4fv(color_based_on_team.get_base());

		glBegin(GL_LINES);

		glVertex3f(bone_position.x, bone_position.y, bone_position.z);
		glVertex3f(parent_bone_position.x, parent_bone_position.y, parent_bone_position.z);

		glEnd();
	}

	glEnable(GL_TEXTURE_2D);

	return true;
}

static int g_cubic_face_translation[6][4] =
{
	{ 0, 4, 6, 2 }, // +X
	{ 0, 1, 5, 4 }, // +Y
	{ 0, 2, 3, 1 }, // +Z
	{ 7, 5, 1, 3 }, // -X
	{ 7, 3, 2, 6 }, // -Y
	{ 7, 6, 4, 5 }, // -Z
};

void CModelChams::render_playerhead_hitbox()
{
	if (!mdlchams_head_box_enable.get_value())
	{
		return;
	}

	auto engine_studio_api = CMemoryHookMgr::the().engine_studio_api().get();

	if (engine_studio_api->GetCurrentEntity()->player == false)
	{
		return;
	}

	auto pstudiohdr = get_currently_rendered_model_header();
	if (!pstudiohdr)
	{
		// our hooks aren't initialized yet. Strange...
		return;
	}

	// light or bone, doesn't matter.
	float(*couple_of_pointers)[MAXSTUDIOBONES][3][4] = (float(*)[MAXSTUDIOBONES][3][4])engine_studio_api->StudioGetBoneTransform();

	auto bbox = (hl::mstudiobbox_t*)((byte*)pstudiohdr + pstudiohdr->hitboxindex);

	Vector bbox_transformator, p[8];

	glDisable(GL_TEXTURE_2D);

	for (int i = 0; i < pstudiohdr->numhitboxes; i++)
	{
		// the infamous head hitbox
		//
		// This won't work for some models such as some huge crazyass zombie models etc. This is
		// sort of a "standard" in the modelling sphere, where if you're a modeler and you're
		// making a player model, you SHOULD make the hitgroup 1 the head, but not everyone does this.
		// Therefor there isn't really a proper way to look for a head hitbox however, from what I've
		// see and from my testing, this works for most regular models, and for the majority of zombie
		// models, but that really depends..
		if (bbox[i].group == hl::HITGROUP_HEAD)
		{
			// get all cube vertices
			for (int j = 0; j < 8; j++)
			{
				bbox_transformator[0] = (j & 1) ? bbox[i].bbmin[0] : bbox[i].bbmax[0];
				bbox_transformator[1] = (j & 2) ? bbox[i].bbmin[1] : bbox[i].bbmax[1];
				bbox_transformator[2] = (j & 4) ? bbox[i].bbmin[2] : bbox[i].bbmax[2];

				CMathUtil::the().vector_transform(bbox_transformator, (*couple_of_pointers)[bbox[i].bone], p[j]);
			}

			glBegin(GL_QUADS);
			
			glColor4fv(mdlchams_head_box_color.get_value().get_base());

			// render all faces of a cube
			for (int j = 0; j < 6; j++)
			{
				glVertex3fv(p[g_cubic_face_translation[j][0]]);
				glVertex3fv(p[g_cubic_face_translation[j][1]]);
				glVertex3fv(p[g_cubic_face_translation[j][2]]);
				glVertex3fv(p[g_cubic_face_translation[j][3]]);
			}

			glEnd();
		}
	}

	glEnable(GL_TEXTURE_2D);
}

hl::model_t* CModelChams::get_currently_rendered_model()
{
	auto& hook = CMemoryHookMgr::the().r_model();
	if (hook.is_installed())
	{
		return *hook.get();
	}

	return nullptr;
}

hl::studiohdr_t* CModelChams::get_currently_rendered_model_header()
{
	auto& hook = CMemoryHookMgr::the().pstudiohdr();
	if (hook.is_installed())
	{
		return *hook.get();
	}

	return nullptr;
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
