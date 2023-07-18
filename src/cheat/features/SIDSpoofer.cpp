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

typedef struct revEmuTicket_s {
	uint32_t version;
	uint32_t highPartAuthID;
	uint32_t signature;
	uint32_t secondSignature;
	uint32_t authID;
	uint32_t thirdSignature;
	uint8_t  hash[128];
} revEmuTicket_t;

namespace RevEmu
{

uint8_t HashSymbolTable[36] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };

uint32_t Hash(const char* str)
{
	uint32_t hash = 0x4E67C6A7;

	for (const char* pch = str; *pch != '\0'; pch++)
		hash ^= (hash >> 2) + (hash << 5) + *pch;

	return hash;
}

}

VarBoolean sid_spoofer_enable("sid_spoofer_enable", "Enables SID changer", false);

std::optional<int> CSIDSpoofer::spoof(void* pAuthBlob)
{
	if (!sid_spoofer_enable.get_value())
	{
		return std::nullopt;
	}

	revEmuTicket_t revEmuTicket;

	std::srand(time(0));

	for (size_t i = 0; i < 7; i++)
		revEmuTicket.hash[i] = RevEmu::HashSymbolTable[std::rand() % sizeof(RevEmu::HashSymbolTable)];

	revEmuTicket.hash[7] = '\0';
	revEmuTicket.version = 'J';
	revEmuTicket.highPartAuthID = RevEmu::Hash((const char*)revEmuTicket.hash) & 0x7FFFFFFF;
	revEmuTicket.signature = 'rev';
	revEmuTicket.secondSignature = 0;
	revEmuTicket.authID = RevEmu::Hash((const char*)revEmuTicket.hash) << 1;
	revEmuTicket.thirdSignature = 0x01100001;
	memcpy(pAuthBlob, &revEmuTicket, sizeof(revEmuTicket));

	return std::make_optional(sizeof(revEmuTicket));
}
