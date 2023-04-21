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

VarBoolean frame_skip_enable("frame_skip_enable", "Enables frame skipping exploit", false);
VarInteger frame_skip_amount("frame_skip_amount", "Amount of frames to skip each update", 2, 2, 20);
VarInteger frame_skip_maxfps("frame_skip_maxfps", "FPS limit when frame skipping is on", 1000, 300, 1500);

bool CFrameSkipper::skip_current_frame()
{
	if (!frame_skip_enable.get_value())
	{
		return false;
	}

	// skip N frames
	int N = frame_skip_amount.get_value();

	bool enforce_skip = (1.0 / CGameUtil::the().get_engine_frametime() > frame_skip_maxfps.get_value());

	static uint32_t frames = 0;
	if (++frames % N != 0 && !enforce_skip)
	{
		//CConsole::the().info("yes");
		return true; // skip
	}
	else
	{
		//CConsole::the().info("no");
		return false; // don't skip
	}
}
