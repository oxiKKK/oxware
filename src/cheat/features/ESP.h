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

#ifndef ESP_H
#define ESP_H
#pragma once

extern VarBoolean esp_enable;
extern VarBoolean esp_background;
extern VarInteger esp_box_type;
extern VarBoolean esp_box_outline;

extern VarBoolean esp_entity_enable;

extern VarBoolean esp_player_enable;
extern VarBoolean esp_player_name;
extern VarBoolean esp_player_enemy;
extern VarBoolean esp_player_teammates;
extern VarBoolean esp_player_details;

extern VarBoolean esp_sound_enable;
extern VarFloat esp_sound_interval;
extern VarBoolean esp_sound_filter_local;
extern VarBoolean esp_sound_resolver;
extern VarInteger esp_sound_resolver_distace;
extern VarInteger esp_sound_type;

static const int k_step_display_interval_s = 10;

struct PlayerStepSound
{
	uint32_t get_life_duration_ms() const
	{
		return GetTickCount() - spawntime_ms;
	}

	Vector origin;
	int entid;
	uint32_t spawntime_ms;
};

struct SoundEntIndexResolverCache
{
	SoundEntIndexResolverCache() : resolved_index(0) {}
	SoundEntIndexResolverCache(int resolved) : resolved_index(resolved) {}
	
	// if original == resolved, everything is fine
	int resolved_index;
};

class CSoundEsp
{
public:
	DECL_BASIC_CLASS(CSoundEsp);

public:
	void register_player_step(const Vector& sound_origin, int entid);

	void update(uint32_t time_limit);

	std::deque<PlayerStepSound> m_stepsounds;

	void reset_cache();

private:
	void resolve_sound_entid(const Vector& sound_origin, int& entid);

	std::unordered_map<int, SoundEntIndexResolverCache> m_resolver_cache;
};

struct ESPBoxMetrics
{
	float box_tall_half;
	float box_wide_half;

	Vector2D box_topleft;
	Vector2D box_topright;
	Vector2D box_botleft;
	Vector2D box_botright;
};

// retreived from w_backpack.mdl
#define DROPPED_BOMB_BBOX_MIN	Vector(-5.22f, -2.94f, -6.85f)
#define DROPPED_BOMB_BBOX_MAX	Vector(5.22f, 2.94f, 6.85f)

class CESP
{
public:
	DECL_BASIC_CLASS(CESP);

public:
	void initialize_gui();

private:
	void on_render();

	void render_players();
	void render_entities();
	void render_sound();
	void render_bomb_info();

	// rendering
	bool render_player_esp(int index, const CGenericPlayer& player);
	bool render_entity_esp(int index, const CGenericEntity& entity);
	bool render_sound_esp(const PlayerStepSound& step, uint32_t time_limit);
	bool render_dropped_bomb_esp(const BombInfo& bomb_info);
	bool render_planted_bomb_esp(const BombInfo& bomb_info);

	// util
	bool origin_to_2d_box(const Vector& origin, const Vector& min, const Vector& max, float box_aspect, ESPBoxMetrics& metrics);

	// rendering widgets
	void render_esp_box(const ESPBoxMetrics& metrics, const CColor& color, float corner_ratio = 1.0f);
	void render_esp_label(const ESPBoxMetrics& metrics, const std::string& label);
	void render_esp_details(const ESPBoxMetrics& metrics, const CGenericPlayer& player);
	void render_box_for_four_points(const Vector2D& top_left, const Vector2D& top_right, const Vector2D& bottom_right, const Vector2D& bottom_left, const CColor& color, float box_tall_half, float corner_ratio = 1.0f);
	void render_line_with_outline(const Vector2D& from, const Vector2D& to, const CColor& color, float thicc = 1.0f);
	void render_circle_with_outline(const Vector2D& center, float radius, int num_segments, const CColor& color, float thicc = 1.0f);
	void render_space_rotated_circle_with_outline(const Vector& origin, float radius, int num_segments, const CColor& color, float thicc = 1.0f);

	inline static constexpr CColor s_outline_color = CColor(0, 0, 0, 230);

	bool decide_player_enemy(VarBoolean* player_enemy, VarBoolean* player_teammates, int index);

	float get_playerbox_ratio(const CGenericPlayer* player);
};

#endif // ESP_H