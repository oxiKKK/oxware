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

void CMath::angle_vectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[YAW] * (float)(std::numbers::pi * 2 / 360.0f);
	sy = sinf(angle);
	cy = cosf(angle);
	angle = angles[PITCH] * (float)(std::numbers::pi * 2 / 360.0f);
	sp = sinf(angle);
	cp = cosf(angle);
	angle = angles[ROLL] * (float)(std::numbers::pi * 2 / 360.0f);
	sr = sinf(angle);
	cr = cosf(angle);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}
	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void CMath::vector_angles(const Vector& forward, Vector& angles)
{
	float	tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

float CMath::approach_angle(float target, float value, float speed)
{
	target = angle_mod(target);
	value = angle_mod(target);

	float delta = target - value;

	// Speed is assumed to be positive
	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;///
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

void CMath::vector_transform(const Vector& vector, const float(*transformation_matrix)[4], Vector& transformed)
{
	transformed[0] = DotProduct(vector, transformation_matrix[0]) + transformation_matrix[0][3];
	transformed[1] = DotProduct(vector, transformation_matrix[1]) + transformation_matrix[1][3];
	transformed[2] = DotProduct(vector, transformation_matrix[2]) + transformation_matrix[2][3];
}

void CMath::vector_ma(const Vector& vector, float scale, const Vector& vector1, Vector& vector_out)
{
	vector_out = vector + scale * vector1;
}

float CMath::vec2yaw(const Vector& dir)
{
	if (dir.IsZero2D())
	{
		return 0.0f;
	}

	float flIdealYaw = atan2(dir.y, dir.x) * 180.0f / M_PI;
	if (flIdealYaw < 0.0f)
	{
		flIdealYaw += 360.0f; // normalize
	}
	return flIdealYaw;
}

float CMath::angle_mod(float a)
{
	if (a < 0)
	{
		a = a + 360 * ((int)(a / 360) + 1);
	}
	else if (a >= 360)
	{
		a = a - 360 * ((int)(a / 360));
	}
	// a = (360.0/65536) * ((int)(a*(65536/360.0)) & 65535);
	return a;
}

void CMath::normalize_angles(float * angles)
{
	for (int i = 0; i < 3; i++)
	{
		if (angles[i] > 180.f)
			angles[i] -= 360.f;
		else if (angles[i] < -180.f)
			angles[i] += 360.f;
	}
}

// Interpolate Euler angles.
// FIXME:  Use Quaternions to avoid discontinuities
// Frac is 0.0 to 1.0 ( i.e., should probably be clamped, but doesn't have to be )
void CMath::interp_angles(float* start, float* end, float* output, float frac)
{
	float ang1, ang2;
	float d;

	normalize_angles(start);
	normalize_angles(end);

	for (int i = 0; i < 3; i++)
	{
		ang1 = start[i];
		ang2 = end[i];

		d = ang2 - ang1;
		if (d > 180.0f)
		{
			d -= 360.0f;
		}
		else if (d < -180.0f)
		{
			d += 360.0f;
		}

		output[i] = ang1 + d * frac;
	}

	normalize_angles(output);
}
