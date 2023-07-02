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

//
// SpriteManager.cpp -- Sprite management.
// 
// Note: This code is taken from my old cheat, you could probably parse these sprites out of the actual files, but I don't
//		 want to dive into that now.
//

#include "precompiled.h"

#define MIN_ALPHA		100
#define RGB_GREENISH	CColor(0, 160, 0)
#define RGB_ORANGEISH	CColor(255, 160, 0)
#define RGB_REDDISH		CColor(255, 16, 16)

VarBoolean hud_render("hud_render", "Enables custom HUD sprite rendering", false);
VarBoolean hud_render_current_weapon("hud_render_current_weapon", "Render currently held weapon on the screen", false);
VarBoolean hud_render_velocity("hud_render_velocity", "Render current velocity", false);

VarBoolean hud_color_enable("hud_color_enable", "Changes the in-game HUD color", false);
VarColor hud_color("hud_color", "Specifies the hud color in rgb", RGB_GREENISH);

std::unordered_map<std::string, SpriteAtlas_t> CSpriteMgr::s_sprite_atlas_map =
{
	{ "640hud1",		{ "sprites/640hud1.spr"			} },
	{ "640hud2",		{ "sprites/640hud2.spr"			} },
	{ "640hud3",		{ "sprites/640hud3.spr"			} },
	{ "640hud4",		{ "sprites/640hud4.spr"			} },
	{ "640hud5",		{ "sprites/640hud5.spr"			} },
	{ "640hud6",		{ "sprites/640hud6.spr"			} },
	{ "640hud7",		{ "sprites/640hud7.spr"			} },
	{ "640hud8",		{ "sprites/640hud8.spr"			} },
	{ "640hud9",		{ "sprites/640hud9.spr"			} },
	{ "640hud16",		{ "sprites/640hud16.spr"		} },
	{ "radar640",		{ "sprites/radar640.spr"		} },
	{ "radaropaque640",	{ "sprites/radaropaque640.spr"	} },
	{ "crosshairs",		{ "sprites/crosshairs.spr"		} },
};

std::unordered_map<std::string, Sprite_t> CSpriteMgr::s_sprite_map =
{
	// 640hud1
	{ "d_knife",		{ "d_knife",		{ 192, 0   }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_ak47",			{ "d_ak47",			{ 192, 80  }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_awp",			{ "d_awp",			{ 192, 128 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_deagle",		{ "d_deagle",		{ 224, 16  }, { 32, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_flashbang",	{ "d_flashbang",	{ 192, 192 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_g3sg1",		{ "d_g3sg1",		{ 192, 144 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_glock18",		{ "d_glock18",		{ 192, 16  }, { 32, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_grenade",		{ "d_grenade",		{ 224, 192 }, { 32, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_m249",			{ "d_m249",			{ 192, 160 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_m3",			{ "d_m3",			{ 192, 48  }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_m4a1",			{ "d_m4a1",			{ 192, 96  }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_mp5navy",		{ "d_mp5navy",		{ 192, 64  }, { 32, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_p228",			{ "d_p228",			{ 224, 32  }, { 32, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_p90",			{ "d_p90",			{ 192, 176 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_scout",		{ "d_scout",		{ 192, 208 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_sg552",		{ "d_sg552",		{ 192, 112 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_usp",			{ "d_usp",			{ 192, 32  }, { 32, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_tmp",			{ "d_tmp",			{ 224, 64  }, { 32, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_xm1014",		{ "d_xm1014",		{ 192, 224 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_skull",		{ "d_skull",		{ 224, 240 }, { 32, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_tracktrain",	{ "d_tracktrain",	{ 192, 240 }, { 32, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_aug",			{ "d_aug",			{ 148, 240 }, { 44, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_mac10",		{ "d_mac10",		{ 109, 240 }, { 34, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_elite",		{ "d_elite",		{ 52 , 240 }, { 57, 16 }, &s_sprite_atlas_map.at("640hud1") } },
	{ "d_headshot",		{ "d_headshot",		{ 0  , 240 }, { 36, 16 }, &s_sprite_atlas_map.at("640hud1") } },

	// 640hud2
	{ "d_famas",		{ "d_famas",		{ 192, 144 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud2") } },
	{ "d_galil",		{ "d_galil",		{ 192, 160 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud2") } },
	{ "item_battery",	{ "item_battery",	{ 176, 0   }, { 44, 44 }, &s_sprite_atlas_map.at("640hud2") } },
	{ "item_healthkit",	{ "item_healthkit",	{ 176, 48  }, { 44, 44 }, &s_sprite_atlas_map.at("640hud2") } },
	{ "item_longjump",	{ "item_longjump",	{ 176, 96  }, { 44, 44 }, &s_sprite_atlas_map.at("640hud2") } },

	// 640hud3 
	{ "selection",		{ "selection",		{ 0, 180 }, { 170, 45 }, &s_sprite_atlas_map.at("640hud3") } },

	// 640hud4
	{ "title_half",		{ "title_half",		{ 0, 226 }, { 256, 30 }, &s_sprite_atlas_map.at("640hud4") } },

	// 640hud5
	{ "train_back",		{ "train_back",		{ 170, 0   }, { 72 , 72 }, &s_sprite_atlas_map.at("640hud5") } },
	{ "train_stop",		{ "train_stop",		{ 170, 72  }, { 72 , 72 }, &s_sprite_atlas_map.at("640hud5") } },
	{ "train_forward1",	{ "train_forward1",	{ 170, 144 }, { 72 , 72 }, &s_sprite_atlas_map.at("640hud5") } },
	{ "title_life",		{ "title_life",		{ 0  , 226 }, { 220, 30 }, &s_sprite_atlas_map.at("640hud5") } },

	// 640hud6
	{ "train_forward2", { "train_forward2", { 170, 0  }, { 72, 72 }, &s_sprite_atlas_map.at("640hud6") } },
	{ "train_forward3", { "train_forward3", { 96 , 72 }, { 24, 24 }, &s_sprite_atlas_map.at("640hud6") } },

	// 640hud7
	{ "bucket1",		{ "bucket1",		{ 168, 72  }, { 20, 20 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "bucket2",		{ "bucket2",		{ 188, 72  }, { 20, 20 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "bucket3",		{ "bucket3",		{ 208, 72  }, { 20, 20 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "bucket4",		{ "bucket4",		{ 168, 92  }, { 20, 20 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "bucket5",		{ "bucket5",		{ 188, 92  }, { 20, 20 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "bucket0",		{ "bucket0",		{ 208, 92  }, { 20, 20 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "number_0",		{ "number_0",		{ 0  , 0   }, { 20, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "number_1",		{ "number_1",		{ 23 , 0   }, { 20, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "number_2",		{ "number_2",		{ 47 , 0   }, { 20, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "number_3",		{ "number_3",		{ 70 , 0   }, { 20, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "number_4",		{ "number_4",		{ 95 , 0   }, { 20, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "number_5",		{ "number_5",		{ 119, 0   }, { 20, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "number_6",		{ "number_6",		{ 144, 0   }, { 20, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "number_7",		{ "number_7",		{ 169, 0   }, { 20, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "number_8",		{ "number_8",		{ 192, 0   }, { 20, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "number_9",		{ "number_9",		{ 216, 0   }, { 20, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "divider",		{ "divider",		{ 240, 0   }, { 2,  40 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "cross",			{ "cross",			{ 48 , 25  }, { 24, 24 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "dollar",			{ "dollar",			{ 192, 25  }, { 18, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "minus",			{ "minus",			{ 210, 25  }, { 13, 23 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "plus",			{ "plus",			{ 223, 25  }, { 17, 23 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "c4",				{ "c4",				{ 0  , 148 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "defuser",		{ "defuser",		{ 32 , 148 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "stopwatch",		{ "stopwatch",		{ 144, 72  }, { 24, 24 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "smallskull",		{ "smallskull",		{ 72 , 25  }, { 12, 12 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "smallc4",		{ "smallc4",		{ 84 , 25  }, { 12, 12 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "smallvip",		{ "smallvip",		{ 96 , 25  }, { 12, 12 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "buyzone",		{ "buyzone",		{ 96 , 148 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "rescue",			{ "rescue",			{ 64 , 148 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "escape",			{ "escape",			{ 128, 148 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "vipsafety",		{ "vipsafety",		{ 160, 148 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "suit_full",		{ "suit_full",		{ 0  , 25  }, { 24, 24 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "suit_empty",		{ "suit_empty",		{ 24 , 25  }, { 24, 24 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "suithelmet_full",{ "suithelmet_full",{ 0  , 124 }, { 24, 24 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "suithelmet_empty",{ "suithelmet_empty",{24, 124 }, { 24, 24 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "flash_full",		{ "flash_full",		{ 160, 25  }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "flash_empty",	{ "flash_empty",	{ 112, 25  }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "flash_beam",		{ "flash_beam",		{ 144, 25  }, { 16, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "hostage",		{ "hostage",		{ 0  , 180 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "bombticking1",	{ "bombticking1",	{ 192, 148 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "bombticking",	{ "bombticking",	{ 224, 148 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "hostage4a",		{ "hostage4a",		{ 32 , 180 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "hostage3a",		{ "hostage3a",		{ 64 , 180 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "hostage2a",		{ "hostage2a",		{ 96 , 180 }, { 32, 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "hostage1a",		{ "hostage1a",		{ 128, 180 }, { 3 , 32 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "hostage4",		{ "hostage4",		{ 0  , 213 }, { 48, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "hostage3",		{ "hostage3",		{ 48 , 213 }, { 48, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "hostage2",		{ "hostage2",		{ 96 , 213 }, { 48, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "hostage1",		{ "hostage1",		{ 144, 213 }, { 48, 25 }, &s_sprite_atlas_map.at("640hud7") } },
	// Still from 640hud7, but not included in hud.txt
	{ "flashbang_ammo",	{ "flashbang_ammo",	{ 48 , 96  }, { 24, 24 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "hegrenade_ammo",	{ "hegrenade_ammo",	{ 72 , 96  }, { 24, 24 }, &s_sprite_atlas_map.at("640hud7") } },
	{ "smokegren_ammo",	{ "smokegren_ammo",	{ 144, 96  }, { 24, 24 }, &s_sprite_atlas_map.at("640hud7") } },

	// 640hud8
	{ "dmg_poison",		{ "dmg_poison",		{ 128, 0 }, { 64, 64 }, &s_sprite_atlas_map.at("640hud8") } },
	{ "dmg_bio",		{ "dmg_bio",		{ 128, 0 }, { 64, 64 }, &s_sprite_atlas_map.at("640hud8") } },
	{ "dmg_chem",		{ "dmg_chem",		{ 0  , 0 }, { 64, 64 }, &s_sprite_atlas_map.at("640hud8") } },
	{ "dmg_drown",		{ "dmg_drown",		{ 64 , 0 }, { 64, 64 }, &s_sprite_atlas_map.at("640hud8") } },
	{ "dmg_shock",		{ "dmg_shock",		{ 192, 0 }, { 64, 64 }, &s_sprite_atlas_map.at("640hud8") } },

	// 640hud9
	{ "dmg_gas",		{ "dmg_gas",		{ 0  , 0 }, { 64, 64 }, &s_sprite_atlas_map.at("640hud9") } },
	{ "dmg_cold",		{ "dmg_cold",		{ 64 , 0 }, { 64, 64 }, &s_sprite_atlas_map.at("640hud9") } },
	{ "dmg_heat",		{ "dmg_heat",		{ 128, 0 }, { 64, 64 }, &s_sprite_atlas_map.at("640hud9") } },
	{ "dmg_rad",		{ "dmg_rad",		{ 192, 0 }, { 64, 64 }, &s_sprite_atlas_map.at("640hud9") } },

	// 640hud16
	{ "d_fiveseven",	{ "d_fiveseven",	{ 192, 0  }, { 32, 16 }, &s_sprite_atlas_map.at("640hud16") } },
	{ "d_sg550",		{ "d_sg550",		{ 192, 48 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud16") } },
	{ "d_ump45",		{ "d_ump45",		{ 192, 80 }, { 48, 16 }, &s_sprite_atlas_map.at("640hud16") } },

	// radar640
	{ "radar",			{ "radar",			{ 0, 0 }, { 128, 128 }, &s_sprite_atlas_map.at("radar640") } },

	// radaropaque640
	{ "radaropaque",	{ "radaropaque",	{ 0, 0 }, { 128, 128 }, &s_sprite_atlas_map.at("radaropaque640") } },

	// crosshairs
	{ "autoaim_c",		{ "autoaim_c",		{ 96, 0 }, { 24, 24 }, &s_sprite_atlas_map.at("crosshairs") } },
};

std::unordered_map<int, std::string> CSpriteMgr::m_wpn_id_for_sprite_name =
{
	{ WEAPON_NONE,			"" },
	{ WEAPON_P228,			"d_p228" },
	{ WEAPON_GLOCK,			"d_glock18" },		// not used anyway
	{ WEAPON_SCOUT,			"d_scout" },
	{ WEAPON_HEGRENADE,		"hegrenade_ammo" },
	{ WEAPON_XM1014,		"d_xm1014" },
	{ WEAPON_C4,			"c4" },
	{ WEAPON_MAC10,			"d_mac10" },
	{ WEAPON_AUG,			"d_aug" },
	{ WEAPON_SMOKEGRENADE,	"smokegren_ammo" },
	{ WEAPON_ELITE,			"d_elite" },
	{ WEAPON_FIVESEVEN,		"d_fiveseven" },
	{ WEAPON_UMP45,			"d_ump45" },
	{ WEAPON_SG550,			"d_sg550" }, 
	{ WEAPON_GALIL,			"d_galil" }, 
	{ WEAPON_FAMAS,			"d_famas" }, 
	{ WEAPON_USP,			"d_usp" },
	{ WEAPON_GLOCK18,		"d_glock18" },
	{ WEAPON_AWP,			"d_awp" },
	{ WEAPON_MP5N,			"d_mp5navy" }, 
	{ WEAPON_M249,			"d_m249" }, 
	{ WEAPON_M3,			"d_m3" },
	{ WEAPON_M4A1,			"d_m4a1" },
	{ WEAPON_TMP,			"d_tmp" },
	{ WEAPON_G3SG1,			"d_g3sg1" },
	{ WEAPON_FLASHBANG,		"flashbang_ammo" },
	{ WEAPON_DEAGLE,		"d_deagle" },
	{ WEAPON_SG552,			"d_sg552" },
	{ WEAPON_AK47,			"d_ak47" },
	{ WEAPON_KNIFE,			"d_knife" },
	{ WEAPON_P90,			"d_p90" },

	// TODO?
	{ WEAPON_SHIELDGUN,		"" },
};

void Sprite_t::render_additive(const Vector2D& position, const CColor& color, bool dim, uint32_t frame) const
{
	auto& [x, y] = m_screen_pos;
	auto& [w, h] = m_screen_dimensions;
	auto& [render_x, render_y] = position;

	hl::wrect_t rect = {
		(int)x, (int)(x + w),
		(int)y, (int)(y + h)
	};

	hl::HSPRITE_t atlas_id = m_atlas_accruing->m_sprite_atlas_id;

	if (atlas_id != NULL_SPRITE)
	{
		CColor new_color = CColor(color.r * 255.0f, color.g * 255.0f, color.b * 255.0f);
		CSpriteMgr::the().scale_hud_colors(new_color, dim);

		CMemoryHookMgr::the().cl_enginefuncs()->pfnSPR_Set(atlas_id, (int)new_color.r, (int)new_color.g, (int)new_color.b);
		CMemoryHookMgr::the().cl_enginefuncs()->pfnSPR_DrawAdditive(frame, (int)render_x, (int)render_y, &rect);
	}
}

bool SpriteAtlas_t::precache_engine_model()
{
	m_precached_model = CMemoryHookMgr::the().engine_studio_api().get()->Mod_ForName(m_sprite_atlas_model_path.string().c_str(), false);
	return m_precached_model != nullptr;
}

bool SpriteAtlas_t::get_cached_sprite_data()
{
	assert(m_precached_model != nullptr);

	if (m_precached_model->type != hl::mod_sprite)
	{
		CConsole::the().info("Trying to get cached data from non-sprite model! (model type: '{}')", m_precached_model->type);
		return false;
	}

	m_cached_sprite_data = (hl::msprite_t*)m_precached_model->cache.data;
	if (!m_cached_sprite_data)
	{
		CConsole::the().info("Sprite model without cached data!");
		return false;
	}

	return m_cached_sprite_data;
}

bool SpriteAtlas_t::precache_engine_sprite()
{
	m_sprite_atlas_id = CMemoryHookMgr::the().cl_enginefuncs()->pfnSPR_Load(m_sprite_atlas_model_path.string().c_str());
	if (m_sprite_atlas_id == NULL_SPRITE)
	{
		CConsole::the().error("Failed to precache engine sprite '{}'", m_sprite_atlas_model_path);
		return false;
	}

	return m_sprite_atlas_id;
}

bool CSpriteMgr::initialize_engine_sprite_data()
{
	CConsole::the().info("Initializing sprite data...");

	for (auto& [key, val] : s_sprite_atlas_map)
	{
		CConsole::the().info("{}...", key);
		
		// model_t*
		if (!val.precache_engine_model())
		{
			CConsole::the().error("Couldn't get engine model");
			return false;
		}

		// msprite_t*
		if (!val.get_cached_sprite_data())
		{
			CConsole::the().error("Couldn't retrieve internal sprite data");
			return false;
		}

		// HSPRITE_t
		if (!val.precache_engine_sprite())
		{
			return false;
		}
	}

	CConsole::the().info("Success!");
	return true;
}

void CSpriteMgr::scale_hud_colors(CColor& color, bool dim)
{
	float x = (float)(dim ? MIN_ALPHA : 255) / 255;
	color.r = (float)((int)(color.r * x));
	color.g = (float)((int)(color.g * x));
	color.b = (float)((int)(color.b * x));
}

void CSpriteMgr::render_current_weapon_sprite() const
{
	auto vm = &CMemoryHookMgr::the().cl().get()->viewent;
	if (!vm->model)
	{
		return;
	}

	auto current_weapon = CWeapons::the().get_current_weapon();
	if (!current_weapon)
	{
		return;
	}

	auto sprite_name = m_wpn_id_for_sprite_name[current_weapon->m_weapon_id];

	if (!sprite_name.empty())
	{
		Sprite_t* spr = get_sprite(sprite_name);

		if (!spr)
		{
			return;
		}

		// default greenish color that most of the cstrike screen sprites have.
		CColor color = RGB_GREENISH;

		auto clr = hud_color.get_value();
		if (clr.is_nonzero())
		{
			color = { (uint8_t)(clr.r * 255.0f),
					  (uint8_t)(clr.g * 255.0f),
					  (uint8_t)(clr.b * 255.0f) };
		}

		spr->render_additive({ 10, CGameUtil::the().get_engine_screen_info().iHeight / 2.f - m_cstrike_sprite_font_size.x - 80 }, color, false);
	}
}

void CSpriteMgr::render_velocity() const
{
	auto local = CLocalState::the().local_player();
	if (!local)
		return;

	float velocity = CLocalState::the().get_local_velocity_2d();

	static float rolling_velocity = 0.0f;

	rolling_velocity = 0.9f * rolling_velocity + (1.0f - 0.9f) * velocity;

	// default orangeish color that most of the cstrike screen sprites have.
	CColor color = RGB_ORANGEISH;

	auto clr = hud_color.get_value();
	if (clr.is_nonzero())
	{
		color = hud_color.get_value();
	}

	auto screen = CGameUtil::the().get_engine_screen_info();
	Vector2D off = render_number(std::abs((int64_t)std::ceil(rolling_velocity - 1.0f)), 
								 Vector2D((float)screen.iWidth / 2.0f, (float)screen.iHeight - m_cstrike_sprite_font_size.y * 2 - 5 - 45),
								 color, true );
}

Vector2D CSpriteMgr::render_digit(uint8_t digit, const Vector2D& position, const CColor& color) const
{
	assert(digit <= 9 && "The digit was >9!");
	if (digit > 9)
	{
		return {};
	}

	char c_digit = '0' + digit;
	auto sprite_id = std::string("number_") + c_digit;

	Sprite_t* spr = get_sprite(sprite_id);
	if (!spr)
	{
		return {};
	}

	spr->render_additive(position, color, true);

	return position;
}

Vector2D CSpriteMgr::render_number(int64_t number, const Vector2D& position, const CColor& color, bool centerize) const
{
	// Handle negative values
	if (number < 0)
	{
		Sprite_t* minus = get_sprite("minus");
		if (!minus)
		{
			return {};
		}
		minus->render_additive(position, color, true);
	}

	std::function<std::vector<uint8_t>(int64_t)> render_foreach = [&](int64_t n) -> std::vector<uint8_t>
	{
		std::vector<uint8_t> ret;
		if (n > 9)
			ret = render_foreach(n / 10);

		if ((n % 10) < 10 && (n % 10) >= 0)
			ret.emplace_back((uint8_t)(n % 10));

		return ret;
	};

	Vector2D off;
	uint32_t i = 0;
	const auto nums = render_foreach(std::abs(number));
	Vector2D initial_pos = { position.x - (centerize ? ((m_cstrike_sprite_font_size.x + 1) * nums.size()) / 2.0f : 0), position.y };
	for (const auto& n : nums)
	{
		off = { initial_pos.x + i++ * m_cstrike_sprite_font_size.x + 1, initial_pos.y };

		render_digit(n, off, color);
	}

	return off;
}

template<uint8_t Precision> requires(Precision < std::numeric_limits<double>::max_digits10 && Precision > 0)
Vector2D CSpriteMgr::render_float_number(double number, const Vector2D& position, const CColor& color, bool centerize) const
{
	// TODO: This isn't working properly (unfinished)
	return {};

	double base = std::round(number);
	double fraction = (double)number - (int64_t)number;

	// base
	Vector2D off = render_number(base, position, color);

	// fraction
	off.x += 5;
	double fraction_scaled = fraction * std::pow(10, (Precision > 1) ? Precision - 1 : Precision);
	off = render_number(std::round(std::abs(fraction_scaled)), off, color);

	return off;
}

void CSpriteMgr::initialize()
{
	static bool once = false;

	if (once)
		return;

	CConsole::the().info("Initializitng sprite manager...");

	// Setup current resolution scale which we'll be rendering sprites on.
	m_resolution_scale = CGameUtil::the().get_engine_screen_info().iWidth < 640 ? 320 : 640;

	// Retrieve data such as model_t or msprite_t.
	// This has to be done explicitly after the constexpr declaration.
	if (!initialize_engine_sprite_data())
	{
		CConsole::the().error("Couldn't retrieve engine sprite data");
		return;
	}

	// get random number and get the width&height out of it
	auto number = get_sprite("number_0");
	if (number)
	{
		m_cstrike_sprite_font_size = number->m_screen_dimensions;
	}
	else
	{
		m_cstrike_sprite_font_size = { 20, 25 }; // for some reason failed? this shouldn't happen...
	}

	CConsole::the().info("All sprites initialized.");

	once = true;
}

void CSpriteMgr::update()
{
	initialize();

	if (!hud_render.get_value())
		return;
	
	// render current weapon
	if (hud_render_current_weapon.get_value())
	{
		render_current_weapon_sprite();
	}
	
	// render velocity
	if (hud_render_velocity.get_value())
	{
		render_velocity();
	}
}

Sprite_t* CSpriteMgr::get_sprite(const std::string& name) const
{
	Sprite_t* sprite = nullptr;
	try
	{
		sprite = &s_sprite_map.at(name);
	}
	catch (...)
	{
		//CConsole::the().error("Tried to find unknown sprite: '{}'", name);
	}

	return sprite;
}

SpriteAtlas_t* CSpriteMgr::get_sprite_atlas(const std::string& name) const
{
	SpriteAtlas_t* sprite_atlas = nullptr;
	try
	{
		sprite_atlas = &s_sprite_atlas_map.at(name);
	}
	catch (...)
	{
		//CConsole::the().error("Tried to find unknown sprite atlas: '{}'", name);
	}

	return sprite_atlas;
}

void CSpriteMgr::handle_color_change(hl::HSPRITE_t spriteId, int& r, int& g, int& b)
{
	if (!hud_color_enable.get_value())
	{
		return;
	}

	if (spriteId != SPRITEID_Radar && spriteId != SPRITEID_RadarTransulcent)
	{
		auto clr = hud_color.get_value();
		r = (int)(clr.r * 255.0f);
		g = (int)(clr.g * 255.0f);
		b = (int)(clr.b * 255.0f);
	}
}
