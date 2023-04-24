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

#ifndef PRECOMPILED_H
#define PRECOMPILED_H
#pragma once

//-------------------------------------------------------------------
//
// Public headers
// 
//-------------------------------------------------------------------
#include <commondefs.h>
#include <interface.h>

#include <interface/IBaseModule.h>
#include <interface/IDeveloperConsole.h>

#include <tier/GenericUtil.h>
#include <tier/MessageBox.h>

#include <ByteBuffer.h>

//-------------------------------------------------------------------
//
// Shared
// 
//-------------------------------------------------------------------
#include "Console.h"
#include "BaseVariable.h"

//-------------------------------------------------------------------
//
// C++ Standard
// 
//-------------------------------------------------------------------
#include <filesystem>
#include <unordered_map>
#include <TlHelp32.h>

//-------------------------------------------------------------------
//
// Current project
// 
//-------------------------------------------------------------------
#include <injection/IBaseInjector.h>
#include <injection/BaseInjector.h>
#include <injection/PortableExecutableObject.h>
#include <injection/VirtualMemoryManager.h>
#include <injection/InjectedDllIPCInterface.h>

#include "RtlIIFT_bytepattern_search.h"

#endif // PRECOMPILED_H