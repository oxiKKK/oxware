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

class CMath
{
public:
	DECL_BASIC_CLASS(CMath);

public:
	void angle_vectors(const Vector& angles, Vector* forward, Vector* right, Vector* up);
	void vector_angles(const Vector& forward, Vector& angles);

	float approach_angle(float target, float value, float speed);

	void vector_transform(const Vector& vector, const float(*transformation_matrix)[4], Vector& transformed);
	void vector_ma(const Vector& vector, float scale, const Vector& vector1, Vector& vector_out);

	template<typename T>
	inline constexpr T deg2rad(T x)
	{
		return static_cast<T>(x * (std::numbers::pi_v<T> / 180.0));
	}

	template<typename T>
	inline constexpr T rad2deg(T x)
	{
		return static_cast<T>(x * (180.0 / std::numbers::pi_v<T>));
	}

	template<typename T>
	inline constexpr T pow2(T x)
	{
		return x * x;
	}

	// turns a directional vector into a yaw value
	// that points down that vector.
	float vec2yaw(const Vector& dir);
	float angle_mod(float a);

	void normalize_angles(float* angles);
	void interp_angles(float* start, float* end, float* output, float frac);
};

#endif // MATHUTIL_H