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
extern VarBoolean mdlchams_viewmodel_enable;
extern VarColor mdlchams_viewmodel_color;
extern VarInteger mdlchams_viewmodel_type;
extern VarBoolean mdlchams_players_t_enable;
extern VarColor mdlchams_players_t_color;
extern VarInteger mdlchams_players_t_type;
extern VarBoolean mdlchams_players_ct_enable;
extern VarColor mdlchams_players_ct_color;
extern VarInteger mdlchams_players_ct_type;
extern VarBoolean mdlchams_player_skeleton;
extern VarBoolean mdlchams_head_box_enable;
extern VarColor mdlchams_head_box_color;

enum EModelChamsType
{
	CHAMS_Flat,
	CHAMS_FlatTercial,
};

class ChammedModel
{
public:
	ChammedModel(VarBoolean* is_enabled, VarColor* color, VarInteger* type, const std::function<bool()>& should_render) :
		m_is_enabled(is_enabled), 
		m_color(color), 
		m_type(type), 
		m_should_render(should_render)
	{
	}

	ChammedModel() = default;

	bool is_enabled() const { return m_is_enabled && m_is_enabled->get_value() != false; }

	bool should_render() const { return m_should_render(); }

	EModelChamsType get_type() const { return (EModelChamsType)m_type->get_value(); }

	virtual void process_studio_pre();
	virtual void process_studio_post();
	
	virtual void process_color(float* lambert);

private:
	VarBoolean* m_is_enabled = nullptr;
	VarColor* m_color = nullptr;

	VarInteger* m_type = nullptr;

	std::function<bool()> m_should_render;
};

class CModelChams
{
public:
	DECL_BASIC_CLASS(CModelChams);

public:
	void initialize();

	void executeall_studio_pre();
	void executeall_studio_post();

	void executeall_color(float* lambert);

	// replacement for R_StudioDrawPoints()
	bool studio_draw_skeleton();

	void render_playerhead_hitbox();

	// returns model that is currently being rendered inside studio code.
	// this is set by the client dll using engine_studio_api_t::SetRenderModel().
	hl::model_t* get_currently_rendered_model();
	hl::studiohdr_t* get_currently_rendered_model_header();

private:
	void intitialize_chammed_model(ChammedModel* model, VarBoolean* is_enabled, VarColor* color, VarInteger* type, const std::function<bool()>& should_render);

	ChammedModel m_viewmodel;
	ChammedModel m_players_t, m_players_ct;

	std::vector<ChammedModel*> m_chammed_models;
};

#endif // MODELCHAMS_H