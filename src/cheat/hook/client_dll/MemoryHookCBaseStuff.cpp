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

bool CMemoryHookCBaseStuff::install_hooks()
{
	if (!ClientWeapons().install()) return false;

	return true;
}

//-----------------------------------------------------------------------------

bool ClientWeapons_Hook::install()
{
	initialize("ClientWeapons", L"client.dll");
	return install_using_bytepattern(1);
}

void ClientWeapons_Hook::test_hook()
{
	auto p = get();

	CHookTests::the().run_seh_protected_block(
		m_name,
		[&]()
		{
			auto wpns = *p;

			bool ok = true;
			for (int i = 1; i < MAX_WEAPONS - 1; i++)
			{
				auto wpn = wpns[i];
				if (i == hl::WEAPON_GLOCK)
				{
					ok &= (wpn == nullptr);
					continue;
				}

				if (CGameUtil::the().is_fully_connected())
				{
					ok &= (wpn != nullptr);
				}
				else
				{
					ok &= (wpn == nullptr);
				}

				if (!ok)
					break;
			}

			return ok;
		});
}

//-----------------------------------------------------------------------------