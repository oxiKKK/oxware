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

VarBoolean speed_control_enable("speed_control_enable", "Enables in-game speed controlling", false);

VarFloat speed_control_amount_engine("speed_control_amount_engine", "Speed intensity of engine speedhack", 1.0f, 0.1f, 50.0f);
VarInteger speed_control_amount_server("speed_control_amount_server", "Speed intensity of server speedhack", 1, 0, 50);
VarFloat speed_control_amount_net("speed_control_amount_net", "Speed intensity of net speedhack", 0.0f, 0.01f, 50.0f);

VarBoolean speed_control_invert("speed_control_invert", "Applies 1/value to every speehack type", false);
VarBoolean speed_control_negative("speed_control_negative", "Makes every speehack value negative", false);

void CEngineSpeedControl::update()
{
	if (!speed_control_enable.get_value())
	{
		reset();
		return;
	}

	float engine_value = speed_control_amount_engine.get_value();
	int server_value = speed_control_amount_server.get_value();
	float net_value = speed_control_amount_net.get_value();

	bool invert = speed_control_invert.get_value();
	bool negative = speed_control_negative.get_value();

	//
	// run speedhacks but only if not off.
	//

	if (engine_value != speed_control_amount_engine.get_min())
	{
		if (invert) engine_value = 1.0f / engine_value;
		if (negative) engine_value *= -1.0f;
		classic_cs16_cheating_scene_speedhack(engine_value);
	}
	else
	{
		reset(); // optimizations
	}

	if (server_value != speed_control_amount_server.get_min())
	{
		if (negative) engine_value *= -1;
		server_speedhack(server_value);
	}

	if (net_value != speed_control_amount_net.get_min())
	{
		if (invert) engine_value = 1.0f / engine_value;
		if (negative) engine_value *= -1.0f;
		realtime_speedhack(net_value);
	}
}

void CEngineSpeedControl::server_speedhack(uint8_t msec, bool override_previous)
{
	CClientMovementPacket::the().svside_movement_speed_factor(msec, override_previous);
}

void CEngineSpeedControl::classic_cs16_cheating_scene_speedhack(double new_speed)
{
	// Well oh well. Here we come, with the CS 1.6 classic.. the "Texture load: %6.1fms\n" speedhack...
	//
	// You may seem this speedhack in several old cheat sources.. But what it acutally hooks may fucking shock
	// you to your fucking core.. So, it hooks a compiler-generated constant, a 1000.0 ... You don't believe me?
	// Well, take a look at this.
	// 
	//													   This is it
	//													   v
	// .text:01D40960 DC 0D E0 D2 E1 01         fmul    >ds:random_1k_speedhack_modifier_constant<
	// .text:01D40966 DD 1C 24                  fstp    [esp+553D4h+var_553D4] ; char
	// .text:01D40969 68 64 1B E4 01            push    offset aTextureLoad61f ; "Texture load: %6.1fms\n"
	//
	// But man, this is just a normal global variable declared by Valve, what are you talking about?! Weeell... let's see then...
	// Let's take a look at the code (and btw, this is Mod_LoadTextures function, but this global variable can be found elsewhere.)
	// 
	// --------------------------------------------------------------------
	// }
	// LABEL_82:
	//   v39 = Sys_FloatTime();
	//   Con_DPrintf("Texture load: %6.1fms\n", (v39 - v43) * 1000.0);
	// }
	// --------------------------------------------------------------------
	// 
	// So where is the global variable? :thinking: .... Well, it's the fucking "1000.0"! Yes, that's fucking right!
	// And now.. what does that mean? Well, it means that this variable is used within the code in several functions inside
	// mathematical formulas, why? because it's a fucking 1000.0... for example used inside:
	// 
	//	CL_Move, CL_AdjustClock, CL_RunUsercmd <<< btw, all of these client-side functions related to movement.
	//	and more...
	// 
	// Inside all of these functions, it is used in mathematical formulas, as said earlier, in particular, inside time-related
	// formulas, such as this one inside CL_RunUsercmd:
	// 
	// --------------------------------------------------------------------
	// pmove->time = *pfElapsed * 1000.0; <<< the variable on the right
	// --------------------------------------------------------------------
	//
	// and in other places similar to this one, too...
	//
	// Well, so if we hook this variable, and modify it let's say by 0.001, we modify the time base of the playermove code? Hell yeah!
	// But what is the most disturbing about this is that the occurence of this global variable is totally "random", and anyone who hooked
	// it first, didn't have a fucking clue (I assume) what it will actually do. I assume it went like this: "Oh, let me just fucking hook
	// random memory location that is used in a bunch of client-side functions, and see what it will do if a multiply it by a number... Wow
	// I just created a speedhack!"... well, pretty hilarious.. isn't it?
	// 
	// How impressive or dumb or hilarious or idiotic it may seem, it really works well... Jeez... :D
	//

	static double prev_random_thing_xd = 1.0;
	if (prev_random_thing_xd != new_speed)
	{
		double* totally_random_stuff_ptr = CMemoryHookMgr::the().random_1k_speedhack_modifier_constant().get();

		CGenericUtil::the().push_page_protection((uintptr_t)totally_random_stuff_ptr, sizeof(double), PAGE_EXECUTE_READWRITE);
		*totally_random_stuff_ptr = new_speed * 1000.0;
		CGenericUtil::the().pop_page_protection();

		prev_random_thing_xd = new_speed;
	}
}

// Note, we don't have to reset this, this is a global variable that gets reseted each time in Host_FilterTime().
void CEngineSpeedControl::realtime_speedhack(double new_speed)
{
	double* realtime = CMemoryHookMgr::the().realtime().get();

	double old = *realtime;
	*realtime += new_speed / 1000.0;

	CConsole::the().dinfo("realtime: {} -> {}", old, *realtime);
}

void CEngineSpeedControl::reset()
{
	// reset this crap back to it's original state.
	// TODO: Find a better way of doing this kind of speedhack, i think that modifying the multiplier 
	//       in CL_Move and CL_RunUsercmd would create the same effect...
	CEngineSpeedControl::the().classic_cs16_cheating_scene_speedhack(1.0);
}