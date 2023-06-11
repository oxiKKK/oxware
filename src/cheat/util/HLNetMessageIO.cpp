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
	net_message().install();
	msg_readcount().install();
	bfread().install();

	// bit reading
	MSG_StartBitReading().install();
	MSG_EndBitReading().install();
	MSG_ReadBits().install();
	MSG_ReadSBits().install();
	MSG_ReadBitCoord().install();
	MSG_ReadBitVec3Coord().install();

	// bit writing
	MSG_StartBitWriting().install();
	MSG_EndBitWriting().install();
	MSG_WriteBits().install();
	MSG_WriteBitData().install();

	// reading
	MSG_ReadChar().install();
	MSG_ReadByte().install();
	MSG_ReadShort().install();
	MSG_ReadWord().install();
	MSG_ReadLong().install();
	MSG_ReadFloat().install();
	MSG_ReadString().install();
	MSG_ReadCoord().install();

	// writing
	MSG_WriteChar().install();
	MSG_WriteByte().install();
	MSG_WriteShort().install();
	MSG_WriteWord().install();
	MSG_WriteLong().install();
	MSG_WriteFloat().install();
	MSG_WriteString().install();
	MSG_WriteCoord().install();
	MSG_WriteBuf().install();

	m_initialized_hooks = true;

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

void CHLNetMessageIO::start_bit_writing(hl::sizebuf_t* sb)
{
	MSG_StartBitWriting().call(sb);
}

void CHLNetMessageIO::end_bit_writing(hl::sizebuf_t* sb)
{
	MSG_EndBitWriting().call(sb);
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

void CHLNetMessageIO::write_bits(uint32_t data, int num_bits)
{
	MSG_WriteBits().call(data, num_bits);
}

void CHLNetMessageIO::write_bit_data(void* data, int num_bits)
{
	MSG_WriteBitData().call(data, num_bits);
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
	write_char(&CMemoryHookMgr::the().cls().get()->netchan.message, i8);
}

void CHLNetMessageIO::write_byte(uint8_t ui8)
{
	write_byte(&CMemoryHookMgr::the().cls().get()->netchan.message, ui8);
}

void CHLNetMessageIO::write_short(int16_t i16)
{
	write_short(&CMemoryHookMgr::the().cls().get()->netchan.message, i16);
}

void CHLNetMessageIO::write_word(uint16_t ui16)
{
	write_word(&CMemoryHookMgr::the().cls().get()->netchan.message, ui16);
}

void CHLNetMessageIO::write_long(int32_t i32)
{
	write_long(&CMemoryHookMgr::the().cls().get()->netchan.message, i32);
}

void CHLNetMessageIO::write_float(float f)
{
	write_float(&CMemoryHookMgr::the().cls().get()->netchan.message, f);
}

void CHLNetMessageIO::write_string(const char* s)
{
	write_string(&CMemoryHookMgr::the().cls().get()->netchan.message, (char*)s);
}

void CHLNetMessageIO::write_coord(float coord)
{
	write_coord(&CMemoryHookMgr::the().cls().get()->netchan.message, coord);
}

void CHLNetMessageIO::write_buf(int iSize, void* buf)
{
	write_buf(&CMemoryHookMgr::the().cls().get()->netchan.message, iSize, buf);
}

//----------------------------------------------------------------------

void CHLNetMessageIO::write_char(hl::sizebuf_t* sb, int8_t i8)
{
	MSG_WriteChar().call(sb, i8);
}

void CHLNetMessageIO::write_byte(hl::sizebuf_t* sb, uint8_t ui8)
{
	MSG_WriteByte().call(sb, ui8);
}

void CHLNetMessageIO::write_short(hl::sizebuf_t* sb, int16_t i16)
{
	MSG_WriteShort().call(sb, i16);
}

void CHLNetMessageIO::write_word(hl::sizebuf_t* sb, uint16_t ui16)
{
	MSG_WriteWord().call(sb, ui16);
}

void CHLNetMessageIO::write_long(hl::sizebuf_t* sb, int32_t i32)
{
	MSG_WriteLong().call(sb, i32);
}

void CHLNetMessageIO::write_float(hl::sizebuf_t* sb, float f)
{
	MSG_WriteFloat().call(sb, f);
}

void CHLNetMessageIO::write_string(hl::sizebuf_t* sb, const char* s)
{
	MSG_WriteString().call(sb, (char*)s);
}

void CHLNetMessageIO::write_coord(hl::sizebuf_t* sb, float coord)
{
	MSG_WriteCoord().call(sb, coord);
}

void CHLNetMessageIO::write_buf(hl::sizebuf_t* sb, int iSize, void* buf)
{
	MSG_WriteBuf().call(sb, iSize, buf);
}

//----------------------------------------------------------------------

bool MSG_StartBitReadingFnHook_t::install()
{
	initialize("MSG_StartBitReading", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_EndBitReadingFnHook_t::install()
{
	initialize("MSG_EndBitReading", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadBitsFnHook_t::install()
{
	initialize("MSG_ReadBits", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadSBitsFnHook_t::install()
{
	initialize("MSG_ReadSBits", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadBitCoordFnHook_t::install()
{
	initialize("MSG_ReadBitCoord", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadBitVec3CoordFnHook_t::install()
{
	initialize("MSG_ReadBitVec3Coord", L"hw.dll");
	return install_using_bytepattern(0);
}

//----------------------------------------------------------------------

bool MSG_StartBitWritingFnHook_t::install()
{
	initialize("MSG_StartBitWriting", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_EndBitWritingFnHook_t::install()
{
	initialize("MSG_EndBitWriting", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_WriteBitsFnHook_t::install()
{
	initialize("MSG_WriteBits", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_WriteBitDataFnHook_t::install()
{
	initialize("MSG_WriteBitData", L"hw.dll");
	return install_using_bytepattern(0);
}

//----------------------------------------------------------------------

bool MSG_ReadCharFnHook_t::install()
{
	initialize("MSG_ReadChar", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadByteFnHook_t::install()
{
	initialize("MSG_ReadByte", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadShortFnHook_t::install()
{
	initialize("MSG_ReadShort", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadWordFnHook_t::install()
{
	initialize("MSG_ReadWord", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadLongFnHook_t::install()
{
	initialize("MSG_ReadLong", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadFloatFnHook_t::install()
{
	initialize("MSG_ReadFloat", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadStringFnHook_t::install()
{
	initialize("MSG_ReadString", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_ReadCoordFnHook_t::install()
{
	initialize("MSG_ReadCoord", L"hw.dll");
	return install_using_bytepattern(0);
}

//----------------------------------------------------------------------

bool MSG_WriteCharFnHook_t::install()
{
	initialize("MSG_WriteChar", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_WriteByteFnHook_t::install()
{
	initialize("MSG_WriteByte", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_WriteShortFnHook_t::install()
{
	initialize("MSG_WriteShort", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_WriteWordFnHook_t::install()
{
	initialize("MSG_WriteWord", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_WriteLongFnHook_t::install()
{
	initialize("MSG_WriteLong", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_WriteFloatFnHook_t::install()
{
	initialize("MSG_WriteFloat", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_WriteStringFnHook_t::install()
{
	initialize("MSG_WriteString", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_WriteCoordFnHook_t::install()
{
	initialize("MSG_WriteCoord", L"hw.dll");
	return install_using_bytepattern(0);
}

bool MSG_WriteBufFnHook_t::install()
{
	initialize("MSG_WriteBuf", L"hw.dll");
	return install_using_bytepattern(0);
}

//----------------------------------------------------------------------

bool net_messageHook::install()
{
	initialize("net_message", L"hw.dll");
	return install_using_bytepattern(1);
}

bool msg_readcountHook::install()
{
	initialize("msg_readcount", L"hw.dll");
	return install_using_bytepattern(1);
}

bool bfreadHook::install()
{
	initialize("bfread", L"hw.dll");
	return install_using_bytepattern(1);
}
