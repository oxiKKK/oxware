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

VarBoolean nonsteam_fps_fix("nonsteam_fps_fix", "Fixes slow-motion with higher fps on nonsteam cs", false);

void CNonSteamFpsFixer::fix_fps()
{
	if (!nonsteam_fps_fix.get_value())
	{
		return;
	}

	if (COxWare::the().get_build_number() > FIRST_CLMOVE_DPS_DEPENDENCE_BUILD)
	{
		return;
	}

	static double frametime_remainder = 0.0;
	double host_frametime = CLocalState::the().get_engine_frametime();
	auto cl = CMemoryHookMgr::the().cl().get();

	//
	// mirrors 8684 implementation.. thanks, random guy
	//https://github.com/ValveSoftware/halflife/issues/1940#issuecomment-13052903
	//
	// fun fact: valve acutally used this code and copy-pasted it into CL_Move.. poor Valve...
	//

	double dmsec = host_frametime * 1000.0;
	int msec = (int)dmsec; // take the base part without a fraction.

	frametime_remainder += dmsec - msec; // increment by the fraction

	if (frametime_remainder > 1.0)
	{
		frametime_remainder--;
		msec++;
	}

	if (msec > MAX_COMMAND_DURATION)
		msec = MAX_COMMAND_DURATION;

	cl->cmd.msec = msec;
	CClientMovementPacket::the().svside_movement_speed_factor(msec);
}
