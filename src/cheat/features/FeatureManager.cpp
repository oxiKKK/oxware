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

void CFeatureManager::initialize()
{
	CConsole::the().info("Initializing Feature Manager...");

	precache_features();

	m_initialized = true;
}

void CFeatureManager::shutdown()
{
	m_features.clear();

	m_initialized = false;
}

void CFeatureManager::add_new_feature(const BaseFeature& feature)
{
	m_features.push_back(feature);
	CConsole::the().info("'{}' assigned to variable '{}'", feature.get_name(), feature.get_variable()->get_name());
}

void CFeatureManager::precache_features()
{
	// esp
	add_new_feature(BaseFeature(&esp_enable, "ESP"));

	// fov
	add_new_feature(BaseFeature(&custom_fov, "Custom FOV"));

	// bypass constrain
	add_new_feature(BaseFeature(&bypass_constrain_renderdist_enable, "Render distance"));
	add_new_feature(BaseFeature(&bypass_constrain_noclip, "Re-enable Noclip"));
	add_new_feature(BaseFeature(&bypass_constrain_renderer_cvars, "Disable R_ForceCVars"));
	add_new_feature(BaseFeature(&bypass_constrain_sponly_cvars, "Disable SP-Only cvars"));

	// chams
	add_new_feature(BaseFeature(&mdlchams_enable, "Model chams"));
	add_new_feature(BaseFeature(&mdlchams_render_real_playermodel, "Render real model"));
	add_new_feature(BaseFeature(&mdlchams_head_box_enable, "Render player head box"));
	add_new_feature(BaseFeature(&mdlchams_player_skeleton, "Render player skeleton"));
	add_new_feature(BaseFeature(&mdlchams_disable_animations, "Disable player animations"));

	// removals
	add_new_feature(BaseFeature(&remove_screenshake, "Remove screenshake"));
	add_new_feature(BaseFeature(&remove_hud_enable, "HUD removal"));
	add_new_feature(BaseFeature(&remove_motd, "MOTD removal"));
	add_new_feature(BaseFeature(&remove_sniper_scope, "Sniper scope removal"));
	add_new_feature(BaseFeature(&remove_viewmodel, "Viewmodel removal"));

	// viewmodel offset
	add_new_feature(BaseFeature(&viewmodel_offset_enable, "Viewmodel offset"));

	// smoke visuals
	add_new_feature(BaseFeature(&smoke_visuals, "Smoke visuals"));

	// frame skip
	add_new_feature(BaseFeature(CMovement::the().strafe.get_toggle_var(), "Frame skip"));

	// cvar filter
	add_new_feature(BaseFeature(&cvar_sandbox_enable, "Server liar / cvar filter"));

	// command filter
	add_new_feature(BaseFeature(&cmdfilter_enable, "Command filter"));

	// crosshair
	add_new_feature(BaseFeature(&crosshair_enable, "Custom crosshair"));

	// HUD rendering
	add_new_feature(BaseFeature(&hud_color_enable, "HUD color"));
	add_new_feature(BaseFeature(&hud_render, "HUD rendering"));

	// Flashbang fade
	add_new_feature(BaseFeature(&flashfademod_enable, "Flashbang fade"));

	// InGame Screen rendering
	add_new_feature(BaseFeature(&ingamescreen_better_cl_showfps, "Better cl_showfps"));

	// Movement
	add_new_feature(BaseFeature(CMovement::bunnyhop.get_toggle_var(), "Bunnyhop"));
	add_new_feature(BaseFeature(CMovement::airstuck.get_toggle_var(), "Air stuck"));
	add_new_feature(BaseFeature(CMovement::gs.get_toggle_var(), "Ground-strafe"));
	add_new_feature(BaseFeature(CMovement::eb.get_toggle_var(), "Edge bug"));
	add_new_feature(BaseFeature(CMovement::strafe.get_toggle_var(), "Strafe hack"));
	add_new_feature(BaseFeature(CMovement::strafe_helper.get_toggle_var(), "Strafe helper"));
	add_new_feature(BaseFeature(CMovement::fastrun.get_toggle_var(), "Fast run"));
	add_new_feature(BaseFeature(CMovement::auto_jof.get_toggle_var(), "Auto JOF"));
	add_new_feature(BaseFeature(&movement_plot, "Movement visualization"));

	// debug
	add_new_feature(BaseFeature(&debug, "Debug"));

	// thirdperson
	add_new_feature(BaseFeature(&thirdperson_dist, 0, "Thirdperson"));

	// non-steam fps fix
	add_new_feature(BaseFeature(&nonsteam_fps_fix, "Non-steam FPS fix"));

	// consistency bypass
	add_new_feature(BaseFeature(&consistencybypass_enable, "Consistency bypass"));

	// anti demo recorder
	add_new_feature(BaseFeature(&anti_demo_recorder_enable, "Anti demo recorder"));

	// enveff
	add_new_feature(BaseFeature(&env_enable, "Environmental effects"));

	// world visuals
	add_new_feature(BaseFeature(&world_visuals_enable, "World visuals"));
	add_new_feature(BaseFeature(&world_visuals_fog, "World fog"));

	// backtrack
	add_new_feature(BaseFeature(&backtrack_enable, "Backtrack"));

	// speed control
	add_new_feature(BaseFeature(&speed_control_enable, "Speed control"));

	// fake latency
	add_new_feature(BaseFeature(&fake_latency, "Fake latency"));

	CConsole::the().info("Precached all features.");
}
