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

InCommand CFrameSkipper::frame_skipper = InCommand("frame_skipper", NULL, false, IN_ACTCOND_None);

VarInteger frame_skip_amount("frame_skip_amount", "Amount of frames to skip each update", 2, 2, 20);
VarInteger frame_skip_method("frame_skip_method", "Frame skipper method", 0, 0, 1);
VarInteger frame_skip_type("frame_skip_type", "Type of the frame skipper. See menu.", 0, 0, 1);

bool CFrameSkipper::skip_current_frame()
{
	if (!frame_skipper.is_active())
	{
		return false;
	}

	// skip N frames
	int N = frame_skip_amount.get_value();

	int type = frame_skip_type.get_value();
	static uint32_t frames = 0;
	switch (type)
	{
		case 0: // skip every N frames
		{
			if (++frames % N != 0)
			{
				//CConsole::the().info("yes");
				return true; // skip
			}
			break;
		}
		case 1: // skip every Nth frame
		{
			if (++frames % N == 0)
			{
				//CConsole::the().info("yes");
				return true; // skip
			}
			break;
		}
	}

	//CConsole::the().info("no");
	return false; // don't skip
}
