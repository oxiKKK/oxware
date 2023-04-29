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

bool CHLNetMessageIO::install_hooks()
{
	// net message & others
	if (!net_message().install()) return false;
	if (!msg_readcount().install()) return false;
	if (!bfread().install()) return false;

	// bit reading
	if (!MSG_StartBitReading().install()) return false;
	if (!MSG_EndBitReading().install()) return false;
	if (!MSG_ReadBits().install()) return false;
	if (!MSG_ReadSBits().install()) return false;
	if (!MSG_ReadBitCoord().install()) return false;
	if (!MSG_ReadBitVec3Coord().install()) return false;

	// reading
	if (!MSG_ReadChar().install()) return false;
	if (!MSG_ReadByte().install()) return false;
	if (!MSG_ReadShort().install()) return false;
	if (!MSG_ReadWord().install()) return false;
	if (!MSG_ReadLong().install()) return false;
	if (!MSG_ReadFloat().install()) return false;
	if (!MSG_ReadString().install()) return false;
	if (!MSG_ReadCoord().install()) return false;

	// writing
	if (!MSG_WriteChar().install()) return false;
	if (!MSG_WriteByte().install()) return false;
	if (!MSG_WriteShort().install()) return false;
	if (!MSG_WriteWord().install()) return false;
	if (!MSG_WriteLong().install()) return false;
	if (!MSG_WriteFloat().install()) return false;
	if (!MSG_WriteString().install()) return false;
	if (!MSG_WriteCoord().install()) return false;

	return true;
}

//----------------------------------------------------------------------

void CHLNetMessageIO::begin_silent_bit_reading()
{
	assert(!m_in_bit_read && "already in bit-reading mode! forgot to call end_silent_bit_reading()?");
	
	save_engine_internal_bitreadstate();

	MSG_StartBitReading().call(net_message().get());

	m_in_bit_read = true;
}

void CHLNetMessageIO::end_silent_bit_reading()
{
	assert(m_in_bit_read && "not in bit-reading mode! forgot to call begin_silent_bit_reading()?");

	// no need to even call MSG_EndBitReading, because we'll be setting back the bfread buffer anyways. (+ if we did, it would break up the net.)

	restore_engine_internal_bitreadstate();

	m_in_bit_read = false;
}

uint32_t CHLNetMessageIO::read_bits(int numbits)
{	
	return MSG_ReadBits().call(numbits);
}

int32_t CHLNetMessageIO::read_signed_bits(int numbits)
{
	return MSG_ReadSBits().call(numbits);
}

float CHLNetMessageIO::read_bit_coord()
{
	return MSG_ReadBitCoord().call();
}

void CHLNetMessageIO::read_bit_vec3coord(Vector& coord)
{
	hl::vec_t v[3];
	MSG_ReadBitVec3Coord().call(v);
	coord = v;
}

//----------------------------------------------------------------------

void CHLNetMessageIO::begin_silent_reading()
{
	assert(!m_in_read && "already in reading mode! forgot to call end_silent_reading()?");

	// save engine state so we can then restore it after we're done reading.
	save_engine_internal_readcount();
	m_in_read = true;
}

void CHLNetMessageIO::end_silent_reading()
{
	assert(m_in_read && "not in reading mode! forgot to call begin_silent_reading()?");

	// restore engine reading data.
	restore_engine_internal_readcount();
	m_in_read = false;
}

void CHLNetMessageIO::end_silent_reading_no_restore()
{
	assert(m_in_read && "not in reading mode! forgot to call begin_silent_reading()?");

	// don't restore engine read data.

	m_in_read = false;
}

int8_t CHLNetMessageIO::read_int8()
{
	return MSG_ReadChar().call();
}

uint8_t CHLNetMessageIO::read_uint8()
{
	return MSG_ReadByte().call();
}

int16_t CHLNetMessageIO::read_int16()
{
	return MSG_ReadShort().call();
}

uint16_t CHLNetMessageIO::read_uint16()
{
	return MSG_ReadWord().call();
}

int32_t CHLNetMessageIO::read_int32()
{
	return MSG_ReadLong().call();
}

float CHLNetMessageIO::read_float()
{
	return MSG_ReadFloat().call();
}

char* CHLNetMessageIO::read_string()
{
	return MSG_ReadString().call();
}

float CHLNetMessageIO::read_coord()
{
	return MSG_ReadCoord().call(net_message().get());
}

//----------------------------------------------------------------------

void CHLNetMessageIO::write_char(int8_t i8)
{
	MSG_WriteChar().call(&CMemoryHookMgr::the().cls().get()->netchan.message, i8);
}

void CHLNetMessageIO::write_byte(uint8_t ui8)
{
	MSG_WriteByte().call(&CMemoryHookMgr::the().cls().get()->netchan.message, ui8);
}

void CHLNetMessageIO::write_short(int16_t i16)
{
	MSG_WriteShort().call(&CMemoryHookMgr::the().cls().get()->netchan.message, i16);
}

void CHLNetMessageIO::write_word(uint16_t ui16)
{
	MSG_WriteWord().call(&CMemoryHookMgr::the().cls().get()->netchan.message, ui16);
}

void CHLNetMessageIO::write_long(int32_t i32)
{
	MSG_WriteLong().call(&CMemoryHookMgr::the().cls().get()->netchan.message, i32);
}

void CHLNetMessageIO::write_float(float f)
{
	MSG_WriteFloat().call(&CMemoryHookMgr::the().cls().get()->netchan.message, f);
}

void CHLNetMessageIO::write_string(const char* s)
{
	MSG_WriteString().call(&CMemoryHookMgr::the().cls().get()->netchan.message, (char*)s);
}

void CHLNetMessageIO::write_coord(float coord)
{
	MSG_WriteCoord().call(&CMemoryHookMgr::the().cls().get()->netchan.message, coord);
}

//----------------------------------------------------------------------

bool MSG_StartBitReadingFnHook_t::install()
{
	initialize("MSG_StartBitReading", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\xA1\x00\x00\x00\x00\x33\xC9\x89" });
}

bool MSG_EndBitReadingFnHook_t::install()
{
	initialize("MSG_EndBitReading", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x8B\x4D\x08\xA1\x00\x00\x00\x00\x3B" });
}

bool MSG_ReadBitsFnHook_t::install()
{
	initialize("MSG_ReadBits", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\xA1\x00\x00\x00\x00\x85\xC0\x74\x00\xB8" });
}

bool MSG_ReadSBitsFnHook_t::install()
{
	initialize("MSG_ReadSBits", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x56\xE8\x00\x00\x00\x00\x8B\xF0\x8B" });
}

bool MSG_ReadBitCoordFnHook_t::install()
{
	initialize("MSG_ReadBitCoord", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x83\xEC\x10\x56\x57\xC7" });
}

bool MSG_ReadBitVec3CoordFnHook_t::install()
{
	initialize("MSG_ReadBitVec3Coord", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x53\x56\x57\xE8\x00\x00\x00\x00\x8B\xF0" });
}

//----------------------------------------------------------------------

bool MSG_ReadCharFnHook_t::install()
{
	initialize("MSG_ReadChar", L"hw.dll");
	return generic_bytepattern_installer({ "\xA1\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x8D\x48\x01\x3B\xCA\x7E\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x83\xC8\xFF\xC3\x8B" });
}

bool MSG_ReadByteFnHook_t::install()
{
	initialize("MSG_ReadByte", L"hw.dll");
	return generic_bytepattern_installer({ "\xA1\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x8D\x48\x01\x3B\xCA\x7E\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x83\xC8\xFF\xC3\x56" });
}

bool MSG_ReadShortFnHook_t::install()
{
	initialize("MSG_ReadShort", L"hw.dll");
	return generic_bytepattern_installer({ "\xA1\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x8D\x50" });
}

bool MSG_ReadWordFnHook_t::install()
{
	initialize("MSG_ReadWord", L"hw.dll");
	return generic_bytepattern_installer({ "\x8B\x00\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x57\x8D\x79\x02" });
}

bool MSG_ReadLongFnHook_t::install()
{
	initialize("MSG_ReadLong", L"hw.dll");
	return generic_bytepattern_installer({ "\x8B\x00\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x57\x8D\x79\x04" });
}

bool MSG_ReadFloatFnHook_t::install()
{
	initialize("MSG_ReadFloat", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x51\xA1\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x8A" });
}

bool MSG_ReadStringFnHook_t::install()
{
	initialize("MSG_ReadString", L"hw.dll");
	return generic_bytepattern_installer({ "\x53\x56\x33\xDB\x33\xF6\xE8\x00\x00\x00\x00\x83\xF8\xFF\x74\x00\x3B\xC3\x74\x00\x88" });
}

bool MSG_ReadCoordFnHook_t::install()
{
	initialize("MSG_ReadCoord", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x83\xEC\x08\xE8\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x85\xC9\x74\x00\xD9" });
}

//----------------------------------------------------------------------

bool MSG_WriteCharFnHook_t::install()
{
	initialize("MSG_WriteChar", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x8B\x45\x08\x6A\x01\x50\xE8\x00\x00\x00\x00\x8A\x4D\x0C\x83\xC4\x08\x88\x08\x5D\xC3\x90\x90\x90\x90\x90\x90\x90\x90\x55\x8B\xEC\x8B\x45\x08\x6A\x01" });
}

bool MSG_WriteByteFnHook_t::install()
{
	initialize("MSG_WriteByte", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x8B\x45\x08\x6A\x01\x50\xE8\x00\x00\x00\x00\x8A\x4D\x0C\x83\xC4\x08\x88\x08\x5D\xC3\x90\x90\x90\x90\x90\x90\x90\x90\x55\x8B\xEC\x8B\x45\x08\x6A\x02" });
}

bool MSG_WriteShortFnHook_t::install()
{
	initialize("MSG_WriteShort", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x8B\x45\x08\x6A\x02\x50\xE8\x00\x00\x00\x00\x8B\x4D\x0C\x83\xC4\x08\x88\x08\xC1\xF9\x08\x88\x48\x01\x5D\xC3\x90\x90\x55\x8B\xEC\x8B\x45\x08\x6A\x02" });
}

bool MSG_WriteWordFnHook_t::install()
{
	initialize("MSG_WriteWord", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x8B\x45\x08\x6A\x02\x50\xE8\x00\x00\x00\x00\x8B\x4D\x0C\x83\xC4\x08\x88\x08\xC1\xF9\x08\x88\x48\x01\x5D\xC3\x90\x90\x55\x8B\xEC\x8B\x45\x08\x6A\x04" });
}

bool MSG_WriteLongFnHook_t::install()
{
	initialize("MSG_WriteLong", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x8B\x45\x08\x6A\x04" });
}

bool MSG_WriteFloatFnHook_t::install()
{
	initialize("MSG_WriteFloat", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x8B\x45\x0C\x50\x89" });
}

bool MSG_WriteStringFnHook_t::install()
{
	initialize("MSG_WriteString", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\x56\x8B\x75\x0C\x85\xF6\x75" });
}

bool MSG_WriteCoordFnHook_t::install()
{
	initialize("MSG_WriteCoord", L"hw.dll");
	return generic_bytepattern_installer({ "\x55\x8B\xEC\xD9\x45\x0C\xD8" });
}

//----------------------------------------------------------------------

bool net_messageHook::install()
{
	initialize("net_message", L"hw.dll");
	return generic_bytepattern_installer({ "\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x04\x6A\x10", 0x1 });
}

bool msg_readcountHook::install()
{
	initialize("msg_readcount", L"hw.dll");
	return generic_bytepattern_installer({ "\x8B\x00\x00\x00\x00\x00\x50\x49", 0x2 });
}

bool bfreadHook::install()
{
	initialize("bfread", L"hw.dll");
	return generic_bytepattern_installer({ "\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x18\xC3\x55", 0x1 });
}
