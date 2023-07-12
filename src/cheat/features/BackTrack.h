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

#ifndef BACKTRACK_H
#define BACKTRACK_H
#pragma once

extern VarBoolean backtrack_enable;
extern VarInteger backtrack_team;
extern VarBoolean backtrack_self;

class CBacktrack
{
public:
	DECL_BASIC_CLASS(CBacktrack);

public:
	void update();

private:
	bool find_interpolation_updates(hl::cl_entity_t* ent, float targettime, hl::position_history_t** ph0, hl::position_history_t** ph1);

	float bind_interpolation_fraction(float frac);

	float calc_update_interval_ms(float updaterate);
	float calc_true_latency(float updaterate, float max_unlag, float ft, float client_latency, float update_interval_ms);
	float calc_interpolation_amount(float ex_interp, float update_interval_ms, float lerp_msec);
	float get_raw_client_latency(hl::frame_t* frame);

	// simulate interpolated origin & angles
	bool backtrack_entity(hl::cl_entity_t* ent, float lerp_msec, Vector& simorg, Vector& simang);

	void construct_backtrack_entity(hl::cl_entity_t& in, const Vector& interpolated_origin, const Vector& interpolated_angles);
};

#endif // BACKTRACK_H