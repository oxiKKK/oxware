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

#ifndef MOVEMENTVISUALINFO_H
#define MOVEMENTVISUALINFO_H
#pragma once

extern VarBoolean movement_visual_info;
extern VarBoolean movement_visual_info_keys;
extern VarFloat movement_visual_info_keys_offset;
extern VarBoolean movement_visual_info_background;

class CMovementVisualInfo
{
public:
	DECL_BASIC_CLASS(CMovementVisualInfo);

public:
	void render();

private:
	void render_values();
	void render_keys();

	// helpers
	void render_unit(const EngineFont& font, const CColor& color, const std::string& label, const std::string& value, const std::string& unit);
	void render_text(const EngineFont& font, const Vector2D& pos, const CColor& color, const std::string& label);

	Vector2D m_rendering_pos;

	bool m_background;
};

#endif // MOVEMENTVISUALINFO_H