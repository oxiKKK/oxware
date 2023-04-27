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

#ifndef VANILLACROSSHAIR_H
#define VANILLACROSSHAIR_H
#pragma once

extern VarBoolean crosshair_enable;
extern VarBoolean crosshair_dynamic;
extern VarBoolean crosshair_translucent;
extern VarBoolean crosshair_static;
extern VarInteger crosshair_size;
extern VarInteger crosshair_gap;
extern VarInteger crosshair_thickness;
extern VarInteger crosshair_type;
extern VarColor crosshair_color;

class CVanillaCrosshair
{
public:
	DECL_BASIC_CLASS(CVanillaCrosshair);

public:
	void draw();

private:
	float get_weapon_max_speed_for_crosshair(int weapon_id);

	int scaled_base(int screen_width);

	int m_ammo_last_check = 0;
	float m_crosshair_gap = 0.0f;
	int m_alpha;
};

#endif // VANILLACROSSHAIR_H