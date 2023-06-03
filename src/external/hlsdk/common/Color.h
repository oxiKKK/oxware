//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef COMMON_COLOR_H
#define COMMON_COLOR_H
#include <cstring>

#ifdef _WIN32
#pragma once
#endif

// CColor relies on int being 4 bytes long
static_assert(sizeof(int) == 4, "int size is not 4");

//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class CColor
{
public:
	// constructors
	CColor()
	{
		memset(_color, 0, 4);
	}
	CColor(int _r,int _g,int _b)
	{
		SetColor(_r, _g, _b, 0);
	}
	CColor(int _r,int _g,int _b,int _a)
	{
		SetColor(_r, _g, _b, _a);
	}
	
	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	void SetColor(int _r, int _g, int _b, int _a = 0)
	{
		_color[0] = (unsigned char)_r;
		_color[1] = (unsigned char)_g;
		_color[2] = (unsigned char)_b;
		_color[3] = (unsigned char)_a;
	}

	void GetColor(int &_r, int &_g, int &_b, int &_a) const
	{
		_r = _color[0];
		_g = _color[1];
		_b = _color[2];
		_a = _color[3];
	}

	void SetRawColor( int color32 )
	{
		memcpy(_color, &color32, 4);
	}

	int GetRawColor() const
	{
		int ret;
		memcpy(&ret, _color, 4);
		return ret;
	}

	inline int r() const	{ return _color[0]; }
	inline int g() const	{ return _color[1]; }
	inline int b() const	{ return _color[2]; }
	inline int a() const	{ return _color[3]; }
	
	unsigned char &operator[](int index)
	{
		return _color[index];
	}

	const unsigned char &operator[](int index) const
	{
		return _color[index];
	}

	bool operator == (const CColor &rhs) const
	{
		return memcmp(_color, rhs._color, 4) == 0;
	}

	bool operator != (const CColor &rhs) const
	{
		return memcmp(_color, rhs._color, 4) != 0;
	}

	CColor &operator=( const CColor &rhs )
	{
		if (this != &rhs)
			memcpy(_color, rhs._color, 4);
		return *this;
	}

private:
	unsigned char _color[4];
};


#endif // COLOR_H
