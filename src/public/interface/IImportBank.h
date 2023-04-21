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

#ifndef IIMPORTBANK_H
#define IIMPORTBANK_H
#pragma once

#include <Tlhelp32.h>
#include <shlobj_core.h>

// resolve collisions
#undef Process32First
#undef Process32Next

class IImportBank : public IBaseInterface
{
public:
	virtual bool initialize() = 0;

	//----------------------------------------------

	// nt.dll
	virtual NTSTATUS NtOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId) = 0;
	virtual NTSTATUS NtWriteVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesWritten) = 0;
	virtual NTSTATUS NtReadVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesRead) = 0;
	virtual NTSTATUS NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout) = 0;
	virtual NTSTATUS NtClose(HANDLE Handle) = 0;
	virtual NTSTATUS NtCreateThreadEx(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ProcessHandle,
									  PVOID StartRoutine, PVOID Argument, ULONG CreateFlags, SIZE_T ZeroBits, SIZE_T StackSize, SIZE_T MaximumStackSize,
									  PPS_ATTRIBUTE_LIST AttributeList) = 0;
	virtual NTSTATUS NtTerminateThread(HANDLE ThreadHandle, NTSTATUS ExitStatus) = 0;
	virtual NTSTATUS NtProtectVirtualMemory(HANDLE ProcessHandle, PVOID *BaseAddress, PSIZE_T RegionSize, ULONG NewProtect, PULONG OldProtect) = 0;
	virtual NTSTATUS NtQueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation,
											   ULONG ProcessInformationLength, PULONG ReturnLength) = 0;
	virtual NTSTATUS NtQueryInformationThread(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength,
											  PULONG ReturnLength) = 0;
	virtual NTSTATUS RtlAllocateHeap(PVOID HeapHandle, ULONG Flags, SIZE_T Size) = 0;

	// kernel32.dll
	virtual HMODULE LoadLibraryA(LPCSTR lpLibFileName) = 0;
	virtual HMODULE LoadLibraryW(LPWSTR lpLibFileName) = 0;
	virtual BOOL FreeLibrary(HMODULE hLibModule) = 0;
	virtual HANDLE CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID) = 0;
	virtual BOOL Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe) = 0;
	virtual BOOL Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe) = 0;
	virtual BOOL CloseHandle(HANDLE hObject) = 0;
	virtual BOOL VirtualFreeEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) = 0;
	virtual BOOL VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType) = 0;
	virtual LPVOID VirtualAllocEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) = 0;
	virtual LPVOID VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect) = 0;
	virtual HANDLE CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName) = 0;
	virtual BOOL ReleaseMutex(HANDLE hMutex) = 0;
	virtual DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) = 0;
	virtual LPSTR GetCommandLineA() = 0;
	virtual BOOL SetCurrentDirectoryA(LPCTSTR lpPathName) = 0;
	virtual DWORD GetCurrentDirectoryA(DWORD nBufferLength, LPTSTR lpBuffer) = 0;
	virtual DWORD GetCurrentDirectoryW(DWORD nBufferLength, LPWSTR lpBuffer) = 0;
	virtual int WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr,
									int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar) = 0;
	virtual int MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr,
									int cchWideChar) = 0;

	// ws2_32.dll
	virtual int WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData) = 0;
	virtual int WSACleanup() = 0;

	// advapi32.dll
	virtual LSTATUS RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE* lpData, DWORD cbData) = 0;
	virtual LSTATUS RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) = 0;
	virtual LSTATUS RegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired,
									CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition) = 0;
	virtual LSTATUS RegCloseKey(HKEY hKey) = 0;

	// Shell32.dll
	virtual HRESULT SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath) = 0;

	// Ole32.dll
	virtual HRESULT CoTaskMemFree(LPVOID pv) = 0;
};

extern IImportBank* g_importbank_i;

#define IIMPORTBANK_INTERFACEID "IImportBank"

#endif // IGUIWIDGETS_H