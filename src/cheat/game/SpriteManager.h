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

#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H
#pragma once

extern VarBoolean hud_render;
extern VarBoolean hud_render_current_weapon;
extern VarBoolean hud_render_velocity;

extern VarBoolean hud_color_enable;
extern VarColor hud_color;

enum ECommonSpriteIds
{
	SPRITEID_Selection1 = 1,
	SPRITEID_MainHUD = 2,
	SPRITEID_DamageTypes1 = 3,
	SPRITEID_DamageTypes2 = 4,
	SPRITEID_Selection2AndTrain = 5,
	SPRITEID_Unknown = 6,
	SPRITEID_Crosshairs = 7,
	SPRITEID_Selection3 = 8,
	SPRITEID_Selection4AndWeaponSet1 = 9,
	SPRITEID_Selection5AndWeaponSet2 = 10,
	SPRITEID_Selection6AndWeaponSet3 = 11,
	SPRITEID_Radar = 12,
	SPRITEID_RadarTransulcent = 13,
};

struct SpriteAtlas_t;

struct Sprite_t
{
public:
	Sprite_t(const std::string& name, const Vector2D& position, const Vector2D& size, SpriteAtlas_t* accruing_atlas) :
		m_name(name),
		m_screen_pos(position),
		m_screen_dimensions(size),
		m_atlas_accruing(accruing_atlas) 
	{
	}

	Sprite_t() = default;
	Sprite_t(const Sprite_t&) = default;
	Sprite_t(Sprite_t&&) = default;

	bool is_valid() const { return !m_screen_dimensions.IsZero() && !m_screen_pos.IsZero() && m_atlas_accruing; }

	// Rendering stuff
	void render_additive(const Vector2D& position, const CColor& color, bool dim, const uint32_t frame = 0) const;

	std::string m_name;

	Vector2D m_screen_pos;
	Vector2D m_screen_dimensions;

	SpriteAtlas_t* m_atlas_accruing;
};

struct SpriteAtlas_t
{
public:
	SpriteAtlas_t(const std::filesystem::path& path) :
		m_sprite_atlas_model_path(path)
	{
	}

	SpriteAtlas_t() = default;
	SpriteAtlas_t(const SpriteAtlas_t&) = default;
	SpriteAtlas_t(SpriteAtlas_t&&) = default;

	bool is_valid() const noexcept { return m_precached_model && m_cached_sprite_data && m_sprite_atlas_id != NULL_SPRITE; }

public:
	bool precache_engine_model();
	bool precache_engine_sprite();
	bool get_cached_sprite_data();

	std::filesystem::path m_sprite_atlas_model_path;

	//	Data gathered from engine
	hl::model_t* m_precached_model = nullptr;
	hl::msprite_t* m_cached_sprite_data = nullptr;
	hl::HSPRITE_t m_sprite_atlas_id = NULL_SPRITE;
};

class CSpriteMgr
{
public:
	DECL_BASIC_CLASS(CSpriteMgr);

public:
	void update();

	Sprite_t* get_sprite(const std::string& name) const;
	SpriteAtlas_t* get_sprite_atlas(const std::string& name) const;

	void handle_color_change(hl::HSPRITE_t spriteId, int& r, int& g, int& b);

	// expects 0-255 range
	static void scale_hud_colors(CColor& color, bool dim);

private:
	void initialize();

	bool initialize_engine_sprite_data();

	//
	// rendering
	//

	void render_current_weapon_sprite() const;
	void render_velocity() const;

	Vector2D render_digit(uint8_t digit, const Vector2D& position, const CColor& color) const;
	Vector2D render_number(int64_t number, const Vector2D& position, const CColor& color, bool centerize = false) const;
	template<uint8_t Precision> requires(Precision < std::numeric_limits<double>::max_digits10 && Precision > 0)
	Vector2D render_float_number(double number, const Vector2D& position, const CColor& color, bool centerize = false) const;

private:
	uint32_t m_resolution_scale;

	static std::unordered_map<std::string, SpriteAtlas_t> s_sprite_atlas_map;
	static std::unordered_map<std::string, Sprite_t> s_sprite_map;
	static std::unordered_map<int, std::string> m_wpn_id_for_sprite_name;
	Vector2D m_cstrike_sprite_font_size;
};

#endif // SPRITEMANAGER_H