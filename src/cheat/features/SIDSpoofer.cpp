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

VarBoolean sid_spoofer_enable("sid_spoofer_enable", "Enables SID changer", false);
VarInteger sid_spoofer_type("sid_spoofer_type", "Type of the ticket generator", 0, 0, 7);

std::vector<std::string> g_sid_emulators =
{ { "revemu2013", "revemu regular", "revemu old", "avsmp", "sc2009", "setti", "smart steam emu", "steam emu" } };

std::optional<int> CSIDSpoofer::spoof(void* pAuthBlob, int cbLength)
{
	if (!sid_spoofer_enable.get_value())
	{
		return std::nullopt;
	}

	int type = sid_spoofer_type.get_value();

	CConsole::the().info("Generating SID using '{}' ticket generator.", g_sid_emulators[type]);
	switch (type)
	{
		default:
		case 0: // revemu2013
		{
			return GenerateRevEmu2013(pAuthBlob, rand() % std::numeric_limits<int32_t>::max());
		}
		case 1: // revemu regular
		{
			return GenerateRevEmu(pAuthBlob, rand() % std::numeric_limits<int32_t>::max());
		}
		case 2: // revemu old
		{
			return GenerateOldRevEmu(pAuthBlob, rand() % std::numeric_limits<int32_t>::max());
		}
		case 3: // avsmp
		{
			return GenerateAVSMP(pAuthBlob, rand() % std::numeric_limits<int32_t>::max());
		}
		case 4: // sc2009
		{
			return GenerateSC2009(pAuthBlob, rand() % std::numeric_limits<int32_t>::max());
		}
		case 5: // setti
		{
			return GenerateSetti(pAuthBlob);
		}
		case 6: // smart steam emu
		{
			return GenerateSmartSteamEmu(pAuthBlob, rand() % std::numeric_limits<int32_t>::max());
		}
		case 7: // steam emu
		{
			return GenerateSteamEmu(pAuthBlob, rand() % std::numeric_limits<int32_t>::max());
		}
	}
}
