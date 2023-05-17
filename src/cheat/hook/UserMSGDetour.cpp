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

bool CUserMSGDetourMgr::install_hooks()
{
	if (!HideWeapon_fn().install_usermsg(HideWeapon_f, "HideWeapon")) return false;
	if (!MOTD_fn().install_usermsg(MOTD_f, "MOTD")) return false;

	return true;
}

void CUserMSGDetourMgr::uninstall_hooks()
{
	// reset to original state.
	int reset_value = 0;
	if (CUserMSGDetourMgr::the().HideWeapon_fn().is_installed())
	{
		CUserMSGDetourMgr::the().HideWeapon_fn().call_usermsg(sizeof(byte), &reset_value);
	}

	HideWeapon_fn().uninstall();
	MOTD_fn().uninstall();
}

//---------------------------------------------------------------------------------

int CUserMSGDetourMgr::HideWeapon_f(const char* pszName, int iSize, void* pbuf)
{
	return CUserMSGDetourMgr::the().HideWeapon_fn().call_usermsg(iSize, pbuf);
}

int CUserMSGDetourMgr::MOTD_f(const char* pszName, int iSize, void* pbuf)
{
	if (CRemovals::the().remove_motd())
	{
		return 0;
	}

	return CUserMSGDetourMgr::the().MOTD_fn().call_usermsg(iSize, pbuf);
}
