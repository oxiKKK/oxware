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

VarBoolean smoke_visuals("smoke_visuals", "Enables smoke visuals", false);
VarColor smoke_color("smoke_color", "Forces this smoke color on creation", CColor(170, 170, 170));
VarBoolean smoke_rainbow("smoke_rainbow", "Forces rainbow smoke color", false);
VarInteger smoke_opacity("smoke_opacity", "Makes the smoke more/less transparent (value in percentage)", 100, 10, 100);

void CSmokeVisuals::on_smoke_particleeffect_create(hl::CPartSmokeGrenade* current_smoke)
{
	m_last_smoke = current_smoke;
}

void CSmokeVisuals::post_gas_smoke_create()
{
	if (!m_last_smoke)
	{
		return;
	}

	if (!smoke_visuals.get_value())
	{
		return;
	}

	m_last_smoke->m_flSize *= (smoke_opacity.get_value() / 100.0f);
	m_last_smoke->m_flBrightness *= (smoke_opacity.get_value() / 100.0f);

	if (smoke_rainbow.get_value())
	{
		m_last_smoke->m_vColor.x = (float)(rand() % 255);
		m_last_smoke->m_vColor.y = (float)(rand() % 255);
		m_last_smoke->m_vColor.z = (float)(rand() % 255);
	}
	else
	{
		m_last_smoke->m_vColor.x = smoke_color.get_value().r * 255.0f;
		m_last_smoke->m_vColor.y = smoke_color.get_value().g * 255.0f;
		m_last_smoke->m_vColor.z = smoke_color.get_value().b * 255.0f;
	}
}
