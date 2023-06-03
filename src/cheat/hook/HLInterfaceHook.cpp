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

bool CHLInterfaceHook::install_hooks()
{
	// install individual hooks
	if (!install_single_hook(&m_ISurface, L"hw.dll", VGUI_SURFACE_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IPanel, L"vgui2.dll", VGUI_PANEL_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_ISchemeManager, L"vgui2.dll", VGUI_SCHEME_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IEngineVGui, L"hw.dll", ENGINEVGUI_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IGameUI, L"GameUI.dll", GAMEUI_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IBaseUI, L"hw.dll", BASEUI_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IClientVGUI, L"client.dll", CLIENTVGUI_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IFileSystem, CoXWARE::the().get_engine_fs_module_name(), FILESYSTEM_INTERFACE_VERSION)) return false;

	return true;
}
