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

VarBoolean ingamescreen_better_cl_showfps("ingamescreen_better_cl_showfps", "Enhances cl_showfps cvar", false);
VarBoolean ingamescreen_better_cl_showfps_background("ingamescreen_better_cl_showfps_background", "Renders background over the cl_showfps text", false);
VarInteger ingamescreen_better_cl_showfps_position("ingamescreen_better_cl_showfps_position", "Position where the text will be rendered", 0, 0, 3);

void CIngameScreenRendering::better_cl_showfps()
{
	if (!ingamescreen_better_cl_showfps.get_value())
	{
		if (cl_showfps_was_on)
		{
			cl_showfps->value = 1.0f;
			strcpy(cl_showfps->string, "1");
			cl_showfps_was_on = false;
		}

		return;
	}

	auto enginefuncs = CMemoryHookMgr::the().cl_enginefuncs().get();
	auto cl = CMemoryHookMgr::the().cl().get();
	auto cls = CMemoryHookMgr::the().cls().get();

	if (!cl_showfps)
	{
		cl_showfps = CGoldSrcCommandMgr::the().get_cvar("cl_showfps");
	}

	// cvar is on
	if (cl_showfps->value != 0.0f)
	{
		cl_showfps_was_on = true;
		cl_showfps->value = 0.0f;
		strcpy(cl_showfps->string, "0");
	}

	auto level_name = cl->levelname;
	bool is_on_map = level_name[0];

	double ft = CGameUtil::the().get_engine_frametime();

#define FPS_AVG_FRAC 0.9 // how fast the number changes
	static double rolling_ft = 0.0;
	rolling_ft = FPS_AVG_FRAC * rolling_ft + (1.0 - FPS_AVG_FRAC) * ft;
	double fps = 1.0 / rolling_ft;

	auto label = std::format("{:.3f} fps ({:.3f} ms)", fps, rolling_ft * 1000.0);

	if (is_on_map)
	{
		label += " | ";
		label += level_name;
	}

	auto screen_info = CGameUtil::the().get_engine_screen_info();
	int w = screen_info.iWidth;
	int h = screen_info.iHeight;

	// label width/height based on the font
	int lw = CEngineRendering::the().calculate_width_for_console_text(label);
	int lh = CEngineRendering::the().get_console_text_height();

	Vector2D pos = {};
	switch (ingamescreen_better_cl_showfps_position.get_value())
	{
		case 0: pos = Vector2D(0, 0); break;			// top left
		case 1: pos = Vector2D(w - lw, 0); break;		// top right
		case 2: pos = Vector2D(w - lw, h - lh); break;	// bottom right
		case 3: pos = Vector2D(0, h - lh); break;		// bottom left
	}

	CEngineRendering::the().render_engine_text(pos, ingamescreen_better_cl_showfps_background.get_value(),
											   CColor(255, 255, 255), label, lw);
}

void CIngameScreenRendering::better_cl_showfps_on_unload()
{
	// restore original cl_showfps state

	if (ingamescreen_better_cl_showfps.get_value())
	{
		if (cl_showfps_was_on)
		{
			cl_showfps->value = 1.0f;
			strcpy(cl_showfps->string, "1");
			cl_showfps_was_on = false;
		}
	}
}

void CIngameScreenRendering::shutdown()
{
	better_cl_showfps_on_unload();

	cl_showfps = nullptr;
	cl_showfps_was_on = false;
}
