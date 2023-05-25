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

IWindowMsgHandler* g_window_msg_handler_i = nullptr;

class CWindowMsgHandler : public IWindowMsgHandler
{
public:
	CWindowMsgHandler();
	~CWindowMsgHandler();

	bool initialize(HWND wnd);
	void destroy();

	void add_msg_callback(const WndProcCallbackFn& callback);
	void add_msg_return_callback(const WndProcRetCallbackFn& callback);

	inline WNDPROC get_wnd_proc() const { return m_wnd_proc; }
	inline WndProcRetCallbackFn get_return_callback() const { return m_return_callback; }

	void run_callbacks(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	std::vector<WndProcCallbackFn> m_callbacks;

	// callback that has the ability of returning a value inside the WndProc hook.
	WndProcRetCallbackFn m_return_callback;

	HWND m_window_handle;

	WNDPROC m_wnd_proc;
	static LRESULT CALLBACK wndproc_hook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

};

CWindowMsgHandler g_window_msg_handler;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CWindowMsgHandler, IWindowMsgHandler, IWINDOWMSGHANDLER_INTERFACEID, g_window_msg_handler);

//-------------------------------------------------------------------------------
//
// CWindowMsgHandler implementation
//
//-------------------------------------------------------------------------------

CWindowMsgHandler::CWindowMsgHandler()
{
	g_window_msg_handler_i = this;
}

CWindowMsgHandler::~CWindowMsgHandler()
{
	g_window_msg_handler_i = nullptr;
}

bool CWindowMsgHandler::initialize(HWND wnd)
{
	m_window_handle = wnd;

	m_wnd_proc = (WNDPROC)SetWindowLongPtrA(m_window_handle, GWLP_WNDPROC, (LONG)wndproc_hook);
	if (!m_wnd_proc)
	{
		CConsole::the().error("Couldn't hook WndProc. (last error: {})", CGenericUtil::the().get_last_win_error());
		return false;
	}

	CConsole::the().info("Initialized Window Message Handler.");
	return true;
}

void CWindowMsgHandler::destroy()
{
	CConsole::the().info("Destroying Window Message Handler...");

	// set back
	if (m_wnd_proc)
	{
		(WNDPROC)SetWindowLongPtrA(m_window_handle, GWLP_WNDPROC, (LONG)m_wnd_proc);
		m_wnd_proc = NULL;
	}
}

void CWindowMsgHandler::add_msg_callback(const WndProcCallbackFn& callback)
{
	m_callbacks.push_back(callback);
}

void CWindowMsgHandler::add_msg_return_callback(const WndProcRetCallbackFn& callback)
{
	m_return_callback = callback;
}

void CWindowMsgHandler::run_callbacks(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (auto& c : m_callbacks)
	{
		c(uMsg, wParam, lParam);
	}
}

LRESULT CWindowMsgHandler::wndproc_hook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// run all user added external callbacks
	g_window_msg_handler.run_callbacks(uMsg, wParam, lParam);

	// return callback allows us to remotely return from this routine.
	auto ret_callback = g_window_msg_handler.get_return_callback();
	if (ret_callback != nullptr)
	{
		LRESULT return_value = NULL;
		EWndProcReturnType ret = ret_callback(hwnd, uMsg, wParam, lParam, &return_value);

		// custom retunr value, return it
		if (ret == WNDPROCRET_Custom)
		{
			return return_value;
		}

		// else, return the generic value.
	}

	return CallWindowProcA(g_window_msg_handler.get_wnd_proc(), hwnd, uMsg, wParam, lParam);
}
