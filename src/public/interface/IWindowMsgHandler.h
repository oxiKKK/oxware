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
// IWindowMsgHandler.h -- Takes care over the WNDPROC windows message handler.
//

#ifndef IWINDOWMESSAGEHANDLER_H
#define IWINDOWMESSAGEHANDLER_H
#pragma once

#include <functional>

enum EWndProcReturnType
{
	WNDPROCRET_Custom,		// custom return value
	WNDPROCRET_Generic,		// call CallWindowProcA
};

typedef std::function<void(UINT uMsg, WPARAM wParam, LPARAM lParam)> WndProcCallbackFn;
typedef std::function<EWndProcReturnType(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* lpReturnValue)> WndProcRetCallbackFn;

class IWindowMsgHandler : public IBaseInterface
{
public:
	virtual bool initialize(HWND wnd) = 0;
	virtual void destroy() = 0;

	virtual void add_msg_callback(const WndProcCallbackFn& callback) = 0;
	virtual void add_msg_return_callback(const WndProcRetCallbackFn& callback) = 0;
};

extern IWindowMsgHandler* g_window_msg_handler_i;

#define IWINDOWMSGHANDLER_INTERFACEID "IWindowMsgHandler"

#endif // IWINDOWMESSAGEHANDLER_H