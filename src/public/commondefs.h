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

#ifndef COMMONDEFS_H
#define COMMONDEFS_H
#pragma once

#include "platform.h"

// module names
#define MODULE_ASSETS			"assets.lib"
#define MODULE_INJECTOR			"injector.lib"
#define MODULE_LOADER			"loader.exe"
#define MODULE_CHEAT			"cheater.dll"
#define MODULE_UTIL				"util.dll"
#define MODULE_TIER				"tier.lib"
#define MODULE_GUI				"oxui.dll"
// wstring version
#define WMODULE_ASSETS			L"assets.lib"
#define WMODULE_INJECTOR		L"injector.lib"
#define WMODULE_LOADER			L"loader.exe"
#define WMODULE_CHEAT			L"cheater.dll"
#define WMODULE_UTIL			L"util.dll"
#define WMODULE_TIER			L"tier.lib"
#define WMODULE_GUI				L"oxui.dll"

#if defined(M_ASSETS)
#	define CURRENT_MODULE		MODULE_ASSETS
#elif defined(M_INJECTOR)
#	define CURRENT_MODULE		MODULE_INJECTOR
#elif defined(M_LOADER)
#	define CURRENT_MODULE		MODULE_LOADER
#elif defined(M_CHEATER)	
#	define CURRENT_MODULE		MODULE_CHEAT
#elif defined(M_UTIL)
#	define CURRENT_MODULE		MODULE_UTIL
#elif defined(M_TIER)
#	define CURRENT_MODULE		MODULE_TIER
#elif defined(M_OXUI)
#	define CURRENT_MODULE		MODULE_GUI
#else
#	define CURRENT_MODULE		"unknown"
#endif

#define GET_CURRENT_TEB()				reinterpret_cast<PTEB>(__readfsdword(0x18))
#define GET_CURRENT_PROCESS_HANDLE()	(HANDLE)-1
#define GET_CURRENT_THREAD_HANDLE()		(HANDLE)-2

// Disables stuff such as security cookies and such. This is needed for example for the shellcode routine.
// for more information see this:
// https://learn.microsoft.com/en-us/cpp/cpp/safebuffers?view=msvc-170&viewFallbackFrom=vs-2017
// and https://stackoverflow.com/questions/19945479/what-is-security-cookie-in-c
#define DISABLE_SAFEBUFFERS __declspec( safebuffers )

// bit helper
template<typename T>
consteval T BIT(T N) { return N == -1 ? 0 : (1 << N); }

// decl singleton for a class instance
#define DECL_BASIC_CLASS(name)			\
	static __forceinline name& the()	\
	{									\
		static name g_##name;			\
		return g_##name;				\
	}

#ifndef NULL
#define NULL 0
#endif

// this allows enumerations to be used as flags, and still remain type-safe!
#define DEFINE_ENUM_BITWISE_OPERATORS( Type ) \
	inline Type  operator|  ( Type  a, Type b ) { return Type( int( a ) | int( b ) ); } \
	inline Type  operator&  ( Type  a, Type b ) { return Type( int( a ) & int( b ) ); } \
	inline Type  operator^  ( Type  a, Type b ) { return Type( int( a ) ^ int( b ) ); } \
	inline Type  operator<< ( Type  a, int  b ) { return Type( int( a ) << b ); } \
	inline Type  operator>> ( Type  a, int  b ) { return Type( int( a ) >> b ); } \
	inline Type &operator|= ( Type &a, Type b ) { return a = a |  b; } \
	inline Type &operator&= ( Type &a, Type b ) { return a = a &  b; } \
	inline Type &operator^= ( Type &a, Type b ) { return a = a ^  b; } \
	inline Type &operator<<=( Type &a, int  b ) { return a = a << b; } \
	inline Type &operator>>=( Type &a, int  b ) { return a = a >> b; } \
	inline Type  operator~( Type a ) { return Type( ~int( a ) ); }

// defines increment/decrement operators for enums for easy iteration
#define DEFINE_ENUM_INCREMENT_OPERATORS( Type ) \
	inline Type &operator++( Type &a      ) { return a = Type( int( a ) + 1 ); } \
	inline Type &operator--( Type &a      ) { return a = Type( int( a ) - 1 ); } \
	inline Type  operator++( Type &a, int ) { Type t = a; ++a; return t; } \
	inline Type  operator--( Type &a, int ) { Type t = a; --a; return t; }

#define OX_ARRAYSIZE(x)    (sizeof(x)/sizeof(x[0]))

// the first engine build in which sdl was introduced instead of winapi.
#define FIRST_SDL_BUILD 5943

// first build (that i could find) where this fix got proposed
// https://github.com/ValveSoftware/halflife/issues/1940#issuecomment-13052903
#define FIRST_CLMOVE_DPS_DEPENDENCE_BUILD 6100 // something around ~6100

//
// create a custom std::hash<> for your custom type
//

inline void hash_combine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	hash_combine(seed, rest...);
}

#define MAKE_HASHABLE(type, ...) \
    namespace std {\
        template<> struct hash<type> {\
            std::size_t operator()(const type &t) const {\
                std::size_t ret = 0;\
                hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }

#endif // COMMONDEFS_H