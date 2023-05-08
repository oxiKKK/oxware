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
// GoldSrcMungificationAlgorithms.cpp -- reverse-engineered Mungification algorithms from the GoldSrc engine.
// 
// This encryption algorithms are used as "anti-proxy/aimbot obfuscation code", as stated in the 03 leak.
// https://github.com/BlackRa1nFR/source-2003/blob/abb4ba70b1d3ad3f706ed71ac309e5b7e85f6103/src_main/engine/common.cpp#L1811
// The code was taken from there. Although there are two more mungification functions, they are all the same and
// the only difference is that they use different mungification table.
// 

#include "precompiled.h"

static const unsigned char mungify_table[] =
{
	0x7A, 0x64, 0x05, 0xF1,
	0x1B, 0x9B, 0xA0, 0xB5,
	0xCA, 0xED, 0x61, 0x0D,
	0x4A, 0xDF, 0x8E, 0xC7
};

static const unsigned char mungify_table2[] =
{
	0x05, 0x61, 0x7A, 0xED,
	0x1B, 0xCA, 0x0D, 0x9B,
	0x4A, 0xF1, 0x64, 0xC7,
	0xB5, 0x8E, 0xDF, 0xA0
};

static const unsigned char mungify_table3[] =
{
	0x20, 0x07, 0x13, 0x61,
	0x03, 0x45, 0x17, 0x72,
	0x0A, 0x2D, 0x48, 0x0C,
	0x4A, 0x12, 0xA9, 0xB5
};

int byteswap_long(int l)
{
	byte b1, b2, b3, b4;

	b1 = l & 255;
	b2 = (l >> 8) & 255;
	b3 = (l >> 16) & 255;
	b4 = (l >> 24) & 255;

	return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

void GoldSrcMungificationAlgorithms::enmungify(unsigned char* data, int length, int seq)
{
	enmungify_internal(mungify_table, data, length, seq);
}

void GoldSrcMungificationAlgorithms::enmungify2(unsigned char* data, int length, int seq)
{
	enmungify_internal(mungify_table2, data, length, seq);
}

void GoldSrcMungificationAlgorithms::enmungify3(unsigned char* data, int length, int seq)
{
	enmungify_internal(mungify_table3, data, length, seq);
}

void GoldSrcMungificationAlgorithms::demungify(unsigned char * data, int length, int seq)
{
	demungify_internal(mungify_table, data, length, seq);
}

void GoldSrcMungificationAlgorithms::demungify2(unsigned char * data, int length, int seq)
{
	demungify_internal(mungify_table2, data, length, seq);
}

void GoldSrcMungificationAlgorithms::demungify3(unsigned char * data, int length, int seq)
{
	demungify_internal(mungify_table3, data, length, seq);
}

//---------------------------------------------------------------------------------------------------------------------------------

void GoldSrcMungificationAlgorithms::enmungify_internal(const unsigned char* table, unsigned char* data, int length, int seq)
{
	int		i;
	int		mungelen;
	int		c;
	int*	pc;
	byte*	p;
	int		j;

	mungelen = length & ~3;
	mungelen /= 4;

	for (i = 0; i < mungelen; i++)
	{
		pc = (int*)&data[i * 4];
		c = *pc;
		c ^= ~seq;
		c = byteswap_long(c);

		p = (byte*)&c;

		for (j = 0; j < 4; j++)
			*p++ ^= (0xa5 | (j << j) | j | table[(i + j) & 0x0f]);

		c ^= seq;
		*pc = c;
	}
}

void GoldSrcMungificationAlgorithms::demungify_internal(const unsigned char* table, unsigned char* data, int length, int seq)
{
	int		i;
	int		mungelen;
	int		c;
	int*	pc;
	byte*	p;
	int		j;

	mungelen = length & ~3;
	mungelen /= 4;

	for (i = 0; i < mungelen; i++)
	{
		pc = (int*)&data[i * 4];
		c = *pc;
		c ^= seq;

		p = (byte*)&c;
		for (j = 0; j < 4; j++)
			*p++ ^= (0xa5 | (j << j) | j | table[(i + j) & 0x0f]);

		c = byteswap_long(c);
		c ^= ~seq;
		*pc = c;
	}
}
