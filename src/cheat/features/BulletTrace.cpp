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

void CBulletTrace::initialize_gui()
{
	COxWareUI::the().add_background_rendering_code([this]() { on_render(); });
	COxWareUI::the().add_background_rendering_constrain(
		[this]()
		{
			return !m_bullets_fired.empty();
		});
}

void CBulletTrace::on_bullet_fired(
	int entid, const Vector& forward, const Vector& right, const Vector& up, const Vector& source, 
	const Vector& shooting_direction, const Vector& spread, float penetration_dist)
{
	Vector dir_with_spread = calc_bullet_shoot_dir_vector(shooting_direction, spread, right, up);

	Vector bullet_impact = calc_bullet_endpos(source, dir_with_spread, penetration_dist);
	Vector traced_bullet_impact = calc_traced_bullet_endpos(source, bullet_impact);

	CConsole::the().info("traced_bullet_impact: {}", traced_bullet_impact);

	m_bullets_fired.push_back({ source, traced_bullet_impact, GetTickCount() });
}

void CBulletTrace::on_render()
{
	if (!m_bullets_fired.empty())
	{
		auto& oldest_bullet = m_bullets_fired.front();
	
		if (oldest_bullet.get_life_duration_ms() > 5000)
		{
			m_bullets_fired.pop_front();
		}
	}

	for (const auto& bullet : m_bullets_fired)
	{
		Vector2D screen_start, screen_end;
		if (CGameUtil::the().world_to_screen(bullet.start_org, screen_start) &&
			CGameUtil::the().world_to_screen(bullet.end_org, screen_end))
		{
			g_gui_window_rendering_i->render_line(
				g_gui_window_rendering_i->get_current_drawlist(),
				screen_start, screen_end,
				CColor(0, 230, 0, 230));
		}
	}
}

Vector CBulletTrace::calc_bullet_shoot_dir_vector(
	const Vector& original_shootdir, const Vector& spread, const Vector& right, const Vector& up)
{
	// apply spread to the final direction
	return original_shootdir + (spread.x * right + spread.y * up);
}

Vector CBulletTrace::calc_bullet_endpos(const Vector& source, const Vector& shootdir_with_spread, float penetration_dist)
{
	// calculate bullet impact position
	return source + shootdir_with_spread * penetration_dist;
}

Vector CBulletTrace::calc_traced_bullet_endpos(const Vector& source, const Vector& end)
{
	auto pm = *CMemoryHookMgr::the().pmove().get();
	auto trace = pm->PM_TraceLine((float*)&source, (float*)&end, PM_TRACELINE_ANYVISIBLE, pm->usehull, -1);

	return trace->endpos;
}
