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
extern VarBoolean esp_only_enemy_team;

extern VarBoolean esp_entity_enable;

extern VarBoolean esp_player_enable;
extern VarBoolean esp_player_name;

extern VarBoolean esp_sound_enable;
extern VarFloat esp_sound_interval;
extern VarBoolean esp_sound_filter_local;

static const int k_step_display_interval_s = 10;

struct PlayerStepSound
{
	float get_life_duration() const
	{
		return std::chrono::duration<float, std::ratio<1, 1>>(std::chrono::high_resolution_clock::now() - spawntime).count();
	}

	Vector origin;
	int entid;
	std::chrono::high_resolution_clock::time_point spawntime;
};

class CESP
{
public:
	DECL_BASIC_CLASS(CESP);

public:
	void initialize_gui();

	void register_player_step(const Vector& origin, int entid);

private:
	void on_render();

	void render_players();
	void render_entities();
	void render_sound();

	void render_box_for_four_points(const Vector2D& top_left, const Vector2D& top_right, const Vector2D& bottom_right, const Vector2D& bottom_left, const CColor& color, float box_tall_half);

	EFontSize fontsize_by_dist(float dist, float max_dist);

	std::deque<PlayerStepSound> m_stepsounds;
};

#endif // ESP_H