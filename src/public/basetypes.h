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

#ifndef BASETYPES_H
#define BASETYPES_H
#pragma once

// for our hooking (from kernel32.dll)
typedef HMODULE(WINAPI*pfnLoadLibraryExW_t)(LPWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
typedef BOOL(WINAPI*pfnFreeLibrary_t)(HMODULE hLibModule);
typedef BOOL(WINAPI*pfnSetDefaultDllDirectories_t)(DWORD DirectoryFlags);

// and ntdll.dll
typedef VOID(NTAPI*pfnRtlInitUnicodeString_t)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
typedef NTSTATUS(NTAPI*pfnLdrAddDllDirectory_t)(PUNICODE_STRING NewDirectory, PDLL_DIRECTORY_COOKIE DirectoryCookie);

// session id that is provided to the injected module. This changes every time we inject.
typedef unsigned int session_id_t;

typedef unsigned char byte;

#endif // BASETYPES_H