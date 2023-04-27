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

VarBoolean crosshair_enable("crosshair_enable", "Enables vanilla-line crosshair rendering", false);
VarBoolean crosshair_dynamic("crosshair_dynamic", "Dynamically sized crosshair", false);
VarBoolean crosshair_translucent("crosshair_translucent", "Dynamically translucent crosshair (transparent)", false);
VarBoolean crosshair_static("crosshair_static", "Makes the crosshair static while shooting", false);
VarInteger crosshair_size("crosshair_size", "Size of the crosshair", 10, 0, 50);
VarInteger crosshair_gap("crosshair_gap", "Size of the gap between bars of the crosshair", 10, 0, 50);
VarInteger crosshair_thickness("crosshair_thickness", "Thickness of the line that represents crosshair", 1, 1, 5);
VarInteger crosshair_type("crosshair_type", "Changes crosshair type (regular, T-shaped, circle)", 0, 0, 2);
VarColor crosshair_color("crosshair_color", "Color of the crosshair", CColor(0, 255, 0));

constexpr static int g_gap_size_by_weapon_index[MAX_WEAPONS][2] =
{
	{ 4, 3 }, // 0  -> WEAPON_NONE
	{ 8, 3 }, // 1	-> WEAPON_P228
	{ 4, 3 }, // 2	-> WEAPON_GLOCK
	{ 8, 3 }, // 3	-> WEAPON_SCOUT
	{ 8, 3 }, // 4	-> WEAPON_HEGRENADE
	{ 9, 4 }, // 5	-> WEAPON_XM1014
	{ 6, 3 }, // 6	-> WEAPON_C4
	{ 9, 3 }, // 7	-> WEAPON_MAC10
	{ 3, 3 }, // 8	-> WEAPON_AUG
	{ 8, 3 }, // 9	-> WEAPON_SMOKEGREN
	{ 4, 3 }, // 10	-> WEAPON_ELITE
	{ 8, 3 }, // 11	-> WEAPON_FIVESEVEN
	{ 6, 3 }, // 12	-> WEAPON_UMP45
	{ 4, 3 }, // 13	-> WEAPON_SG550
	{ 4, 3 }, // 14	-> WEAPON_GALIL
	{ 4, 3 }, // 15	-> WEAPON_FAMAS
	{ 8, 3 }, // 16	-> WEAPON_USP
	{ 8, 3 }, // 17	-> WEAPON_GLOCK18
	{ 8, 3 }, // 18	-> WEAPON_AWP
	{ 6, 2 }, // 19	-> WEAPON_MP5N
	{ 6, 3 }, // 20	-> WEAPON_M249
	{ 8, 6 }, // 21	-> WEAPON_M3
	{ 4, 3 }, // 22	-> WEAPON_M4A1
	{ 7, 3 }, // 23	-> WEAPON_TMP
	{ 6, 4 }, // 24	-> WEAPON_G3SG1
	{ 8, 3 }, // 25	-> WEAPON_FLASHBANG
	{ 8, 3 }, // 26	-> WEAPON_DEAGLE
	{ 5, 3 }, // 27	-> WEAPON_SG552
	{ 4, 4 }, // 28	-> WEAPON_AK47
	{ 7, 3 }, // 29	-> WEAPON_KNIFE
	{ 7, 3 }, // 30	-> WEAPON_P90
};

float CVanillaCrosshair::get_weapon_max_speed_for_crosshair(int weapon_id)
{
	switch (weapon_id)
	{
		case hl::WEAPON_AUG:
		case hl::WEAPON_M4A1:
		case hl::WEAPON_FAMAS:
		case hl::WEAPON_SG550:
		case hl::WEAPON_GALIL:
		case hl::WEAPON_AK47:
		case hl::WEAPON_M249:
		case hl::WEAPON_SG552:
		{
			return 140.f;
		}
		case hl::WEAPON_P90:
		{
			return 170.f;
		}
	}

	return 0;
}

#define SCALEDBASE_SMALL	1024
#define SCALEDBASE_MEDIUM	800
#define SCALEDBASE_LARGE	640

int CVanillaCrosshair::scaled_base(int screen_width)
{
	char* crosshair_size = CMemoryHookMgr::the().cl_enginefuncs().get()->pfnGetCvarString((char*)"cl_crosshair_size");

	if (crosshair_size != NULL)
	{
		if (!stricmp(crosshair_size, "auto"))
		{
			if (screen_width < 640)
			{
				return SCALEDBASE_SMALL;
			}
			else if (screen_width < 1024)
			{
				return SCALEDBASE_MEDIUM;
			}
		}
		else if (!stricmp(crosshair_size, "small"))
		{
			return SCALEDBASE_SMALL;
		}
		else if (!stricmp(crosshair_size, "medium"))
		{
			return SCALEDBASE_MEDIUM;
		}
		else if (!stricmp(crosshair_size, "large"))
		{
			return SCALEDBASE_LARGE;
		}
	}

	return SCALEDBASE_LARGE;
}

void CVanillaCrosshair::draw()
{
	auto current_weapon = CGameUtil::the().get_current_weapon();
	if (!current_weapon)
	{
		return;
	}

	// for time-based calculations, rather then fps-based
	static auto prev_cross_time = std::chrono::high_resolution_clock::now();

	// simulate 100fps frametimes...
	bool flip_every_1_ms = false;
	if (std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - prev_cross_time).count() > 10)
	{
		flip_every_1_ms = true;
		prev_cross_time = std::chrono::high_resolution_clock::now();
	}

	auto screen_info = CGameUtil::the().get_engine_screen_info();

	int gap = 0, delta_gap = 0;

	gap = g_gap_size_by_weapon_index[current_weapon->m_iId][0];
	delta_gap = g_gap_size_by_weapon_index[current_weapon->m_iId][1];

	bool use_vanilla_gap = (crosshair_gap.get_value() == 0);

	// apply settings on the gap
	gap = use_vanilla_gap ? gap : crosshair_gap.get_value();

	int accuracy_flags = CGameUtil::the().get_weapon_accuracy_flags(current_weapon->m_iId, current_weapon->m_iWeaponState);

	// dynamic crosshair, based on movement, playermove flags, weapon velocity, etc.
	if (accuracy_flags != ACCURACY_NONE && crosshair_dynamic.get_value() != 0)
	{
		int player_flags = CGameUtil::the().get_player_flags();

		if ((player_flags & FL_ONGROUND) || !(accuracy_flags & ACCURACY_AIR))
		{
			if ((player_flags & FL_DUCKING) && accuracy_flags & ACCURACY_DUCK)
			{
				gap *= 0.5f;
			}
			else if (CGameUtil::the().get_local_velocity_2d() > get_weapon_max_speed_for_crosshair(current_weapon->m_iId) && (accuracy_flags & ACCURACY_SPEED))
			{
				gap *= 1.5f;
			}
		}
		else
		{
			gap *= 2.0f;
		}

		if (accuracy_flags & ACCURACY_MULTIPLY_BY_14)
		{
			gap *= 1.4f;
		}

		if (accuracy_flags & ACCURACY_MULTIPLY_BY_14_2)
		{
			gap *= 1.4f;
		}
	}

	if (!crosshair_static.get_value())
	{
		auto shots_fired_ptr = CMemoryHookMgr::the().g_iShotsFired().get();

		if (m_ammo_last_check >= *shots_fired_ptr)
		{
			if (flip_every_1_ms)
			{
				m_crosshair_gap -= (0.013f * m_crosshair_gap + 0.1f);
			}
			m_alpha += 2;
		}
		else
		{
			m_crosshair_gap += delta_gap;
			m_crosshair_gap =
				std::min(m_crosshair_gap,
						 use_vanilla_gap ? 15.0f : (crosshair_gap.get_value() * 3.0f));

			m_alpha -= 40;
			m_alpha = std::max(m_alpha, 120);
		}

		// reset the counter if it "overflows".
		if (*shots_fired_ptr > 600)
		{
			*shots_fired_ptr = 1;
		}
		m_ammo_last_check = *shots_fired_ptr;
	}

	// clamp before we render
	if (m_crosshair_gap <= gap)
	{
		m_crosshair_gap = gap;
	}
	if (m_alpha > 255)
	{
		m_alpha = 255;
	}

	int bar_size = (crosshair_size.get_value() == 0) ? ((m_crosshair_gap - gap) / 2.0f) + 5 : crosshair_size.get_value();
	float crosshair_dist = m_crosshair_gap;

	CColor cross_color = crosshair_color.get_value();

	int scaled = scaled_base(screen_info.iWidth);
	if (screen_info.iWidth != scaled)
	{
		bar_size = screen_info.iWidth * bar_size / scaled;
		crosshair_dist = screen_info.iWidth * crosshair_dist / scaled;
	}

	int r = cross_color.r * 255.0f;
	int g = cross_color.g * 255.0f;
	int b = cross_color.b * 255.0f;
	int a = m_alpha;

	int w = screen_info.iWidth;
	int h = screen_info.iHeight;

	int type = crosshair_type.get_value();

	auto fillrgba = crosshair_translucent.get_value() ? CMemoryHookMgr::the().cl_enginefuncs().get()->pfnFillRGBA : CMemoryHookMgr::the().cl_enginefuncs().get()->pfnFillRGBABlend;

	int t = crosshair_thickness.get_value();

	if (type == 0)
	{
		fillrgba(w / 2 - crosshair_dist - bar_size + 1, h / 2, bar_size, t, r, g, b, a);	// left
		fillrgba(w / 2 + crosshair_dist, h / 2, bar_size, t, r, g, b, a);					// right
		fillrgba(w / 2, h / 2 - crosshair_dist - bar_size + 1, t, bar_size, r, g, b, a);	// top
		fillrgba(w / 2, h / 2 + crosshair_dist, t, bar_size, r, g, b, a);					// bottom
	}
	else if (type == 1) // T-shaped
	{
		fillrgba(w / 2 - crosshair_dist - bar_size + 1, h / 2, bar_size, t, r, g, b, a);	// left
		fillrgba(w / 2 + crosshair_dist, h / 2, bar_size, t, r, g, b, a);					// right
		// top one missing
		fillrgba(w / 2, h / 2 + crosshair_dist, t, bar_size, r, g, b, a);					// bottom
	}
	else if (type == 2) // circle
	{
#if 0
		float step = (std::numbers::pi * 2) / 100;

		for (float angle = 0.0f; angle < std::numbers::pi * 2; angle += step)
		{
			float x = (w / 2) + (10 * cosf(angle));
			float y = (h / 2) + (10 * sinf(angle));

			fillrgba(x, y, 1, 1, r, g, b, a);	// left
		}
#else
		// would be inefficient to use fillRGBA for this.
		int segments = std::clamp((int)(crosshair_dist / 3), 16, 64);
		CGameUtil::the().render_circle_opengl(w / 2, h / 2, crosshair_dist, segments, t, !crosshair_translucent.get_value(), r, g, b, a);
#endif
	}
#if 0 // little silly, would need some work xD
	else if (type == 3) // spinner
	{
		auto render_spinner_line = [&](int delta_angle)
		{
			int segments = std::clamp((int)(crosshair_dist / 3), 16, 64);
			float rad1 = 10.0f, rad2 = 20.0f;

			// would be inefficient to use fillRGBA for this.
			static int ii = 0, ms = 0;
			if (GetTickCount() > ms + 100)
			{
				if (++ii > segments)
					ii = 0;

				ms = GetTickCount();
			}
			float theta = 2.0f * std::numbers::pi * float(ii) / float(segments); // get the current angle

			float x1 = w / 2 + rad1 * cosf(theta + delta_angle);
			float y1 = h / 2 + rad1 * sinf(theta + delta_angle);

			float x2 = w / 2 + rad2 * cosf(theta + delta_angle);
			float y2 = h / 2 + rad2 * sinf(theta + delta_angle);

			CGameUtil::the().render_line_opengl({ x1, y1 }, { x2, y2 }, t, !crosshair_translucent.get_value(), r, g, b, a);
		};

		render_spinner_line(90 * 0);
		render_spinner_line(90 * 1);
		render_spinner_line(90 * 2);
	}
#endif
}