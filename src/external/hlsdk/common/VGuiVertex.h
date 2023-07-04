//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef VGUIVERTEX_H
#define VGUIVERTEX_H
#pragma once

// In retail this struct is both declare for vgui1 & vgui2 namespaces. We solve it
// by unifying it into one form.
class VGuiVertex
{
public:
	VGuiVertex() 
	{
	}

	~VGuiVertex() 
	{
	}

	VGuiVertex(int x, int y, float u = 0, float v = 0)
	{
		Init(x, y, u, v);
	}

	void Init(int xIn, int yIn, float uIn = 0, float vIn = 0)
	{
		x = xIn;
		y = yIn;
		u = uIn;
		v = vIn;
	}

	int		x, y;
	float	u, v;
};

#endif // VGUIVERTEX_H