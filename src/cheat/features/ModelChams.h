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

#ifndef MODELCHAMS_H
#define MODELCHAMS_H
#pragma once

extern VarBoolean mdlchams_enable;

extern VarBoolean mdlchams_flatshaded;
extern VarBoolean mdlchams_blend;

extern VarBoolean mdlchams_rainbow;
extern VarInteger mdlchams_rainbow_speed;

extern VarBoolean mdlchams_viewmodel_enable;
extern VarColor mdlchams_viewmodel_color;
extern VarInteger mdlchams_viewmodel_type;
extern VarInteger mdlchams_viewmodel_shadelight;
extern VarInteger mdlchams_viewmodel_ambientlight;

extern VarBoolean mdlchams_players_t_enable;
extern VarColor mdlchams_players_t_color;
extern VarInteger mdlchams_players_t_type;
extern VarInteger mdlchams_players_t_shadelight;
extern VarInteger mdlchams_players_t_ambientlight;

extern VarBoolean mdlchams_players_ct_enable;
extern VarColor mdlchams_players_ct_color;
extern VarInteger mdlchams_players_ct_type;
extern VarInteger mdlchams_players_ct_shadelight;
extern VarInteger mdlchams_players_ct_ambientlight;

extern VarBoolean mdlchams_players_backtrack_enable;
extern VarColor mdlchams_players_backtrack_color;
extern VarInteger mdlchams_players_backtrack_type;
extern VarInteger mdlchams_players_backtrack_shadelight;
extern VarInteger mdlchams_players_backtrack_ambientlight;

extern VarBoolean mdlchams_player_skeleton;

extern VarBoolean mdlchams_head_box_enable;
extern VarColor mdlchams_head_box_color;

extern VarBoolean mdlchams_render_real_playermodel;

extern VarBoolean mdlchams_disable_animations;

extern VarBoolean mdlchams_force_default_viewmodel;

enum EModelChamsType
{
	CHAMS_Flat,
	CHAMS_FlatTercial,
	CHAMS_FlatLightened,
	CHAMS_Texture,
	CHAMS_Wireframe,
};

using ShouldRenderFn = bool(*)(hl::cl_entity_t* current_ent);

class ChammedModel
{
public:
	ChammedModel(VarBoolean* is_enabled, VarColor* color, VarInteger* type, VarInteger* shade, VarInteger* ambient,
				ShouldRenderFn should_render) :
		m_is_enabled(is_enabled), 
		m_color(color), 
		m_type(type), 
		m_shade(shade),
		m_ambient(ambient),
		m_should_render(should_render)
	{
	}

	ChammedModel() = default;

	bool is_enabled() const { return m_is_enabled && m_is_enabled->get_value() != false; }

	bool should_render(hl::cl_entity_t* current_ent) const { return m_should_render(current_ent); }

	EModelChamsType get_type() const { return (EModelChamsType)m_type->get_value(); }

	virtual void process_studio_drawpoints_pre();
	virtual void process_studio_drawpoints_post();
	virtual void process_studio_lighting(hl::alight_t* plighting);
	
	virtual void process_color(GLfloat* r, GLfloat* g, GLfloat* b, GLfloat* a);

private:
	VarBoolean* m_is_enabled = nullptr;
	VarColor* m_color = nullptr;

	VarInteger* m_shade= nullptr, *m_ambient = nullptr;

	VarInteger* m_type = nullptr;

	ShouldRenderFn m_should_render;
};

enum EChammedModel
{
	CHAMS_VIEWMODEL,
	CHAMS_PLAYERS_CT,
	CHAMS_PLAYERS_T,
	CHAMS_BACKTRACK,

	CHAMS_COUNT
};

class CModelChams
{
public:
	DECL_BASIC_CLASS(CModelChams);

public:
	void update_studio_drawpoints_pre();
	void update_studio_drawpoints_post();
	void update_studio_lighting(hl::alight_t* plighting);
	void update_studio_colors(GLfloat* r, GLfloat* g, GLfloat* b, GLfloat* a);

	// replacement for R_GLStudioDrawPoints()
	bool studio_draw_skeleton();

	void render_playerhead_hitbox();

	void force_default_models();

	// returns model that is currently being rendered inside studio code.
	// this is set by the client dll using engine_studio_api_t::SetRenderModel().
	hl::model_t* get_currently_rendered_model();
	hl::studiohdr_t* get_currently_rendered_model_header();

	void toggle_rendering_real_playermodel() { m_rendering_real_playermodel ^= 1; }

	bool is_drawing_studio_model() const { return m_studio_draw_points_lock; }

private:
	ChammedModel m_viewmodel;
	ChammedModel m_players_t, m_players_ct;
	ChammedModel m_players_backtrack;

	static std::array<ChammedModel, CHAMS_COUNT> m_chammed_models;

	bool m_rendering_real_playermodel = false;

	// set to true of we're execuging R_StudioDrawPoints code right now
	bool m_studio_draw_points_lock = false;
};

#endif // MODELCHAMS_H