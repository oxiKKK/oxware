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

#ifndef ASSETS_TEXTURES_H
#define ASSETS_TEXTURES_H
#pragma once

// bot pfp jpeg
extern unsigned int g_bot_pfp_size_raw;
extern unsigned int g_bot_pfp_size;
extern unsigned char g_bot_pfp[4752];

// nonsteam pfp jpeg
extern unsigned int g_nonsteam_pfp_size_raw;
extern unsigned int g_nonsteam_pfp_size;
extern unsigned char g_nonsteam_pfp[5486];

// invalid pfp jpeg
extern unsigned int g_invalid_pfp_size_raw;
extern unsigned int g_invalid_pfp_size;
extern unsigned char g_invalid_pfp[4574];

// avatar not found
extern unsigned int g_avatar_not_found_size_raw;
extern unsigned int g_avatar_not_found_size;
extern unsigned char g_avatar_not_found[5865];

// checkerboard
extern unsigned int g_checkerboard_size_raw;
extern unsigned int g_checkerboard_size;
extern unsigned char g_checkerboard[2246];

#endif // ASSETS_TEXTURES_H