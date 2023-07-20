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
// BackTrack.cpp -- Backtracking of a player aka client-side lag compensation exploits.
// 
// Quick note: This code was originally "coded" by either xvi or killabeez however, they copy-pasted
//			   it from the 03 leak. I will just use my engine knowledge and knowledge about networking
//			   and client-side engine code, and make the code as good as possible. ;)
// 
// https://github.com/BlackRa1nFR/source-2003/blob/abb4ba70b1d3ad3f706ed71ac309e5b7e85f6103/src_main/dlls/player_lagcompensation.cpp#L285
//

#include "precompiled.h"

// ~ from 03 leak lag compensation code
#define LAG_COMPENSATION_TELEPORTED_DISTANCE_SQR ( 64.0f * 64.0f )
#define LAG_COMPENSATION_EPS_SQR ( 0.1f * 0.1f )
// Allow 4 units of error ( about 1 / 8 bbox width )
#define LAG_COMPENSATION_ERROR_EPS_SQR ( 4.0f * 4.0f )
// Only keep 1 second of data
#define LAG_COMPENSATION_DATA_TIME	1.0f

VarBoolean backtrack_enable("backtrack_enable", "Enables backtracking on players", false);
VarInteger backtrack_team("backtrack_team", "Which team to backtrack on", 0, 0, 4);
VarBoolean backtrack_self("backtrack_self", "Applies backtracking on your player", false);

void CBacktrack::update()
{
	if (!backtrack_enable.get_value())
	{
		return;
	}

	auto cmd = CClientMovementPacket::the().get_cmd();

	if (!cmd)
	{
		return;
	}

	for (auto& [index, player] : CEntityMgr::the().m_known_players)
	{
		if (!player.is_valid() || (player.is_local_player() && !backtrack_self.get_value()) || player.is_local_spectating() || !player.is_alive() || player.is_out_of_update_for(1.0f))
		{
			continue;
		}

		auto cl_ent = player.cl_entity();

		Vector simorg, simang;
		if (!backtrack_entity(cl_ent, cmd->lerp_msec, simorg, simang))
		{
			continue;
		}

		int selected_team = backtrack_team.get_value();

		auto local = CLocalState::the().local_player();

		int player_team = player.get_team();
		int local_team = local ? local->get_team() : hl::UNASSIGNED;

		if (
			// draw entity for only terrorists
			(selected_team == 0 && player_team == hl::TERRORIST) ||
			// draw entity for only cts
			(selected_team == 1 && player_team == hl::CT) ||
			// draw entity for only our team
			(selected_team == 2 && player_team == local_team) ||
			// draw entity for only enemy team
			(selected_team == 3 && player_team != local_team) ||
			// draw entity for both teams
			(selected_team == 4 && (player_team == hl::TERRORIST || player_team == hl::CT)))
		{
			// this needs to be static because we will carry this pointer outside of this call stack
			static hl::cl_entity_t copy;

			copy = *cl_ent;

			// construct fake entity for backtracked player
			construct_backtrack_entity(copy, simorg, simang);

			CFakePlayerRenderer::the().render_player(copy, ENT_ID_BACKTRACK);
		}
	}
}

// Finds two history timestamps depending on the targettime and extrapolation settings. (ex_interp). 
//
// Returns TRUE if we're extrapolating
//
// ph0	-	Newer interpolated position history
// ph1	-	Older interpolated position history
bool CBacktrack::find_interpolation_updates(hl::cl_entity_t* ent, float targettime,
											hl::position_history_t** ph0, hl::position_history_t** ph1)
{
	int		i, i0, i1, imod;
	float	at;
	bool	extrapolate;

	imod = ent->current_position;
	i0 = (imod - 0) & HISTORY_MASK; // Position history for this ph record
	i1 = (imod - 1) & HISTORY_MASK; // Position history for last update ph record

	extrapolate = true;

	//
	// accumulate through saved history positions and see if we're ahead
	// of the animation time for current animation time record, if yes, then
	// we interpolate. Otherwise we extrapolate.
	//

	if (targettime <= ent->ph[i0].animtime)
	{
		for (i = 1; i < HISTORY_MAX - 1; i++)
		{
			// Get the movement time for this particular history record
			at = ent->ph[(imod - i) & HISTORY_MASK].animtime;
			if (at == 0.0f)
				break;

			// see if we're not ahead of our time
			if (targettime > at)
			{
				i0 = ((imod - i) + 1) & HISTORY_MASK;
				i1 = ((imod - i) + 0) & HISTORY_MASK;
				extrapolate = false;
				break;
			}
		}
	}

	//
	// return back
	//

	if (ph0)
		*ph0 = &ent->ph[i0];
	if (ph1)
		*ph1 = &ent->ph[i1];

	return extrapolate;
}

// Bounds fraction to [0.0f, 1.2f]
float CBacktrack::bind_interpolation_fraction(float frac)
{
	return std::clamp(frac, 0.0f, 1.2f);
}

// calculate how fast we've set to receive updates from the server, in milliseconds.
float CBacktrack::calc_update_interval_ms(float updaterate)
{
	// Note that cl_updaterate
	return 1.0f / std::max(updaterate, 10.0f);
}

// add in fake latency, too
float CBacktrack::get_raw_client_latency(hl::frame_t * frame)
{
	// fake latency we simulate through sending server history sequences
	float lat = 0.0f;
	if (fake_latency.get_value())
	{
		lat = CNetchanSequenceHistory::the().get_desired_fake_latency();
	}
	else
	{
		// actual latency calculated by the engine this frame
		// the engine latency is delta between current engine time and time of command from last incoming_acknowledged sequence.
		lat = frame->latency;
	}

	return lat;
}

float CBacktrack::calc_true_latency(float updaterate, float max_unlag, float ft, float client_latency, float update_interval_ms)
{
	// cap at 1500 ms
	float l = std::min(client_latency, 1.5f);

	// fixup delay based on message interval (cl_updaterate, default 20 so 50 msec)
	l -= update_interval_ms;

	// further fixup due to client side delay because packets arrive 1/2 through the frame loop, on average.
	// note: this is really minor, should have minimal effects.
	l -= ft/* * 0.5*/;

	// absolute bounds on lag compensation (1s)
	float correction = std::min(LAG_COMPENSATION_DATA_TIME, l);

	// see if server is applying a lower cap
	if (max_unlag)
	{
		// make sure it's not negative
		if (max_unlag < 0.0f)
			max_unlag = 0.0f;

		// apply server cap
		correction = std::min(correction, max_unlag);
	}

	return correction;
}

float CBacktrack::calc_interpolation_amount(float ex_interp, float update_interval_ms, float lerp_msec)
{
	// remove lag based on player interpolation, as well
	return std::min((lerp_msec == -1) ? ex_interp : (lerp_msec / 1000.0f), 0.1f);
}

bool CBacktrack::backtrack_entity(hl::cl_entity_t* ent, float lerp_msec, Vector& simorg, Vector& simang)
{
	auto ex_interp = CGoldSrcCommandMgr::the().get_cvar("ex_interp");			// how far to the past we want to extrapolate
	auto sv_unlagpush = CGoldSrcCommandMgr::the().get_cvar("sv_unlagpush");		// a small amount that server can add to target time
	auto cl_updaterate = CGoldSrcCommandMgr::the().get_cvar("cl_updaterate");	// receive # updates per second from the server
	auto sv_maxunlag = CGoldSrcCommandMgr::the().get_cvar("sv_maxunlag");		// number of seconds to store player positions for.

	if (!ex_interp || !sv_unlagpush || !cl_updaterate || !sv_maxunlag)
	{
		CConsole::the().derror("Something went horribly wrong. Couldn't get ex_interp, sv_unlagpush, etc.");
		return false;
	}

	// client-side data
	float ft = CLocalState::the().get_engine_frametime();			// engine frametime
	auto frame = CLocalState::the().get_current_frame();		// latest frame data we received from server
	float realtime = CMemoryHookMgr::the().cl().get()->time;	// engine timestamp

	// capped update rate interval
	float update_interval_ms = calc_update_interval_ms(cl_updaterate->value);

	// get raw clientside latency calculated by the engine + our fake latency
	float raw_client_latency = get_raw_client_latency(frame);

	// calculate the true latency based on some major factors
	float true_client_latency = calc_true_latency(cl_updaterate->value, sv_maxunlag->value, ft, raw_client_latency, update_interval_ms);

	// this will be our interpolant
	float target_time = realtime - true_client_latency;

	// take to a count client interpolation
	float client_interp_amount = calc_interpolation_amount(ex_interp->value, update_interval_ms, lerp_msec);
	target_time -= client_interp_amount;

	// server can apply a fudge, probably not needed, defaults to 0.0f
	target_time += sv_unlagpush->value;

	// don't run to the future on our local time
	target_time = std::min(realtime, target_time);

//	CConsole::the().info("lat: {}", realtime - target_time);

	hl::position_history_t* newer, *older;
	find_interpolation_updates(ent, target_time, &newer, &older);

	if (!newer || !older)
	{
		return false;
	}

	if (newer->animtime != 0)
	{
		Vector pos, ang;
		Vector delta;
		float frac;

		delta = newer->origin - older->origin;

		if (newer->animtime != older->animtime)
		{
			frac = (target_time - older->animtime) / (newer->animtime - older->animtime);
		}
		else
		{
			frac = 1.0f;
		}

		frac = bind_interpolation_fraction(frac);

		// interpolate position and angles based on fraction
		CMath::the().vector_ma(older->origin, frac, delta, pos);
		CMath::the().vector_ma(older->angles, frac, delta, ang);

		simorg = pos;
		simang = ang;
	}
	else
	{
		simorg = older->origin;
		simang = older->angles;
	}

	return true;
}

void CBacktrack::construct_backtrack_entity(hl::cl_entity_t& in, const Vector& interpolated_origin, const Vector& interpolated_angles)
{
	in.origin = interpolated_origin;
	in.angles = interpolated_angles;

	// don't render weapon
	in.curstate.weaponmodel = 0;

	// without any effects
	in.curstate.renderamt = 0;
	in.curstate.rendercolor = {};
	in.curstate.renderfx = hl::kRenderFxNone;
	in.curstate.rendermode = hl::kRenderNormal;
}
