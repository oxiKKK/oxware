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

#ifndef HLNETMESSAGEIO_H
#define HLNETMESSAGEIO_H
#pragma once

//---------------------------------------------------------------------------
// bit reading

// void __cdecl MSG_StartBitReading(sizebuf_t *buf);
class MSG_StartBitReadingFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::sizebuf_t*>
{
public:
	bool install();
};

// void __cdecl MSG_EndBitReading(sizebuf_t *buf);
class MSG_EndBitReadingFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::sizebuf_t*>
{
public:
	bool install();
};

// uint32 __cdecl MSG_ReadBits(int numbits);
class MSG_ReadBitsFnHook_t final : public CGenericMemoryFnHookCdecl<hl::uint32, int>
{
public:
	bool install();
};

// int32 __cdecl MSG_ReadSBits(int numbits);
class MSG_ReadSBitsFnHook_t final : public CGenericMemoryFnHookCdecl<hl::int32, int>
{
public:
	bool install();
};

// float __cdecl MSG_ReadBitCoord();
class MSG_ReadBitCoordFnHook_t final : public CGenericMemoryFnHookCdecl<float>
{
public:
	bool install();
};

// void __cdecl MSG_ReadBitVec3Coord(vec_t* fa);
class MSG_ReadBitVec3CoordFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::vec_t*>
{
public:
	bool install();
};

//---------------------------------------------------------------------------
// reading

// int __cdecl MSG_ReadChar();
class MSG_ReadCharFnHook_t final : public CGenericMemoryFnHookCdecl<int>
{
public:
	bool install();
};

// int __cdecl MSG_ReadByte();
class MSG_ReadByteFnHook_t final : public CGenericMemoryFnHookCdecl<int>
{
public:
	bool install();
};

// int __cdecl MSG_ReadShort();
class MSG_ReadShortFnHook_t final : public CGenericMemoryFnHookCdecl<int>
{
public:
	bool install();
};

// int __cdecl MSG_ReadWord();
class MSG_ReadWordFnHook_t final : public CGenericMemoryFnHookCdecl<int>
{
public:
	bool install();
};

// int __cdecl MSG_ReadLong();
class MSG_ReadLongFnHook_t final : public CGenericMemoryFnHookCdecl<int>
{
public:
	bool install();
};

// float __cdecl MSG_ReadFloat();
class MSG_ReadFloatFnHook_t final : public CGenericMemoryFnHookCdecl<float>
{
public:
	bool install();
};

// char* __cdecl MSG_ReadString();
class MSG_ReadStringFnHook_t final : public CGenericMemoryFnHookCdecl<char*>
{
public:
	bool install();
};

// float __cdecl MSG_ReadCoord(sizebuf_t *sb);
class MSG_ReadCoordFnHook_t final : public CGenericMemoryFnHookCdecl<float, hl::sizebuf_t*>
{
public:
	bool install();
};

//---------------------------------------------------------------------------
// writing

// void __cdecl MSG_WriteChar(sizebuf_t *sb, int c);
class MSG_WriteCharFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::sizebuf_t*, int>
{
public:
	bool install();
};

// void __cdecl MSG_WriteByte(sizebuf_t *sb, int c);
class MSG_WriteByteFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::sizebuf_t*, int>
{
public:
	bool install();
};

// void __cdecl MSG_WriteShort(sizebuf_t *sb, int c);
class MSG_WriteShortFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::sizebuf_t*, int>
{
public:
	bool install();
};

// void __cdecl MSG_WriteWord(sizebuf_t *sb, int c);
class MSG_WriteWordFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::sizebuf_t*, int>
{
public:
	bool install();
};

// void __cdecl MSG_WriteLong(sizebuf_t *sb, int c);
class MSG_WriteLongFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::sizebuf_t*, int>
{
public:
	bool install();
};

// void __cdecl MSG_WriteFloat(sizebuf_t *sb, float f);
class MSG_WriteFloatFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::sizebuf_t*, float>
{
public:
	bool install();
};

// void __cdecl MSG_WriteString(sizebuf_t *sb, char* s);
class MSG_WriteStringFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::sizebuf_t*, char*>
{
public:
	bool install();
};

// void __cdecl MSG_WriteCoord(sizebuf_t *sb, float f);
class MSG_WriteCoordFnHook_t final : public CGenericMemoryFnHookCdecl<void, hl::sizebuf_t*, float>
{
public:
	bool install();
};

//---------------------------------------------------------------------------

// sizebuf_t net_message;
// obtained from CL_FlushEntityPacket()
class net_messageHook final : public CGenericMemHook<hl::sizebuf_t>
{
public:
	bool install() override;
};

// int msg_readcount;
// obtained from CL_WriteMessageHistory()
class msg_readcountHook final : public CGenericMemHook<int>
{
public:
	bool install() override;
};

// bf_read_t bfread;
// obtained from ...
class bfreadHook final : public CGenericMemHook<hl::bf_read_t>
{
public:
	bool install() override;
};

//---------------------------------------------------------------------------

class CHLNetMessageIO
{
public:
	DECL_BASIC_CLASS(CHLNetMessageIO);

public:
	bool install_hooks();

	// bit reading
	void begin_silent_bit_reading();
	void end_silent_bit_reading();
	uint32_t read_bits(int numbits);
	int32_t read_signed_bits(int numbits);
	float read_bit_coord();
	void read_bit_vec3coord(Vector& coord);

	// reading
	void begin_silent_reading();
	void end_silent_reading();
	void end_silent_reading_no_restore(); // this may be handy if we aren't 100% sure if we'll handle the reading entirely by ourself.
	int8_t read_int8();
	uint8_t read_uint8();
	int16_t read_int16();
	uint16_t read_uint16();
	int32_t read_int32();
	float read_float();
	char* read_string();
	float read_coord();

	// writing
	void write_char(int8_t i8);
	void write_byte(uint8_t ui8);
	void write_short(int16_t i16);
	void write_word(uint16_t ui16);
	void write_long(int32_t i32);
	void write_float(float f);
	void write_string(const char* s);
	void write_coord(float coord);

private:
	inline auto& net_message() { static net_messageHook hook; return hook; };
	inline auto& msg_readcount() { static msg_readcountHook hook; return hook; };
	inline auto& bfread() { static bfreadHook hook; return hook; };

	// reading
	int m_num_read_saved = 0;
	bool m_in_read = false;

	void save_engine_internal_readcount() { m_num_read_saved = *msg_readcount().get(); }
	void restore_engine_internal_readcount() { *msg_readcount().get() = m_num_read_saved; }

	// bit reading
	hl::bf_read_t m_saved_read_state = {};
	bool m_in_bit_read = false;

	void save_engine_internal_bitreadstate() 
	{
		memcpy(&m_saved_read_state, bfread().get(), sizeof(hl::bf_read_t));
	}

	void restore_engine_internal_bitreadstate()
	{
		memcpy(bfread().get(), &m_saved_read_state, sizeof(hl::bf_read_t));
	}

private:
	// bit reading
	inline auto& MSG_StartBitReading() { static MSG_StartBitReadingFnHook_t hook; return hook; }
	inline auto& MSG_EndBitReading() { static MSG_EndBitReadingFnHook_t hook; return hook; }
	inline auto& MSG_ReadBits() { static MSG_ReadBitsFnHook_t hook; return hook; }
	inline auto& MSG_ReadSBits() { static MSG_ReadSBitsFnHook_t hook; return hook; }
	inline auto& MSG_ReadBitCoord() { static MSG_ReadBitCoordFnHook_t hook; return hook; }
	inline auto& MSG_ReadBitVec3Coord() { static MSG_ReadBitVec3CoordFnHook_t hook; return hook; }

	// reading
	inline auto& MSG_ReadChar() { static MSG_ReadCharFnHook_t hook; return hook; }
	inline auto& MSG_ReadByte() { static MSG_ReadByteFnHook_t hook; return hook; }
	inline auto& MSG_ReadShort() { static MSG_ReadShortFnHook_t hook; return hook; }
	inline auto& MSG_ReadWord() { static MSG_ReadWordFnHook_t hook; return hook; }
	inline auto& MSG_ReadLong() { static MSG_ReadLongFnHook_t hook; return hook; }
	inline auto& MSG_ReadFloat() { static MSG_ReadFloatFnHook_t hook; return hook; }
	inline auto& MSG_ReadString() { static MSG_ReadStringFnHook_t hook; return hook; }
	inline auto& MSG_ReadCoord() { static MSG_ReadCoordFnHook_t hook; return hook; }

	// writing
	inline auto& MSG_WriteChar() { static MSG_WriteCharFnHook_t hook; return hook; }
	inline auto& MSG_WriteByte() { static MSG_WriteByteFnHook_t hook; return hook; }
	inline auto& MSG_WriteShort() { static MSG_WriteShortFnHook_t hook; return hook; }
	inline auto& MSG_WriteWord() { static MSG_WriteWordFnHook_t hook; return hook; }
	inline auto& MSG_WriteLong() { static MSG_WriteLongFnHook_t hook; return hook; }
	inline auto& MSG_WriteFloat() { static MSG_WriteFloatFnHook_t hook; return hook; }
	inline auto& MSG_WriteString() { static MSG_WriteStringFnHook_t hook; return hook; }
	inline auto& MSG_WriteCoord() { static MSG_WriteCoordFnHook_t hook; return hook; }
};

#endif // HLNETMESSAGEIO_H