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
#include <vector.h>

#include <interface/IDeveloperConsole.h>
#include <interface/IBaseModule.h>
#include <interface/IRegistry.h>
#include <interface/IFileSystem.h>
#include <interface/IAppDataManager.h>

#include <interface/gui/IGLFWApp.h>
#include <interface/gui/IGUIWidgets.h>
#include <interface/gui/IGUIFontManager.h>
#include <interface/gui/IGUIThemeManager.h>
#include <interface/gui/IGUIWindowRendering.h>

#include <tier/GenericUtil.h>
#include <tier/StringTools.h>
#include <tier/MessageBox.h>

#include <injection/IBaseInjector.h>
#include <injection/BaseInjector.h>

#include <DependencyLoader.h>

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
#include <unordered_map>
#include <functional>
#include <set>
#include <assert.h>

//-------------------------------------------------------------------
//
// HLSDK
// 
//-------------------------------------------------------------------
namespace hl
{
#include <hlsdk/interface/IFileSystem.h>
#include <hlsdk/interface/IEngineAPI.h>
} // namespace hl

//-------------------------------------------------------------------
//
// Current project
// 
//-------------------------------------------------------------------
#include "loader.h"
#include "loaderui.h"
#include "DependencyLoader.h"

#endif // PRECOMPILED_H