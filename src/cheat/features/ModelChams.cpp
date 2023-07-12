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

// properties
VarBoolean mdlchams_flatshaded("mdlchams_flatshaded", "Enables flat-shaded chams", false);
VarBoolean mdlchams_blend("mdlchams_blend", "Enables chams blend", false);
VarBoolean mdlchams_rainbow("mdlchams_rainbow", "Enables rainbow chams colors", false);
VarInteger mdlchams_rainbow_speed("mdlchams_rainbow_speed", "Speed of the rainbow color changer", 1, 1, 5);

// viewmodel
VarBoolean mdlchams_viewmodel_enable("mdlchams_viewmodel_enable", "Enables viewmodel chams", false);
VarColor mdlchams_viewmodel_color("mdlchams_viewmodel_color", "Viewmodel chams color", CColor(230, 0, 170, 255));
VarInteger mdlchams_viewmodel_type("mdlchams_viewmodel_type", "Viewmodel chams type", CHAMS_Flat, CHAMS_Flat, CHAMS_Wireframe);
VarInteger mdlchams_viewmodel_shadelight("mdlchams_viewmodel_shadelight", "Controls shadelight on viewmodel", 177, 0, 255);
VarInteger mdlchams_viewmodel_ambientlight("mdlchams_viewmodel_ambientlight", "Controls ambientlight on viewmodel", 20, 0, 255);

// t
VarBoolean mdlchams_players_t_enable("mdlchams_players_t_enable", "Enables players T chams", false);
VarColor mdlchams_players_t_color("mdlchams_players_t_color", "Players chams T color", CColor(230, 50, 50, 255));
VarInteger mdlchams_players_t_type("mdlchams_players_t_type", "Players chams T type", CHAMS_Flat, CHAMS_Flat, CHAMS_Wireframe);
VarInteger mdlchams_players_t_shadelight("mdlchams_players_t_shadelight", "Controls shadelight on Ts", 177, 0, 255);
VarInteger mdlchams_players_t_ambientlight("mdlchams_players_t_ambientlight", "Controls ambientlight on Ts", 20, 0, 255);

// ct
VarBoolean mdlchams_players_ct_enable("mdlchams_players_ct_enable", "Enables players CT chams", false);
VarColor mdlchams_players_ct_color("mdlchams_players_ct_color", "Players chams CT color", CColor(50, 50, 230, 255));
VarInteger mdlchams_players_ct_type("mdlchams_players_ct_type", "Players chams CT type", CHAMS_Flat, CHAMS_Flat, CHAMS_Wireframe);
VarInteger mdlchams_players_ct_shadelight("mdlchams_players_ct_shadelight", "Controls shadelight on CTs", 177, 0, 255);
VarInteger mdlchams_players_ct_ambientlight("mdlchams_players_ct_ambientlight", "Controls ambientlight on CTs", 20, 0, 255);

// backtrack
VarBoolean mdlchams_players_backtrack_enable("mdlchams_players_backtrack_enable", "Enables players backtrack chams", false);
VarColor mdlchams_players_backtrack_color("mdlchams_players_backtrack_color", "Players chams backtrack color", CColor(210, 0, 250, 255));
VarInteger mdlchams_players_backtrack_type("mdlchams_players_backtrack_type", "Players chams backtrack type", CHAMS_Flat, CHAMS_Flat, CHAMS_Wireframe);
VarInteger mdlchams_players_backtrack_shadelight("mdlchams_players_backtrack_shadelight", "Controls shadelight on backtracked players", 177, 0, 255);
VarInteger mdlchams_players_backtrack_ambientlight("mdlchams_players_backtrack_ambientlight", "Controls ambientlight on backtracked players", 20, 0, 255);

VarBoolean mdlchams_player_skeleton("mdlchams_player_skeleton", "Disables playermodels but instead renders their skeleton", false);

VarBoolean mdlchams_head_box_enable("mdlchams_head_box_enable", "Renders box at players heads", false);
VarColor mdlchams_head_box_color("mdlchams_head_box_color", "Color of the player head box", CColor(0, 255, 0));

VarBoolean mdlchams_render_real_playermodel("mdlchams_render_real_playermodel", "Renders real playermodel over the current one", false);
VarBoolean mdlchams_disable_animations("mdlchams_disable_animations", "Disables animations on playermodels", false);

VarBoolean mdlchams_force_default_viewmodel("mdlchams_force_default_viewmodel", "Enforces default viewmodel", false);

std::array<ChammedModel, CHAMS_COUNT> CModelChams::m_chammed_models =
{ {
	// CHAMS_VIEWMODEL
	ChammedModel(
		&mdlchams_viewmodel_enable, &mdlchams_viewmodel_color, &mdlchams_viewmodel_type, &mdlchams_viewmodel_shadelight, &mdlchams_viewmodel_ambientlight,
		[](hl::cl_entity_t* current_ent) 
		{
			return current_ent == &CMemoryHookMgr::the().cl().get()->viewent;
		}),

	// CHAMS_PLAYERS_CT
	ChammedModel(
		&mdlchams_players_ct_enable, &mdlchams_players_ct_color, &mdlchams_players_ct_type, &mdlchams_players_ct_shadelight, &mdlchams_players_ct_ambientlight,
		[](hl::cl_entity_t* current_ent) 
		{
			if (current_ent->player)
			{
				auto player = CEntityMgr::the().get_player_by_id(current_ent->index);
				return player && (*player)->get_team() == hl::CT && current_ent->curstate.iuser4 == ENT_ID_NONE;
			}
			return false;
		}),

	// CHAMS_PLAYERS_T
	ChammedModel(
		&mdlchams_players_t_enable, &mdlchams_players_t_color, &mdlchams_players_t_type, &mdlchams_players_t_shadelight, &mdlchams_players_t_ambientlight,
		[](hl::cl_entity_t* current_ent) 
		{
			if (current_ent->player)
			{
				auto player = CEntityMgr::the().get_player_by_id(current_ent->index);
				return player && (*player)->get_team() == hl::TERRORIST && current_ent->curstate.iuser4 == ENT_ID_NONE;
			}
			return false;
		}),

	// CHAMS_BACKTRACK
	ChammedModel(
		&mdlchams_players_backtrack_enable, &mdlchams_players_backtrack_color, &mdlchams_players_backtrack_type, &mdlchams_players_backtrack_shadelight, &mdlchams_players_backtrack_ambientlight,
		[](hl::cl_entity_t* current_ent) 
		{
			return current_ent->player && current_ent->curstate.iuser4 == ENT_ID_BACKTRACK;
		}),
} };

void CModelChams::update_studio_drawpoints_pre()
{
	OX_PROFILE_SCOPE("studio_points_pre");

	m_studio_draw_points_lock = true;
	
	auto engine_studio_api = CMemoryHookMgr::the().engine_studio_api().get();
	auto current_ent = engine_studio_api->GetCurrentEntity();
	
	if (mdlchams_enable.get_value())
	{
		for (auto& chams : m_chammed_models)
		{
			if (chams.is_enabled() && chams.should_render(current_ent))
			{
				chams.process_studio_drawpoints_pre();
				break; // we're done here, this is called per model
			}
		}
	}
	else
	{
		if (mdlchams_render_real_playermodel.get_value() && !m_rendering_real_playermodel)
		{
			if (current_ent->player)
			{
				glBlendFunc(GL_ONE, GL_ONE);
				glEnable(GL_BLEND);
				glDepthMask(GL_FALSE);
			}
		}
	}
}

void CModelChams::update_studio_drawpoints_post()
{
	OX_PROFILE_SCOPE("studio_points_post");
	
	auto engine_studio_api = CMemoryHookMgr::the().engine_studio_api().get();
	auto current_ent = engine_studio_api->GetCurrentEntity();

	if (mdlchams_enable.get_value())
	{
		for (auto& chams : m_chammed_models)
		{
			if (chams.is_enabled() && chams.should_render(current_ent))
			{
				chams.process_studio_drawpoints_post();
				break; // we're done here, this is called per model
			}
		}
	}
	else
	{
		if (mdlchams_render_real_playermodel.get_value() && !m_rendering_real_playermodel)
		{
			if (current_ent->player)
			{
				glDisable(GL_BLEND);
				glDepthMask(GL_TRUE);
			}
		}
	}

	m_studio_draw_points_lock = false;
}

void CModelChams::update_studio_lighting(hl::alight_t* plighting)
{
	OX_PROFILE_SCOPE("studio_lighting");
	
	auto engine_studio_api = CMemoryHookMgr::the().engine_studio_api().get();
	auto current_ent = engine_studio_api->GetCurrentEntity();

	if (mdlchams_enable.get_value())
	{
		for (auto& chams : m_chammed_models)
		{
			if (chams.is_enabled() && chams.should_render(current_ent))
			{
				chams.process_studio_lighting(plighting);
				break; // we're done here, this is called per model
			}
		}
	}
}

void CModelChams::update_studio_colors(GLfloat* r, GLfloat* g, GLfloat* b, GLfloat* a)
{
	OX_PROFILE_SCOPE("modelchams_color");
	
	if (!mdlchams_enable.get_value())
	{
		return;
	}

	auto current_ent = CMemoryHookMgr::the().engine_studio_api().get()->GetCurrentEntity();
	if (!current_ent)
	{
		// since this is called inside glColor4f, this can be nullptr.
		return;
	}

	for (auto& chams : m_chammed_models)
	{
		if (chams.is_enabled() && chams.should_render(current_ent))
		{
			chams.process_color(r, g, b, a);
			break; // we're done here
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

	CColor color_based_on_team = CColor(170, 170, 170, 255); // gray by default
	if (player && (*player)->is_valid())
	{
		color_based_on_team = (*player)->get_color_based_on_team();
	}

	if (m_rendering_real_playermodel)
	{
		color_based_on_team = CColor(180, 196, 36, 255);
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
		glColor3f(color_based_on_team.r, color_based_on_team.g, color_based_on_team.b);

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

	CColor clr = mdlchams_head_box_color.get_value();

	if (m_rendering_real_playermodel)
	{
		clr = CColor(clr.g, clr.b, clr.r, clr.a); // to distinguish
	}

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

				CMath::the().vector_transform(bbox_transformator, (*couple_of_pointers)[bbox[i].bone], p[j]);
			}

			glBegin(GL_QUADS);
			
			glColor3f(clr.r, clr.g, clr.b);

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

void CModelChams::force_default_models()
{
	if (!mdlchams_force_default_viewmodel.get_value())
	{
		return;
	}

	auto cl = CMemoryHookMgr::the().cl().get();
	auto vm = &cl->viewent;

	auto current_weapon = CWeapons::the().get_current_weapon();
	if (current_weapon)
	{
		std::string model_name = current_weapon->m_model_name;

		if (!model_name.empty())
		{
			vm->model = CMemoryHookMgr::the().cl_enginefuncs()->pfnCL_LoadModel(model_name.c_str(), NULL);
		}
	}
}

//---------------------------------------------------------------------------------

void ChammedModel::process_studio_drawpoints_pre()
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

	switch (m_type->get_value())
	{
		case CHAMS_Flat:
		case CHAMS_FlatTercial:
		case CHAMS_FlatLightened:
		{
			glDisable(GL_TEXTURE_2D);
			break;
		}
		case CHAMS_Wireframe:
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
			glDisable(GL_TEXTURE_2D);
			break;
		}
		case CHAMS_Texture:
		{
			glEnable(GL_TEXTURE_2D);
			break;
		}
	}
}

void ChammedModel::process_studio_drawpoints_post()
{
	if (mdlchams_blend.get_value())
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	switch (m_type->get_value())
	{
		case CHAMS_Flat:
		case CHAMS_FlatTercial:
		case CHAMS_FlatLightened:
		{
			glEnable(GL_TEXTURE_2D);
			break;
		}
		case CHAMS_Wireframe:
		{
			glEnable(GL_TEXTURE_2D);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		}
		case CHAMS_Texture:
		{
			break;
		}
	}
}

void ChammedModel::process_color(GLfloat* r, GLfloat* g, GLfloat* b, GLfloat* a)
{
	if (!CModelChams::the().is_drawing_studio_model())
	{
		return;
	}

	if (CAntiScreen::the().hide_visuals() && !CPanic::the().panicking())
	{
		return;
	}

	auto color = m_color->get_value();
	
	if (mdlchams_rainbow.get_value())
	{
		float speed = (float)mdlchams_rainbow_speed.get_value();
		double tm = CMemoryHookMgr::the().cl().get()->time;

		// all phase-shifted so that it's more cool
		color.r = std::abs(std::sinf(tm * speed + 0));
		color.g = std::abs(std::sinf(tm * speed + 90));
		color.b = std::abs(std::sinf(tm * speed + 180));
	}
	
	switch (get_type())
	{
		case CHAMS_Flat:
		case CHAMS_Texture:
		case CHAMS_Wireframe:
		{
			*r = color.r;
			*g = color.g;
			*b = color.b;
			
			break;
		}
		case CHAMS_FlatTercial:
		{
			*r = color.r * *r;
			*g = color.g * *g;
			*b = color.b * *b;

			break;
		}
		case CHAMS_FlatLightened:
		{
			*r = color.r / (*r == 0.0f ? 0.01f : *r);
			*g = color.g / (*g == 0.0f ? 0.01f : *g);
			*b = color.b / (*b == 0.0f ? 0.01f : *b);

			break;
		}
	}
}

void ChammedModel::process_studio_lighting(hl::alight_t* plighting)
{
	int shade = m_shade->get_value();
	int ambient = m_ambient->get_value();

	if (shade != 0)
	{
		plighting->shadelight = shade;
	}

	if (ambient != 0)
	{
		plighting->ambientlight = ambient;
	}

	// some generic values that usually look good
	plighting->plightvec[0] = 0.366f;
	plighting->plightvec[1] = 0.341f;
	plighting->plightvec[2] = -0.866f;
}
