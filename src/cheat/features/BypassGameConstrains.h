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

#ifndef FORCEENABLEDISABLED_H
#define FORCEENABLEDISABLED_H
#pragma once

extern VarBoolean bypass_constrain_renderdist_enable;
extern VarInteger bypass_constrain_renderdist_value;
extern VarBoolean bypass_constrain_noclip;
extern VarBoolean bypass_constrain_renderer_cvars;
extern VarBoolean bypass_constrain_renderer_glclear;
extern VarBoolean bypass_constrain_sponly_cvars;

class CForceEnableDisabled
{
public:
	DECL_BASIC_CLASS(CForceEnableDisabled);

public:
	void force_enable_noclip_pre();
	void force_enable_noclip_post();

	std::optional<GLdouble> force_max_viewable_renderdistance();

	bool disable_renderer_cvar_constrain();
	void update_disable_sponly_cvars();

private:
	// helper for noclip
	float prev_dm;

	std::vector<std::string> m_saved_sponly_cvars;

	// renderer cvars
	void save_renderer_cvars();
	void restore_renderer_cvars();

	float prev_r_lightmap;
	float prev_gl_clear;
	float prev_r_novis;
	float prev_r_fullbright;
	float prev_snd_show;
	float prev_chase_active;
	float prev_gl_monolights;
	float prev_gl_wireframe;
	float prev_r_dynamic;
	float prev_gl_alphamin;
	float prev_gl_max_size;
	float prev_gl_polyoffset;
	float prev_r_drawentities;
	float prev_lightgamma;
	bool m_just_disabled_renderer_cvars = false;

	// sp-only cvars
	bool m_just_disabled_sponly_cvars = false;
};

#endif // FORCEENABLEDISABLED_H