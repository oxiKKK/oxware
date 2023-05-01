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
// custom_format.h -- custom types for std::format
//

#include "Color.h"
#include "vector.h"
#include "KeyValue.h"

#include <format>
#include <filesystem>

// CColor
template <>
struct std::formatter<CColor> : std::formatter<std::string> {
	auto format(const CColor& clr, std::format_context& ctx) {
		return std::formatter<string>::format(
		  std::format("[r: {:.3f} g: {:.3f} b: {:.3f} a: {:.3f}]", clr.r, clr.g, clr.b, clr.a), ctx);
	}
};

// Vector
template <>
struct std::formatter<Vector> : std::formatter<std::string> {
	auto format(const Vector& vec, std::format_context& ctx) {
		return std::formatter<string>::format(
		  std::format("[x: {:.3f} y: {:.3f} z: {:.3f}]", vec.x, vec.y, vec.z), ctx);
	}
};

// Vector2D
template <>
struct std::formatter<Vector2D> : std::formatter<std::string> {
	auto format(const Vector2D& vec, std::format_context& ctx) {
		return std::formatter<string>::format(
		  std::format("[x: {:.3f} y: {:.3f}]", vec.x, vec.y), ctx);
	}
};

// KeyValue
template <>
struct std::formatter<KeyValue> : std::formatter<std::string> {
	auto format(const KeyValue& kv, std::format_context& ctx) {
		return std::formatter<string>::format(
		  std::format("[{}: {}]", kv.key(), kv.value()), ctx);
	}
};

// std::filesystem::path
template <>
struct std::formatter<std::filesystem::path> : std::formatter<std::string> {
	auto format(const std::filesystem::path& p, std::format_context& ctx) {
		return std::formatter<string>::format(
		  std::format("{}", p.string()), ctx);
	}
};
