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

Vector g_vecZero = Vector(0, 0, 0);

VarBoolean env_enable("env_enable", "Enables environmental effects", false);

VarBoolean env_rain("env_rain", "Enables raining", false);
VarInteger env_radius("env_radius", "Radius where to generate snow/rain/etc.", 500, 200, 1000);
VarBoolean env_ground_fog("env_ground_fog", "Enables ground fog", false);
VarInteger env_ground_fog_density("env_ground_fog_density", "Ground fog density", 1, 1, 10);
VarFloat env_ground_fog_size("env_ground_fog_size", "Ground fog cloud size", 1.0f, 0.5f, 2.0f);

VarFloat env_wind_speed("env_wind_speed", "Controls wind speed", 1.0f, 0.0f, 6.0f);
VarFloat env_particle_fallspeed("env_particle_fallspeed", "Controls fallspeed of dropplets or snowflakes", 1.0f, 0.5f, 1.0f);

VarFloat env_rain_density("env_rain_density", "Controls density of the rain", 2.0f, 0.1f, 8.0f);
VarBoolean env_rain_ambient("env_rain_ambient", "Plays ambient raining sound", false);
VarFloat env_rain_ambient_volume("env_rain_ambient_volume", "Volume of the raining ambient sound", 0.5f, 0.1f, 1.0f);
VarBoolean env_rain_ambient_thunder("env_rain_ambient_thunder", "Plays ambient thunder sound", false);

VarBoolean env_snow("env_snow", "Enables snowing", false);
VarFloat env_snow_density("env_snow_density", "Controls density of the snow", 2.0f, 0.1f, 8.0f);
VarInteger env_snow_flake_size("env_snow_flake_size", "Snow flake size", 1, 1, 10);
VarFloat env_snow_flake_die_time("env_snow_flake_die_time", "Time when the flake starts to die after it hits ground", 0.5f, 0.5f, 5.0f);
VarBoolean env_snow_ambient("env_snow_ambient", "Plays ambient eerie sound for snowy environment", false);
VarFloat env_snow_ambient_volume("env_snow_ambient_volume", "Volume of the eerie sound for snowy environment", 0.5f, 0.1f, 1.0f);

void CEnvironmentalEffects::initialize()
{
	begin_emulation();

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	// sounds
	m_thunder_timer = cl_enginefuncs->pfnGetClientTime() + cl_enginefuncs->pfnRandomLong(60, 60 * 3);

	precache_sprites();

	// wind
	initialize_wind_variables();

	if (m_initialized_for_the_first_time)
	{
		CConsole::the().info("In-Game environmental effects initialized");
	}

	m_initialized = true;

	m_initialized_for_the_first_time = false;
}

void CEnvironmentalEffects::shutdown()
{
	if (!m_initialized)
	{
		return;
	}

	m_shutting_down = true;

	stop_all_ambient_sound();

	reset_particles();

	CParticlemanMiniMemEmulation::the().restore_from_emulation();

	m_initialized = false;
	m_shutting_down = false;
}

void CEnvironmentalEffects::restart()
{
	if (!env_enable.get_value())
	{
		return;
	}

	shutdown();
	initialize();
}

void CEnvironmentalEffects::render()
{
	if (m_shutting_down)
	{
		return;
	}

	//
	// sound - has to run even if disabled (in order to disable sound when env_enable is off)
	//

	if (m_initialized)
	{
		play_ambient_rain_sound();
	}

	if (!env_enable.get_value())
	{
		return;
	}

	static bool as_lock = false;

	if (!m_initialized && !as_lock)
	{
		initialize();
	}

	// TODO: Figure out a better way of doing this, since this respawns all particles, it doesn't
	//		 just stop rendering them..
	if (CAntiScreen::the().hide_visuals() || CPanic::the().panicking())
	{
		shutdown();
		as_lock = true;
		return;
	}
	else
	{
		begin_emulation();
		as_lock = false;
	}

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	auto client_data = CLocalState::the().get_current_frame_clientdata();

	Vector weather_origin = CLocalState::the().get_origin();
	if (client_data->iuser1 > OBS_NONE && client_data->iuser1 != OBS_ROAMING)
	{
		hl::cl_entity_t* ent = cl_enginefuncs->pfnGetEntityByIndex(client_data->iuser2);
		if (ent)
		{
			weather_origin = ent->origin;
		}
	}

	weather_origin.z += 36.0f;
	m_weather_origin = weather_origin;

	update_wind_variables();

	//
	// rendering
	//
	
	if (env_rain.get_value())
	{
		render_rain();
	}

	if (env_snow.get_value())
	{
		render_snow();
	}

	if (env_ground_fog.get_value())
	{
		render_ground_fog();
	}

	m_old_time = cl_enginefuncs->pfnGetClientTime();
}

void CEnvironmentalEffects::reset_particles()
{
	// TODO: Without this the game crashes on unload however, also, after calling this, 
	//		 every other live particle will die (e.g. a smoke)
	auto iparticleman = CHLInterfaceHook::the().IParticleMan();
	if (iparticleman)
	{
		iparticleman->ResetParticles();
	}
}

void CEnvironmentalEffects::begin_emulation()
{
	if (m_failed_emulation)
	{
		return; // don't try again if we've failed.
	}

	if (!CParticlemanMiniMemEmulation::the().emulate())
	{
		CConsole::the().error("Failed to create particleman MiniMem emulator.");
		m_failed_emulation = true;
	}
}

void CEnvironmentalEffects::precache_sprites()
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	m_snow_sprite = (hl::model_t*)cl_enginefuncs->pfnGetSpritePointer(cl_enginefuncs->pfnSPR_Load("sprites/effects/snowflake.spr"));
	m_rain_sprite = (hl::model_t*)cl_enginefuncs->pfnGetSpritePointer(cl_enginefuncs->pfnSPR_Load("sprites/effects/rain.spr"));
	m_ripple = (hl::model_t*)cl_enginefuncs->pfnGetSpritePointer(cl_enginefuncs->pfnSPR_Load("sprites/effects/ripple.spr"));
	m_water_splash = (hl::model_t*)cl_enginefuncs->pfnGetSpritePointer(cl_enginefuncs->pfnSPR_Load("sprites/wsplash3.spr"));
	m_wind_particle_sprite = (hl::model_t*)cl_enginefuncs->pfnGetSpritePointer(cl_enginefuncs->pfnSPR_Load("sprites/gas_puff_01.spr"));
}

void CEnvironmentalEffects::render_rain()
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	Vector local_velocity = CLocalState::the().get_local_velocity_vec();

	int density = (int)(env_rain_density.get_value() * 100.0f + (env_radius.get_value() / 75));

	for (int j = 0; j < density; j++)
	{
		Vector origin_to_generate = m_weather_origin;
		Vector endpos;

		float radius = (float)env_radius.get_value() + 100.0f; // add a little bit more to rain
		origin_to_generate.x += local_velocity.x + cl_enginefuncs->pfnRandomFloat(-radius, radius);
		origin_to_generate.y += local_velocity.y + cl_enginefuncs->pfnRandomFloat(-radius, radius);
		origin_to_generate.z += local_velocity.z + cl_enginefuncs->pfnRandomFloat(100.0f, 300.0f);

		float f = local_velocity.x;
		endpos.x = origin_to_generate.x + ((cl_enginefuncs->pfnRandomLong(0, 5) > 2) ? f : -f);
		endpos.y = origin_to_generate.y + local_velocity.y;
		endpos.z = 4086.0f;

		hl::pmtrace_t skytrace;
		cl_enginefuncs->pEventAPI->EV_SetTraceHull(2);
		cl_enginefuncs->pEventAPI->EV_PlayerTrace(origin_to_generate, endpos, PM_WORLD_ONLY, -1, &skytrace);

		char* traced_texture = (char*)cl_enginefuncs->pEventAPI->EV_TraceTexture(skytrace.ent, origin_to_generate, skytrace.endpos);

		// create rain only from sky
		if (traced_texture && strncmp(traced_texture, "sky", 3) == 0)
		{
			create_raindrop(origin_to_generate);
		}
	}
}

void CEnvironmentalEffects::render_snow()
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	Vector local_velocity = CLocalState::the().get_local_velocity_vec();

	int density = (int)(env_snow_density.get_value() * 90 + (env_radius.get_value() / 75));

	for (int j = 0; j < density; j++)
	{
		Vector origin_to_generate = m_weather_origin;
		Vector endpos;

		float radius = (float)env_radius.get_value();
		origin_to_generate.x += local_velocity.x + cl_enginefuncs->pfnRandomFloat(-radius, radius);
		origin_to_generate.y += local_velocity.y + cl_enginefuncs->pfnRandomFloat(-radius, radius);
		origin_to_generate.z += local_velocity.z + cl_enginefuncs->pfnRandomFloat(100.0f, 300.0f);

		float f = local_velocity.x;
		endpos.x = origin_to_generate.x + ((cl_enginefuncs->pfnRandomLong(0, 5) > 2) ? f : -f);
		endpos.y = origin_to_generate.y + local_velocity.y;
		endpos.z = 4086.0f;

		hl::pmtrace_t skytrace;
		cl_enginefuncs->pEventAPI->EV_SetTraceHull(2);
		cl_enginefuncs->pEventAPI->EV_PlayerTrace(origin_to_generate, endpos, PM_WORLD_ONLY, -1, &skytrace);

		char* traced_texture = (char*)cl_enginefuncs->pEventAPI->EV_TraceTexture(skytrace.ent, origin_to_generate, skytrace.endpos);

		// create snow only from sky
		if (traced_texture && strncmp(traced_texture, "sky", 3) == 0)
		{
			create_snow_flake(origin_to_generate);
		}
	}
}

void CEnvironmentalEffects::render_ground_fog()
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	Vector local_velocity = CLocalState::the().get_local_velocity_vec();
	Vector local_origin = CLocalState::the().get_origin();

	int density = (int)(env_ground_fog_density.get_value() + (env_radius.get_value() / 75));

	for (int j = 0; j < density; j++)
	{
		Vector origin_to_generate = m_weather_origin;
		Vector endpos;

		float radius = (float)env_radius.get_value() + 100.0f; // add a little bit more to rain
		origin_to_generate.x += local_velocity.x + cl_enginefuncs->pfnRandomFloat(-radius, radius);
		origin_to_generate.y += local_velocity.y + cl_enginefuncs->pfnRandomFloat(-radius, radius);
		origin_to_generate.z += local_velocity.z + cl_enginefuncs->pfnRandomFloat(100.0f, 300.0f);

		float f = local_velocity.x;
		endpos.x = origin_to_generate.x + ((cl_enginefuncs->pfnRandomLong(0, 5) > 2) ? f : -f);
		endpos.y = origin_to_generate.y + local_velocity.y;
		endpos.z = 4096.0f;

		hl::pmtrace_t skytrace;
		cl_enginefuncs->pEventAPI->EV_SetTraceHull(2);
		cl_enginefuncs->pEventAPI->EV_PlayerTrace(origin_to_generate, endpos, PM_WORLD_ONLY, -1, &skytrace);

		char* traced_texture = (char*)cl_enginefuncs->pEventAPI->EV_TraceTexture(skytrace.ent, origin_to_generate, skytrace.endpos);

		// create fog only from sky
		if (traced_texture && strncmp(traced_texture, "sky", 3) == 0)
		{
			endpos.z = -4096.0f;

			cl_enginefuncs->pEventAPI->EV_SetTraceHull(2);
			cl_enginefuncs->pEventAPI->EV_PlayerTrace(origin_to_generate, endpos, PM_WORLD_ONLY, -1, &skytrace);

			create_wind_particle(skytrace.endpos, 75.0f);
		}
	}
}

void CEnvironmentalEffects::create_raindrop(const Vector& origin)
{
	if (!m_rain_sprite)
	{
		return;
	}

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	auto p = new hl::CPartRainDrop();
	if (!p)
	{
		return; // couldn't alloc more particles
	}

	p->InitializeSprite(origin, g_vecZero, m_rain_sprite, 2.0f, 1.0f);

	strcpy(p->m_szClassname, "particle_rain");

	p->m_flStretchY = cl_enginefuncs->pfnRandomFloat(20.0f, 40.0f);

	p->m_vVelocity.x = m_wind_span.x * cl_enginefuncs->pfnRandomFloat(1.0f, 2.0f);
	p->m_vVelocity.y = m_wind_span.y * cl_enginefuncs->pfnRandomFloat(1.0f, 2.0f);
	p->m_vVelocity.z = cl_enginefuncs->pfnRandomFloat(-500.0f, -1800.0f) * env_particle_fallspeed.get_value();

	p->m_iRendermode = hl::kRenderTransAlpha;
	p->m_flGravity = 0;

	p->m_vColor.x = p->m_vColor.y = p->m_vColor.z = 255.0f;

	p->SetCollisionFlags(TRI_COLLIDEWORLD | TRI_COLLIDEKILL | TRI_WATERTRACE);
	p->SetCullFlag((LIGHT_NONE | CULL_PVS | CULL_FRUSTUM_PLANE));

	p->m_flDieTime = cl_enginefuncs->pfnGetClientTime() + 1.0f;
}

void CEnvironmentalEffects::create_snow_flake(const Vector& origin)
{
	if (!m_snow_sprite)
	{
		return;
	}

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	auto p = new hl::CPartSnowFlake();
	if (!p)
	{
		return; // couldn't alloc more particles
	}

	p->InitializeSprite(origin, g_vecZero, m_snow_sprite, cl_enginefuncs->pfnRandomFloat(2.0, 2.5), 1.0);

	strcpy(p->m_szClassname, "snow_particle");

	p->m_iNumFrames = m_snow_sprite->numframes;

	p->m_vVelocity.x = m_wind_span.x / cl_enginefuncs->pfnRandomFloat(1.0, 2.0);
	p->m_vVelocity.y = m_wind_span.y / cl_enginefuncs->pfnRandomFloat(1.0, 2.0);
	p->m_vVelocity.z = cl_enginefuncs->pfnRandomFloat(-100.0, -200.0) * env_particle_fallspeed.get_value();

	p->SetCollisionFlags(TRI_COLLIDEWORLD);

	float r = cl_enginefuncs->pfnRandomFloat(0.0, 1.0);
	if (r < 0.1)
	{
		p->m_vVelocity.x *= 0.5;
		p->m_vVelocity.y *= 0.5;
	}
	else if (r < 0.2)
	{
		p->m_vVelocity.z = -65.0;
	}
	else if (r < 0.3)
	{
		p->m_vVelocity.z = -75.0;
	}

	p->m_iRendermode = hl::kRenderTransAdd;

	p->SetCullFlag(RENDER_FACEPLAYER | LIGHT_NONE | CULL_PVS | CULL_FRUSTUM_SPHERE);
	
	p->m_flScaleSpeed = 0;
	p->m_flDampingTime = 0;
	p->m_iFrame = 0;
	p->m_flMass = 1.0;
	p->m_flSize = (int)env_snow_flake_size.get_value() + 1.0f;

	p->m_flGravity = 0;
	p->m_flBounceFactor = 0;

	p->m_vColor.x = p->m_vColor.y = p->m_vColor.z = 128.0f;

	p->m_flDieTime = cl_enginefuncs->pfnGetClientTime() + 3.0f;

	p->m_bSpiral = cl_enginefuncs->pfnRandomLong(0, 1) != 0;

	p->m_flSpiralTime = cl_enginefuncs->pfnGetClientTime() + cl_enginefuncs->pfnRandomLong(2, 4);
}

void CEnvironmentalEffects::create_wind_particle(const Vector& origin, float max_size)
{
	if (!m_wind_particle_sprite)
	{
		return;
	}

	auto p = new hl::CPartWind();
	if (!p)
	{
		return; // couldn't alloc more particles
	}

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	float size = cl_enginefuncs->pfnRandomFloat(50.0, max_size) * env_ground_fog_size.get_value();
	p->InitializeSprite(origin + Vector(0.0f, 0.0f, 10.0f), g_vecZero, m_wind_particle_sprite, size, 1.0);

	p->m_iNumFrames = m_wind_particle_sprite->numframes;

	strcpy(p->m_szClassname, "wind_particle");

	p->m_iFrame = cl_enginefuncs->pfnRandomLong(m_wind_particle_sprite->numframes / 2, m_wind_particle_sprite->numframes);

	p->m_vVelocity.x = m_wind_span.x / cl_enginefuncs->pfnRandomFloat(1.0f, 2.0f);
	p->m_vVelocity.y = m_wind_span.y / cl_enginefuncs->pfnRandomFloat(1.0f, 2.0f);

	if (cl_enginefuncs->pfnRandomFloat(0.0, 1.0) < 0.1)
	{
		p->m_vVelocity.x *= 0.5;
		p->m_vVelocity.y *= 0.5;
	}

	p->m_iRendermode = hl::kRenderTransAlpha;

	p->m_vAVelocity.z = cl_enginefuncs->pfnRandomFloat(-1.0, 1.0);

	p->m_flScaleSpeed = 0.4f;
	p->m_flDampingTime = 0;

	p->m_iFrame = 0;
	p->m_flGravity = 0.0f;

	p->m_flMass = 1.0f;
	p->m_flBounceFactor = 0;
	p->m_vColor.x = p->m_vColor.y = p->m_vColor.z = cl_enginefuncs->pfnRandomFloat(100.0f, 140.0f);

	p->m_flFadeSpeed = -1.0f;

	p->SetCollisionFlags(TRI_COLLIDEWORLD);
	p->SetCullFlag(RENDER_FACEPLAYER | LIGHT_NONE | CULL_PVS | CULL_FRUSTUM_SPHERE);

	p->m_flDieTime = cl_enginefuncs->pfnGetClientTime() + cl_enginefuncs->pfnRandomFloat(5.0f, 8.0f);
}

void CEnvironmentalEffects::update_wind_variables()
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	static float last_speed = env_wind_speed.get_value();

	// change direction of the rain if it's time
	if (m_next_wind_change_time <= cl_enginefuncs->pfnGetClientTime() || last_speed != env_wind_speed.get_value())
	{
		float speed = 80.0f * env_wind_speed.get_value();
		m_desired_wind_direction.x = cl_enginefuncs->pfnRandomFloat(-speed, speed);
		m_desired_wind_direction.y = cl_enginefuncs->pfnRandomFloat(-speed, speed);
		m_desired_wind_direction.z = 0;

		m_next_wind_change_time = cl_enginefuncs->pfnGetClientTime() + cl_enginefuncs->pfnRandomFloat(15.0f, 30.0f);

		m_desired_wind_speed = m_desired_wind_direction.NormalizeInPlace();

		m_ideal_wind_yaw = CMath::the().vec2yaw(m_desired_wind_direction);

		last_speed = env_wind_speed.get_value();
	}

	Vector angles;
	Vector forward = m_wind_span.Normalize();

	CMath::the().vector_angles(forward, angles);

	float f = CMath::the().angle_mod(angles[YAW]);
	if (m_ideal_wind_yaw != f)
	{
		float vel = (cl_enginefuncs->pfnGetClientTime() - m_old_time) * 5.0f;
		angles[YAW] = CMath::the().approach_angle(m_ideal_wind_yaw, f, vel);
	}

	CMath::the().angle_vectors(angles, &forward, nullptr, nullptr);

	m_wind_span = forward * m_desired_wind_speed;
}

void CEnvironmentalEffects::initialize_wind_variables()
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();
	
	m_wind_span.x = cl_enginefuncs->pfnRandomFloat(-80.0f, 80.0f);
	m_wind_span.y = cl_enginefuncs->pfnRandomFloat(-80.0f, 80.0f);
	m_wind_span.z = 0;

	m_desired_wind_direction.x = cl_enginefuncs->pfnRandomFloat(-80.0f, 80.0f);
	m_desired_wind_direction.y = cl_enginefuncs->pfnRandomFloat(-80.0f, 80.0f);
	m_desired_wind_direction.z = 0;

	m_next_wind_change_time = cl_enginefuncs->pfnGetClientTime();
}

void CEnvironmentalEffects::play_ambient_rain_sound()
{
	static float prev_vol_rain = 0.0f, prev_vol_snow = 0.0f;

	play_ambient_looped_sound_helper(&env_rain, &env_rain_ambient, &env_rain_ambient_volume, &prev_vol_rain, &m_rain_sound_played, "sound/ambience/rain.wav");
	play_ambient_looped_sound_helper(&env_snow, &env_snow_ambient, &env_snow_ambient_volume, &prev_vol_snow, &m_snow_sound_played, "sound/de_torn/torn_ambience.wav");
	// NOTE: If you add more ambient sounds, don't forget to add them below.

	if (env_enable.get_value() && env_rain.get_value() && env_rain_ambient_thunder.get_value())
	{
		auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

		float time = cl_enginefuncs->pfnGetClientTime();
		if (time > m_thunder_timer)
		{
			CEngineSoundPlayer::the().play_ambient_sound_unique(0, Vector(0, 0, 0), "sound/ambience/thunder_clap.wav", 1.0f, AMBIENT_EVERYWHERE, false);
			m_thunder_timer = time + cl_enginefuncs->pfnRandomLong(20, 60);
		}
	}
}

void CEnvironmentalEffects::stop_all_ambient_sound()
{
	CEngineSoundPlayer::the().stop_sound("sound/ambience/rain.wav");
	m_rain_sound_played = false;

	CEngineSoundPlayer::the().stop_sound("sound/de_torn/torn_ambience.wav");
	m_snow_sound_played = false;

	// NOTE: Add all ambient sounds here.
}

void CEnvironmentalEffects::play_ambient_looped_sound_helper(VarBoolean* env_effect_enabled, VarBoolean* sound_enabled,
															 VarFloat* sound_volume, float* prev_volume, bool* sound_played, const char* sound_file)
{
	// TODO: Figure out a better way of doing this, this is pretty sus and looks like shit...

	bool env_enabled = env_enable.get_value();
	bool rain_enabled = env_effect_enabled->get_value();
	bool ambient_enabled = sound_enabled->get_value();
	float volume = sound_volume->get_value();

	if (ambient_enabled && env_enabled && rain_enabled && !*sound_played)
	{
		CEngineSoundPlayer::the().play_ambient_sound_unique(0, Vector(0, 0, 0), sound_file,
															volume, AMBIENT_EVERYWHERE, true);
//		CConsole::the().info("played");
		*sound_played = true;
	}

	if ((!ambient_enabled || !env_enabled || !rain_enabled || *prev_volume != volume) && *sound_played)
	{
		CEngineSoundPlayer::the().stop_sound(sound_file);
//		CConsole::the().info("stopped");
		*sound_played = false;
		*prev_volume = volume;
	}

}

//--------------------------------------------------------------------------------------------------
// Particle methods
//

namespace hl
{

void CPartRainDrop::Think(float time)
{
	Vector	right;
	Vector	vViewAngles;
	float	dotLength;

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();
	
	cl_enginefuncs->pfnGetViewAngles(vViewAngles);

	CMath::the().angle_vectors(vViewAngles, NULL, &right, NULL);

	m_vAngles[YAW] = vViewAngles[YAW];
	dotLength = DotProduct(m_vVelocity, right);
	m_vAngles[ROLL] = atan(dotLength / m_vVelocity.z) * (float)(180.0f / M_PI);

	if (m_flBrightness < 155.0f)
	{
		m_flBrightness += 6.5f;
	}

	CBaseParticle::Think(time);
}

void CPartSnowFlake::Think(float time)
{
	float flFrametime;
	float fastFreq;

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	if (m_flBrightness < 130.0 && !m_bTouched)
	{
		m_flBrightness += 4.5;
	}

	Fade(time);
	Spin(time);

	if (m_flSpiralTime <= cl_enginefuncs->pfnGetClientTime())
	{
		m_bSpiral = !m_bSpiral;

		m_flSpiralTime = cl_enginefuncs->pfnGetClientTime() + cl_enginefuncs->pfnRandomLong(2, 4);
	}

	if (m_bSpiral && !m_bTouched)
	{
		flFrametime = time - CEnvironmentalEffects::the().get_old_time();
		fastFreq = sin(flFrametime * 5.0f + (float)(int)this); // TODO: Hmmm, this is weird

		m_vOrigin = m_vOrigin + m_vVelocity * flFrametime;
		m_vOrigin.x += (fastFreq * fastFreq) * 0.3f;
	}
	else
	{
		CalculateVelocity(time);
	}

	CheckCollision(time);
}

void CPartSnowFlake::Touch(Vector pos, Vector normal, int index)
{
	if (m_bTouched)
	{
		return;
	}

	m_bTouched = true;

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	SetRenderFlag(RENDER_FACEPLAYER);

	m_flOriginalBrightness = m_flBrightness;

	m_vVelocity = g_vecZero;

	m_iRendermode = kRenderTransAdd;

	m_flFadeSpeed = 0;
	m_flScaleSpeed = 0;
	m_flDampingTime = 0;
	m_iFrame = 0;
	m_flMass = 1.0;
	m_flGravity = 0;

	m_vColor.x = m_vColor.y = m_vColor.z = 128.0f;

	m_flDieTime = cl_enginefuncs->pfnGetClientTime() + env_snow_flake_die_time.get_value();

	m_flTimeCreated = cl_enginefuncs->pfnGetClientTime();
}

// TRUE if in water and returns vecResult.
// utility function
static hl::qboolean water_entry_point(hl::pmtrace_t* pTrace, const Vector& vecSrc, Vector& vecResult)
{
	Vector	result;
	Vector	a, b, c;
	Vector	vecDir;
	float	fTolerance;

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	if (cl_enginefuncs->pfnPM_PointContents(pTrace->endpos, NULL) == cl_enginefuncs->pfnPM_PointContents((Vector&)vecSrc, NULL))
	{
		return FALSE;
	}

	a = vecSrc;
	b = pTrace->endpos;
	vecDir = b - a;

	fTolerance = 4.0f;
	while (vecDir.Length() > fTolerance)
	{
		vecDir /= 2.0f; // half the distance each iteration

		c = a + vecDir;

		if (cl_enginefuncs->pfnPM_PointContents(c, NULL) == cl_enginefuncs->pfnPM_PointContents(a, NULL))
		{
			a = c;
			result = c;
		}
		else
		{
			b = c;
			result = a;
		}

		vecDir = b - result;
	}

	vecResult = result;
	return TRUE;
}

void CPartRainDrop::Touch(Vector pos, Vector normal, int index)
{
	Vector			start, end, pt;
	pmtrace_t		tr;
	Vector			vRippleAngles, angles;
	CBaseParticle*	p;
	model_t*		pSprite;

	if (m_bTouched)
	{
		return;
	}

	m_bTouched = true;

	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	start = m_vOrigin;
	start.z += 32.0f;

	end = m_vOrigin;
	end.z -= 16.0f;

	cl_enginefuncs->pEventAPI->EV_PlayerTrace(start, end, PM_WORLD_ONLY, -1, &tr);

	angles.x = normal.x;
	angles.y = normal.y;
	angles.z = -normal.z;

	CMath::the().vector_angles(angles, vRippleAngles);

	//
	// see if we dropped into water
	//
	if (water_entry_point(&tr, start, pt))
	{
		p = new CBaseParticle();
		if (!p)
		{
			return; // couldn't alloc more particles
		}

		pSprite = CEnvironmentalEffects::the().get_ripple_sprite();

		p->InitializeSprite(pt, vRippleAngles, pSprite, 15.0f, 110.0f);

		p->m_iRendermode = kRenderTransAdd;
		p->m_flScaleSpeed = 1.0f;
		p->m_flFadeSpeed = 2.0f;

		p->m_vColor.x = p->m_vColor.y = p->m_vColor.z = 255.0f;

		p->SetCullFlag(LIGHT_INTENSITY | CULL_PVS | CULL_FRUSTUM_SPHERE);

		p->m_flDieTime = cl_enginefuncs->pfnGetClientTime() + 2.0f;
	}
	else
	{
		p = new CBaseParticle();
		if (!p)
		{
			return; // couldn't alloc more particles
		}

		pSprite = CEnvironmentalEffects::the().get_splash_sprite();

		p->InitializeSprite(m_vOrigin + normal, Vector(90.0f, 0, 0), pSprite, (float)cl_enginefuncs->pfnRandomLong(20, 25), 125.0f);

		p->m_iRendermode = kRenderTransAdd;

		p->m_flMass = 1.0f;
		p->m_flGravity = 0.1f;

		p->m_vColor.x = p->m_vColor.y = p->m_vColor.z = 255.0f;

		p->m_iNumFrames = pSprite->numframes - 1;
		p->m_iFramerate = cl_enginefuncs->pfnRandomLong(30, 45);
		p->SetCollisionFlags(TRI_ANIMATEDIE);
		p->SetRenderFlag(RENDER_FACEPLAYER);
		p->SetCullFlag(LIGHT_INTENSITY | CULL_PVS | CULL_FRUSTUM_SPHERE);

		p->m_flDieTime = cl_enginefuncs->pfnGetClientTime() + 0.3f;
	}
}

void CPartWind::Think(float flTime)
{
	auto cl_enginefuncs = CMemoryHookMgr::the().cl_enginefuncs();

	if (m_flDieTime - flTime <= 3.0f)
	{
		if (m_flBrightness > 0.0f)
		{
			m_flBrightness -= (flTime - m_flTimeCreated) * 0.4f;
		}

		if (m_flBrightness < 0.0f)
		{
			m_flBrightness = 0.0f;
			flTime = m_flDieTime = cl_enginefuncs->pfnGetClientTime();
		}
	}
	else
	{
		if (m_flBrightness < 105.0f)
		{
			m_flBrightness += (flTime - m_flTimeCreated) * 5.0f + 4.0f;
		}
	}

	CBaseParticle::Think(flTime);
}

} // namespace hl