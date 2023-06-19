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

VarBoolean world_visuals_enable("world_visuals_enable", "Enables world visuals", false);
VarInteger world_visuals_dimlight("world_visuals_dimlight", "Brightness of the world", 255, 0, 255);
VarBoolean world_visuals_rainbow("world_visuals_rainbow", "Rainbow world", false);

void CWorldVisuals::update_gl_begin()
{
	if (!world_visuals_enable.get_value())
	{
		return;
	}

	CColor color = CColor(255, 255, 255, 255);
	if (world_visuals_rainbow.get_value())
	{
		float speed = (float)1.0f;
		double tm = CMemoryHookMgr::the().cl().get()->time;

		// all phase-shifted so that it's more cool
		color.r = std::abs(std::sinf(tm * speed + 0));
		color.g = std::abs(std::sinf(tm * speed + 90));
		color.b = std::abs(std::sinf(tm * speed + 180));
	}

	float brightness = (float)world_visuals_dimlight.get_value() / 255;
	if (brightness != 1.0f)
	{
		color.r *= brightness;
		color.g *= brightness;
		color.b *= brightness;
	}

	glColor4f(color.r, color.g, color.b, 1.0f);
}
