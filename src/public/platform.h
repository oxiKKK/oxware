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
// platform.h -- Platform-specific lowlevel declarations.
//

#ifndef PLATFORM_H
#define PLATFORM_H
#pragma once

#include "version.h"

// Disable that stupid warning
#define _CRT_SECURE_NO_WARNINGS

#define DLLEXPORT						__declspec(dllexport)
#define C_DLLEXPORT			extern "C"	__declspec(dllexport)

//---------------------------------------------------------------------------------
// PHNT, thank you guys
// 
//	https://github.com/processhacker/phnt
//---------------------------------------------------------------------------------

//#define PHNT_VERSION PHNT_WINXP // Windows XP
//#define PHNT_VERSION PHNT_WS03 // Windows Server 2003
//#define PHNT_VERSION PHNT_VISTA // Windows Vista
//#define PHNT_VERSION PHNT_WIN7 // Windows 7
//#define PHNT_VERSION PHNT_WIN8 // Windows 8
//#define PHNT_VERSION PHNT_WINBLUE // Windows 8.1
#define PHNT_VERSION PHNT_THRESHOLD // Windows 10

//#define NOGDI
#define NOMINMAX // FUCK YOU

#include <phnt/phnt_windows.h>
#include <phnt/phnt.h>

#include <winsock.h>
#include <knownfolders.h>

#ifdef small
#undef small // bruh
#endif

//---------------------------------------------------------------------------------

// Bypass the POSIX deprecation errors
#define stricmp _stricmp
#define strnicmp _strnicmp
#define wcsicmp _wcsicmp

#include "basetypes.h"

#include "custom_format.h"

#include <chrono>
using namespace std::chrono_literals;

// nlohmann json
#include "json.h"

#endif // PLATFORM_H