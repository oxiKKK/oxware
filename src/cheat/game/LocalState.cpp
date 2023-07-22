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

void CLocalState::update_pre_clientmove(float frametime, hl::usercmd_t * cmd)
{
	auto cl = CMemoryHookMgr::the().cl().get();
	m_last_viewangles = cl->viewangles;
}

void CLocalState::update_clientmove(float frametime, hl::usercmd_t* cmd)
{
	// cache everything related to our client.

	m_engine_frametime = frametime;

	auto cl = CMemoryHookMgr::the().cl().get();
	m_vieangle_delta = Vector2D(cl->viewangles[YAW] - m_last_viewangles[YAW], cl->viewangles[PITCH] - m_last_viewangles[PITCH]);
	m_vieangle_delta *= -1.0f;

	// round to zero
	if (m_vieangle_delta.x < 0.01 && m_vieangle_delta.x > -0.01) m_vieangle_delta.x = 0.0f;
	if (m_vieangle_delta.y < 0.01 && m_vieangle_delta.y > -0.01) m_vieangle_delta.y = 0.0f;

	m_pmove = *CMemoryHookMgr::the().pmove().get();

	m_current_frame = &cl->frames[cl->parsecountmod];

	// playermove flags
	m_player_flags = m_current_frame->clientdata.flags;

	// set this first! because the tracing code needs it!
	m_tracing_hull = is_ducking() ? HULL_DUCKING : HULL_STANDING;

	m_moving_velocity = m_pmove->velocity;
	m_ground_angle = CGameUtil::the().compute_ground_angle_for_origin(m_pmove->origin);
	m_ground_dist = CGameUtil::the().compute_distance_to_ground(m_pmove->origin);
	m_edge_dist = CGameUtil::the().compute_edge_distance(m_pmove->origin);
	m_is_surfing = m_ground_angle > 45.0f && m_ground_dist < 30.0f;

	// in-game chat
	m_in_messagemode = *CMemoryHookMgr::the().key_dest().get() == hl::key_message;

	// local player
	auto local = CEntityMgr::the().get_local_player();
	m_local_player = local ? *local : nullptr;

	if (m_local_player)
	{
		if (!m_local_player->is_valid() || !m_local_player->is_alive())
		{
			// reset, we don't want it then
			m_local_player = nullptr;
		}
	}

	if (CHLInterfaceHook::the().ISteamUser())
	{
		// TODO: set only once?
		m_local_steamid = CHLInterfaceHook::the().ISteamUser()->GetSteamID();
	}
}

int CLocalState::get_player_flags()
{
	return m_player_flags;
}

bool CLocalState::player_can_shoot()
{
	return m_current_frame->clientdata.iuser3 & PLAYER_CAN_SHOOT;
}

bool CLocalState::player_freeze_time_over()
{
	return m_current_frame->clientdata.iuser3 & PLAYER_FREEZE_TIME_OVER;
}

bool CLocalState::player_in_bomb_zone()
{
	return m_current_frame->clientdata.iuser3 & PLAYER_IN_BOMB_ZONE;
}

bool CLocalState::player_holding_shield()
{
	return m_current_frame->clientdata.iuser3 & PLAYER_HOLDING_SHIELD;
}

bool CLocalState::is_ducking()
{
	return m_player_flags & FL_DUCKING;
}

bool CLocalState::is_on_ground()
{
	return m_player_flags & FL_ONGROUND;
}

bool CLocalState::is_on_ground_safe()
{
	// HACKHACK
	return is_on_ground() || m_ground_dist < 0.1f;
}

float CLocalState::get_fov()
{
	return *CMemoryHookMgr::the().scr_fov_value().get();
}

float CLocalState::get_local_velocity_2d()
{
	return m_moving_velocity.Length2D();
}

float CLocalState::get_local_velocity_3d()
{
	return m_moving_velocity.Length();
}

Vector CLocalState::get_local_velocity_vec()
{
	return m_pmove->velocity;
}

hl::clientdata_t* CLocalState::get_current_frame_clientdata()
{
	return &m_current_frame->clientdata;
}

Vector CLocalState::get_origin()
{
	return m_pmove->origin;
}

float CLocalState::get_fall_velocity()
{
	return m_pmove->flFallVelocity;
}

float CLocalState::get_fall_velocity_abs()
{
	return abs(m_pmove->flFallVelocity);
}

float CLocalState::get_maxspeed()
{
	return m_pmove->maxspeed;
}

float CLocalState::get_gravity()
{
	return m_pmove->gravity != 0.0f ? (m_pmove->gravity * m_pmove->movevars->gravity) : m_pmove->movevars->gravity;
}
float CLocalState::get_ground_angle()
{
	return m_ground_angle;
}

float CLocalState::get_ground_dist()
{
	return m_ground_dist;
}

float CLocalState::get_edge_dist()
{
	return m_edge_dist;
}

bool CLocalState::is_surfing()
{
	return m_is_surfing;
}

bool CLocalState::is_on_ladder()
{
	return m_pmove->movetype == MOVETYPE_FLY;
}

int CLocalState::get_movetype()
{
	return m_pmove->movetype;
}

int CLocalState::get_waterlevel()
{
	return m_pmove->waterlevel;
}

EPlayerHull CLocalState::get_current_hull()
{
	return (EPlayerHull)m_pmove->usehull;
}

// on some servers the hull is being set to 2 and remainsant, idk why.
// resolve the hull for ourselfs, otherwise tracing code will fail
EPlayerHull CLocalState::get_current_hull_tracing()
{
	return m_tracing_hull;
}

Vector2D CLocalState::get_viewangle_delta()
{
	return m_vieangle_delta;
}

hl::frame_t* CLocalState::get_current_frame()
{
	return m_current_frame;
}

// only set when local player object is valid and alive.
CGenericPlayer* CLocalState::local_player()
{
	return m_local_player;
}

int CLocalState::get_spectating_player()
{
	return get_current_frame_clientdata()->iuser2;
}

ESpectatingMode CLocalState::get_spectating_mode()
{
	return (ESpectatingMode)get_current_frame_clientdata()->iuser1;
}

bool CLocalState::is_in_messagemode()
{
	return m_in_messagemode;
}

bool CLocalState::is_in_spectator_mapview()
{
	auto spect_mode = get_spectating_mode();

	return spect_mode == SPECT_MAP_FREE || spect_mode == SPECT_MAP_CHASE;
}

double CLocalState::get_engine_frametime()
{
	return m_engine_frametime;
}

hl::CSteamID CLocalState::get_local_steamid()
{
	return m_local_steamid;
}
