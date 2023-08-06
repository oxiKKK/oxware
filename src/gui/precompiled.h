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
#include <interface/IConfigManager.h>
#include <interface/IRegistry.h>

#include <interface/gui/IGLFWApp.h>
#include <interface/gui/IGUIWidgets.h>
#include <interface/gui/IGUIThemeManager.h>
#include <interface/gui/IGUIFontManager.h>
#include <interface/gui/IImGUIPlatformLayer.h>
#include <interface/gui/IGUIWindowRendering.h>

#include <tier/MessageBox.h>

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
#include <mutex>
#include <unordered_set>

//-------------------------------------------------------------------
//
// GLFW
// 
//-------------------------------------------------------------------
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

//-------------------------------------------------------------------
//
// SOIL
// 
//-------------------------------------------------------------------
#include <SOIL/SOIL.h>

//-------------------------------------------------------------------
//
// ImGUI
// 
//-------------------------------------------------------------------
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl2.h>
#include <backends/imgui_impl_win32.h>

//-------------------------------------------------------------------
//
// Current project
// 
//-------------------------------------------------------------------
#include "ImGUILayerGLFW.h"
#include "ImGUILayerWin32.h"
#include "color_translation.h"

#endif // PRECOMPILED_H