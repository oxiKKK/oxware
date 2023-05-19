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

#ifndef MATHUTIL_H
#define MATHUTIL_H
#pragma once

#define PITCH	0
#define YAW		1
#define ROLL	2

#define DotProduct(x,y)			(x[0]*y[0]+x[1]*y[1]+x[2]*y[2])

class CMathUtil
{
public:
	DECL_BASIC_CLASS(CMathUtil);

public:
	void angle_vectors(const Vector& angles, Vector& forward, Vector& right, Vector& up);

	void vector_transform(const Vector& vector, const float(*transformation_matrix)[4], Vector& transformed);

	inline float deg2rad(float x)
	{
		return (x * (M_PI / 180.0f));
	}

	inline float rad2deg(float x)
	{
		return (x * (180.0f / M_PI));
	}

};

#endif // MATHUTIL_H