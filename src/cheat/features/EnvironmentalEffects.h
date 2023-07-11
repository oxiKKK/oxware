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

#ifndef ENVIRONMENTALEFFECTS_H
#define ENVIRONMENTALEFFECTS_H
#pragma once

extern VarBoolean env_enable;

extern VarBoolean env_rain;
extern VarInteger env_radius;
extern VarBoolean env_ground_fog;
extern VarInteger env_ground_fog_density;
extern VarFloat env_ground_fog_size;

extern VarFloat env_wind_speed;
extern VarFloat env_particle_fallspeed;

extern VarFloat env_rain_density;
extern VarBoolean env_rain_ambient;
extern VarFloat env_rain_ambient_volume;
extern VarBoolean env_rain_ambient_thunder;

extern VarBoolean env_snow;
extern VarFloat env_snow_density;
extern VarInteger env_snow_flake_size;
extern VarFloat env_snow_flake_die_time;
extern VarBoolean env_snow_ambient;
extern VarFloat env_snow_ambient_volume;

class CEnvironmentalEffects
{
public:
	DECL_BASIC_CLASS(CEnvironmentalEffects);

public:
	void initialize();
	void shutdown();
	void restart(); // does reinitialization

	void render();

	inline float get_old_time() { return m_old_time; }

	hl::model_t* get_ripple_sprite() { return m_ripple; }
	hl::model_t* get_splash_sprite() { return m_water_splash; }

private:
	bool m_initialized = false, m_shutting_down = false;
	bool m_failed_emulation = false;

	bool m_initialized_for_the_first_time = true;

	void reset_particles();

	void begin_emulation();

	hl::model_t* m_snow_sprite;
	hl::model_t* m_rain_sprite, *m_ripple, *m_water_splash;
	hl::model_t* m_wind_particle_sprite;

	Vector m_weather_origin;

	// to keep track of timings
	float m_old_time;

	void precache_sprites();

	void render_rain();
	void render_snow();
	void render_ground_fog();

	void play_ambient_rain_sound();
	void stop_all_ambient_sound();
	bool m_rain_sound_played = false;
	bool m_snow_sound_played = false;

	void play_ambient_looped_sound_helper(VarBoolean* env_effect_enabled, VarBoolean* sound_enabled, VarFloat* sound_volume, float* prev_volume, bool* sound_played, const char* sound_file);

	float m_thunder_timer;

	void create_raindrop(const Vector& origin);
	void create_wind_particle(const Vector& origin, float max_size);
	void create_snow_flake(const Vector& origin);

	//
	// wind
	//

	void update_wind_variables();
	void initialize_wind_variables();

	float m_next_wind_change_time;
	Vector m_desired_wind_direction, m_wind_span;
	float m_desired_wind_speed;
	float m_ideal_wind_yaw;
};

#endif // ENVIRONMENTALEFFECTS_H