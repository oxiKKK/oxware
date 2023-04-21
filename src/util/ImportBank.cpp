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
// ImportBank.cpp -- Quietly loads up module exports without creating entries inside IAT.
//

#include "precompiled.h"

// ntdll.dll
typedef NTSTATUS(NTAPI*pfnNtOpenProcess_t)(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);
typedef NTSTATUS(NTAPI*pfnNtWriteVirtualMemory_t)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesWritten);
typedef NTSTATUS(NTAPI*pfnNtReadVirtualMemory_t)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesRead);
typedef NTSTATUS(NTAPI*pfnNtWaitForSingleObject_t)(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout);
typedef NTSTATUS(NTAPI*pfnNtClose_t)(HANDLE Handle);
typedef NTSTATUS(NTAPI*pfnNtCreateThreadEx_t)(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ProcessHandle,
											  PVOID StartRoutine, PVOID Argument, ULONG CreateFlags, SIZE_T ZeroBits, SIZE_T StackSize, SIZE_T MaximumStackSize,
											  PPS_ATTRIBUTE_LIST AttributeList);
typedef NTSTATUS(NTAPI*pfnNtTerminateThread_t)(HANDLE ThreadHandle, NTSTATUS ExitStatus);
typedef NTSTATUS(NTAPI*pfnNtProtectVirtualMemory_t)(HANDLE ProcessHandle, PVOID *BaseAddress, PSIZE_T RegionSize, ULONG NewProtect, PULONG OldProtect);
typedef NTSTATUS(NTAPI*pfnNtQueryInformationProcess_t)(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation,
													   ULONG ProcessInformationLength, PULONG ReturnLength);
typedef NTSTATUS(NTAPI*pfnNtQueryInformationThread_t)(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength,
													  PULONG ReturnLength);
typedef NTSTATUS(NTAPI*pfnRtlAllocateHeap_t)(PVOID HeapHandle, ULONG Flags, SIZE_T Size);

// kernel32.dll
typedef HMODULE(WINAPI*pfnLoadLibraryA_t)(LPCSTR lpLibFileName);
typedef HMODULE(WINAPI*pfnLoadLibraryW_t)(LPWSTR lpLibFileName);
typedef BOOL(WINAPI*pfnFreeLibrary_t)(HMODULE hLibModule);
typedef HANDLE(WINAPI*pfnCreateToolhelp32Snapshot_t)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL(WINAPI*pfnProcess32First_t)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL(WINAPI*pfnProcess32Next_t)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL(WINAPI*pfnCloseHandle_t)(HANDLE hObject);
typedef BOOL(WINAPI*pfnVirtualFreeEx_t)(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
typedef BOOL(WINAPI*pfnVirtualFree_t)(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
typedef LPVOID(WINAPI*pfnVirtualAllocEx_t)(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
typedef LPVOID(WINAPI*pfnVirtualAlloc_t)(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
typedef HANDLE(WINAPI*pfnCreateMutexA_t)(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName);
typedef BOOL(WINAPI*pfnReleaseMutex_t)(HANDLE hMutex);
typedef DWORD(WINAPI*pfnWaitForSingleObject_t)(HANDLE hHandle, DWORD dwMilliseconds);
typedef LPSTR(WINAPI*pfnGetCommandLineA_t)();
typedef BOOL(WINAPI*pfnSetCurrentDirectoryA_t)(LPCTSTR lpPathName);
typedef DWORD(WINAPI*pfnGetCurrentDirectoryA_t)(DWORD nBufferLength, LPTSTR lpBuffer);
typedef DWORD(WINAPI*pfnGetCurrentDirectoryW_t)(DWORD nBufferLength, LPWSTR lpBuffer);
typedef int (WINAPI*pfnWideCharToMultiByte_t)(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr,
											  int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar);
typedef int (WINAPI*pfnMultiByteToWideChar_t)(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr,
											  int cchWideChar);


// ws2_32.dll
typedef int(PASCAL*pfnWSAStartup_t)(WORD wVersionRequired, LPWSADATA lpWSAData);
typedef int(PASCAL*pfnWSACleanup_t)();

// advapi32.dll
typedef LSTATUS(APIENTRY*pfnRegSetValueExA_t)(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE* lpData, DWORD cbData);
typedef LSTATUS(APIENTRY*pfnRegQueryValueExA_t)(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
typedef LSTATUS(APIENTRY*pfnRegCreateKeyExA_t)(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired,
											   CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);
typedef LSTATUS(APIENTRY*pfnRegCloseKey_t)(HKEY hKey);

// Shell32.dll
typedef HRESULT (STDAPICALLTYPE*pfnSHGetKnownFolderPath_t)(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath);

// Ole32.dll
typedef HRESULT (STDAPICALLTYPE*pfnCoTaskMemFree_t)(LPVOID pv);

IImportBank* g_importbank_i = nullptr;

class CImportBank : public IImportBank
{
public:
	CImportBank();
	~CImportBank();

	bool initialize();

	// ntdll.dll
	NTSTATUS NtOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);
	NTSTATUS NtWriteVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesWritten);
	NTSTATUS NtReadVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesRead);
	NTSTATUS NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout);
	NTSTATUS NtClose(HANDLE Handle);
	NTSTATUS NtCreateThreadEx(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ProcessHandle,
							  PVOID StartRoutine, PVOID Argument, ULONG CreateFlags, SIZE_T ZeroBits, SIZE_T StackSize, SIZE_T MaximumStackSize,
							  PPS_ATTRIBUTE_LIST AttributeList);
	NTSTATUS NtTerminateThread(HANDLE ThreadHandle, NTSTATUS ExitStatus);
	NTSTATUS NtProtectVirtualMemory(HANDLE ProcessHandle, PVOID *BaseAddress, PSIZE_T RegionSize, ULONG NewProtect, PULONG OldProtect);
	NTSTATUS NtQueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation,
									   ULONG ProcessInformationLength, PULONG ReturnLength);
	NTSTATUS NtQueryInformationThread(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength,
									  PULONG ReturnLength);
	NTSTATUS RtlAllocateHeap(PVOID HeapHandle, ULONG Flags, SIZE_T Size);

	// kernel32.dll
	HMODULE LoadLibraryA(LPCSTR lpLibFileName);
	HMODULE LoadLibraryW(LPWSTR lpLibFileName);
	BOOL FreeLibrary(HMODULE hLibModule);
	HANDLE CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID);
	BOOL Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
	BOOL Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
	BOOL CloseHandle(HANDLE hObject);
	BOOL VirtualFreeEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
	BOOL VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
	LPVOID VirtualAllocEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
	LPVOID VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
	HANDLE CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName);
	BOOL ReleaseMutex(HANDLE hMutex);
	DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
	int WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData);
	int WSACleanup();
	LPSTR GetCommandLineA();
	BOOL SetCurrentDirectoryA(LPCTSTR lpPathName);
	DWORD GetCurrentDirectoryA(DWORD nBufferLength, LPTSTR lpBuffer);
	DWORD GetCurrentDirectoryW(DWORD nBufferLength, LPWSTR lpBuffer);
	int WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr,
							int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar);
	int MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr,
							int cchWideChar);

	// advapi32.dll
	LSTATUS RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE* lpData, DWORD cbData);
	LSTATUS RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
	LSTATUS RegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired,
							CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);
	LSTATUS RegCloseKey(HKEY hKey);

	// Shell32.dll
	HRESULT SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);

	// Ole32.dll
	HRESULT CoTaskMemFree(LPVOID pv);

private:
	// Modules
	HMODULE m_ntdll;
	HMODULE m_ke32;
	HMODULE m_advapi32;
	HMODULE m_ws2_32;
	HMODULE m_shell32;
	HMODULE m_ole32;

	// ntdll.dll
	pfnNtOpenProcess_t				pfnNtOpenProcess;
	pfnNtWriteVirtualMemory_t		pfnNtWriteVirtualMemory;
	pfnNtReadVirtualMemory_t		pfnNtReadVirtualMemory;
	pfnNtWaitForSingleObject_t		pfnNtWaitForSingleObject;
	pfnNtClose_t					pfnNtClose;
	pfnNtCreateThreadEx_t			pfnNtCreateThreadEx;
	pfnNtTerminateThread_t			pfnNtTerminateThread;
	pfnNtProtectVirtualMemory_t		pfnNtProtectVirtualMemory;
	pfnNtQueryInformationProcess_t	pfnNtQueryInformationProcess;
	pfnNtQueryInformationThread_t	pfnNtQueryInformationThread;
	pfnRtlAllocateHeap_t			pfnRtlAllocateHeap;

	// kernel32.dll
	pfnLoadLibraryA_t				pfnLoadLibraryA;
	pfnLoadLibraryW_t				pfnLoadLibraryW;
	pfnFreeLibrary_t				pfnFreeLibrary;
	pfnCreateToolhelp32Snapshot_t	pfnCreateToolhelp32Snapshot;
	pfnProcess32First_t				pfnProcess32First;
	pfnProcess32Next_t				pfnProcess32Next;
	pfnCloseHandle_t				pfnCloseHandle;
	pfnVirtualFreeEx_t				pfnVirtualFreeEx;
	pfnVirtualFree_t				pfnVirtualFree;
	pfnVirtualAllocEx_t				pfnVirtualAllocEx;
	pfnVirtualAlloc_t				pfnVirtualAlloc;
	pfnCreateMutexA_t				pfnCreateMutexA;
	pfnReleaseMutex_t				pfnReleaseMutex;
	pfnWaitForSingleObject_t		pfnWaitForSingleObject;
	pfnGetCommandLineA_t			pfnGetCommandLineA;
	pfnSetCurrentDirectoryA_t		pfnSetCurrentDirectoryA;
	pfnGetCurrentDirectoryA_t		pfnGetCurrentDirectoryA;
	pfnGetCurrentDirectoryW_t		pfnGetCurrentDirectoryW;
	pfnWideCharToMultiByte_t		pfnWideCharToMultiByte;
	pfnMultiByteToWideChar_t		pfnMultiByteToWideChar;

	// ws2_32.dll
	pfnWSAStartup_t					pfnWSAStartup;
	pfnWSACleanup_t					pfnWSACleanup;

	// advapi32.dll
	pfnRegSetValueExA_t				pfnRegSetValueExA;
	pfnRegQueryValueExA_t			pfnRegQueryValueExA;
	pfnRegCreateKeyExA_t			pfnRegCreateKeyExA;
	pfnRegCloseKey_t				pfnRegCloseKey;

	// Shell32.dll
	pfnSHGetKnownFolderPath_t		pfnSHGetKnownFolderPath;

	// Ole32.dll
	pfnCoTaskMemFree_t				pfnCoTaskMemFree;

public:
	template<typename T>
	static T initialize_procedure_function_pointer(HMODULE module, const char* proc_name)
	{
		uintptr_t address_to_procedure = (uintptr_t)g_libloader_i->find_proc_in_target_library((uintptr_t)module, proc_name);
		assert(address_to_procedure); // TODO: handle this via a modal window or smth
		return reinterpret_cast<T>(address_to_procedure);
	}
};

CImportBank g_importbank;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CImportBank, IImportBank, IIMPORTBANK_INTERFACEID, g_importbank);

//-------------------------------------------------------------------------------
//
// CImportBank implementation
//
//-------------------------------------------------------------------------------

template<typename T>
static auto init_proc(HMODULE module, const char* proc_name)
{
	return CImportBank::initialize_procedure_function_pointer<T>(module, proc_name);
}

static auto load_lib(const wchar_t* path_to, const wchar_t* name)
{
	auto module_handle = g_libloader_i->load_library(path_to, name);
	if (!module_handle)
	{
		CMessageBox::display_error("Couldn't load library '{}'.", CStringTools::the().unicode_to_utf8(name));
	}

	return module_handle;
}

CImportBank::CImportBank()
{
	g_importbank_i = this;
}

CImportBank::~CImportBank()
{
	g_importbank_i = nullptr;
}

bool CImportBank::initialize()
{
	static bool recursion_guard = true;
	assert(recursion_guard);

	// These two are always loaded within an executable
	m_ntdll = (HMODULE)g_libloader_i->get_target_loaded_dll_base_address(L"ntdll.dll");
	m_ke32 = (HMODULE)g_libloader_i->get_target_loaded_dll_base_address(L"kernel32.dll");

	// ntdll.dll
	pfnNtOpenProcess = init_proc<pfnNtOpenProcess_t>(m_ntdll, "NtOpenProcess");
	pfnNtWriteVirtualMemory = init_proc<pfnNtWriteVirtualMemory_t>(m_ntdll, "NtWriteVirtualMemory");
	pfnNtReadVirtualMemory = init_proc<pfnNtReadVirtualMemory_t>(m_ntdll, "NtReadVirtualMemory");
	pfnNtWaitForSingleObject = init_proc<pfnNtWaitForSingleObject_t>(m_ntdll, "NtWaitForSingleObject");
	pfnNtClose = init_proc<pfnNtClose_t>(m_ntdll, "NtClose");
	pfnNtCreateThreadEx = init_proc<pfnNtCreateThreadEx_t>(m_ntdll, "NtCreateThreadEx");
	pfnNtTerminateThread = init_proc<pfnNtTerminateThread_t>(m_ntdll, "NtTerminateThread");
	pfnNtProtectVirtualMemory = init_proc<pfnNtProtectVirtualMemory_t>(m_ntdll, "NtProtectVirtualMemory");
	pfnNtQueryInformationProcess = init_proc<pfnNtQueryInformationProcess_t>(m_ntdll, "NtQueryInformationProcess");
	pfnNtQueryInformationThread = init_proc<pfnNtQueryInformationThread_t>(m_ntdll, "NtQueryInformationThread");
	pfnRtlAllocateHeap = init_proc<pfnRtlAllocateHeap_t>(m_ntdll, "RtlAllocateHeap");

	// kernel32.dll
	pfnLoadLibraryA = init_proc<pfnLoadLibraryA_t>(m_ke32, "LoadLibraryA");
	pfnLoadLibraryW = init_proc<pfnLoadLibraryW_t>(m_ke32, "LoadLibraryW");
	pfnFreeLibrary = init_proc<pfnFreeLibrary_t>(m_ke32, "FreeLibrary");
	pfnCreateToolhelp32Snapshot = init_proc<pfnCreateToolhelp32Snapshot_t>(m_ke32, "CreateToolhelp32Snapshot");
	pfnProcess32First = init_proc<pfnProcess32First_t>(m_ke32, "Process32First");
	pfnProcess32Next = init_proc<pfnProcess32Next_t>(m_ke32, "Process32Next");
	pfnCloseHandle = init_proc<pfnCloseHandle_t>(m_ke32, "CloseHandle");
	pfnVirtualFreeEx = init_proc<pfnVirtualFreeEx_t>(m_ke32, "VirtualFreeEx");
	pfnVirtualFree = init_proc<pfnVirtualFree_t>(m_ke32, "VirtualFree");
	pfnVirtualAllocEx = init_proc<pfnVirtualAllocEx_t>(m_ke32, "VirtualAllocEx");
	pfnVirtualAlloc = init_proc<pfnVirtualAlloc_t>(m_ke32, "VirtualAlloc");
	pfnCreateMutexA = init_proc<pfnCreateMutexA_t>(m_ke32, "CreateMutexA");
	pfnReleaseMutex = init_proc<pfnReleaseMutex_t>(m_ke32, "ReleaseMutex");
	pfnWaitForSingleObject = init_proc<pfnWaitForSingleObject_t>(m_ke32, "WaitForSingleObject");
	pfnGetCommandLineA = init_proc<pfnGetCommandLineA_t>(m_ke32, "GetCommandLineA");
	pfnSetCurrentDirectoryA = init_proc<pfnSetCurrentDirectoryA_t>(m_ke32, "SetCurrentDirectoryA");
	pfnGetCurrentDirectoryW = init_proc<pfnGetCurrentDirectoryW_t>(m_ke32, "GetCurrentDirectoryW");
	pfnWideCharToMultiByte = init_proc<pfnWideCharToMultiByte_t>(m_ke32, "WideCharToMultiByte");
	pfnMultiByteToWideChar = init_proc<pfnMultiByteToWideChar_t>(m_ke32, "MultiByteToWideChar");

	auto windir = g_libloader_i->get_windows_directory("SysWOW64");
	auto windir_32bit = windir.wstring();

	// ws2_32.dll
	m_ws2_32 = load_lib(windir_32bit.c_str(), L"ws2_32.dll");
	if (!m_ws2_32)
	{
		return false;
	}
	pfnWSAStartup = init_proc<pfnWSAStartup_t>(m_ws2_32, "WSAStartup");
	pfnWSACleanup = init_proc<pfnWSACleanup_t>(m_ws2_32, "WSACleanup");

	// advapi32.dll
	m_advapi32 = load_lib(windir_32bit.c_str(), L"advapi32.dll");
	if (!m_advapi32)
	{
		return false;
	}
	pfnRegSetValueExA = init_proc<pfnRegSetValueExA_t>(m_advapi32, "RegSetValueExA");
	pfnRegQueryValueExA = init_proc<pfnRegQueryValueExA_t>(m_advapi32, "RegQueryValueExA");
	pfnRegCreateKeyExA = init_proc<pfnRegCreateKeyExA_t>(m_advapi32, "RegCreateKeyExA");
	pfnRegCloseKey = init_proc<pfnRegCloseKey_t>(m_advapi32, "RegCloseKey");

	// Shell32.dll
	m_shell32 = load_lib(windir_32bit.c_str(), L"shell32.dll");
	if (!m_shell32)
	{
		return false;
	}
	pfnSHGetKnownFolderPath = init_proc<pfnSHGetKnownFolderPath_t>(m_shell32, "SHGetKnownFolderPath");

	// Ole32.dll
	m_ole32 = load_lib(windir_32bit.c_str(), L"ole32.dll");
	if (!m_ole32)
	{
		return false;
	}
	pfnCoTaskMemFree = init_proc<pfnCoTaskMemFree_t>(m_ole32, "CoTaskMemFree");

	CConsole::the().info("Import bank initialized.");

	recursion_guard = false;
	return true;
}

//-----------------------------------------------------------------------------------------------
//
// ntdll.dll
// 
//-----------------------------------------------------------------------------------------------

NTSTATUS CImportBank::NtOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId)
{
	return pfnNtOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
}

NTSTATUS CImportBank::NtWriteVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesWritten)
{
	return pfnNtWriteVirtualMemory(ProcessHandle, BaseAddress, Buffer, BufferSize, NumberOfBytesWritten);
}

NTSTATUS CImportBank::NtReadVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesRead)
{
	return pfnNtReadVirtualMemory(ProcessHandle, BaseAddress, Buffer, BufferSize, NumberOfBytesRead);
}

NTSTATUS CImportBank::NtWaitForSingleObject(HANDLE Handle, BOOLEAN Alertable, PLARGE_INTEGER Timeout)
{
	return pfnNtWaitForSingleObject(Handle, Alertable, Timeout);
}

NTSTATUS CImportBank::NtClose(HANDLE Handle)
{
	return pfnNtClose(Handle);
}

NTSTATUS CImportBank::NtCreateThreadEx(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ProcessHandle,
									   PVOID StartRoutine, PVOID Argument, ULONG CreateFlags, SIZE_T ZeroBits, SIZE_T StackSize, SIZE_T MaximumStackSize,
									   PPS_ATTRIBUTE_LIST AttributeList)
{
	return pfnNtCreateThreadEx(ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle, StartRoutine, Argument, CreateFlags, ZeroBits, StackSize,
							   MaximumStackSize, AttributeList);
}

NTSTATUS CImportBank::NtTerminateThread(HANDLE ThreadHandle, NTSTATUS ExitStatus)
{
	return pfnNtTerminateThread(ThreadHandle, ExitStatus);
}

NTSTATUS CImportBank::NtProtectVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, PSIZE_T RegionSize, ULONG NewProtect, PULONG OldProtect)
{
	return pfnNtProtectVirtualMemory(ProcessHandle, BaseAddress, RegionSize, NewProtect, OldProtect);
}

NTSTATUS CImportBank::NtQueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation,
												ULONG ProcessInformationLength, PULONG ReturnLength)
{
	return pfnNtQueryInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}

NTSTATUS CImportBank::NtQueryInformationThread(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation,
											   ULONG ThreadInformationLength, PULONG ReturnLength)
{
	return pfnNtQueryInformationThread(ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength, ReturnLength);
}

NTSTATUS CImportBank::RtlAllocateHeap(PVOID HeapHandle, ULONG Flags, SIZE_T Size)
{
	return pfnRtlAllocateHeap(HeapHandle, Flags, Size);
}

//-----------------------------------------------------------------------------------------------
//
// kernel32.dll
// 
//-----------------------------------------------------------------------------------------------

HMODULE CImportBank::LoadLibraryA(LPCSTR lpLibFileName)
{
	return pfnLoadLibraryA(lpLibFileName);
}

HMODULE CImportBank::LoadLibraryW(LPWSTR lpLibFileName)
{
	return pfnLoadLibraryW(lpLibFileName);
}

BOOL CImportBank::FreeLibrary(HMODULE hLibModule)
{
	return pfnFreeLibrary(hLibModule);
}

HANDLE CImportBank::CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID)
{
	return pfnCreateToolhelp32Snapshot(dwFlags, th32ProcessID);
}

BOOL CImportBank::Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
	return pfnProcess32First(hSnapshot, lppe);
}

BOOL CImportBank::Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
	return pfnProcess32Next(hSnapshot, lppe);
}

BOOL CImportBank::CloseHandle(HANDLE hObject)
{
	return pfnCloseHandle(hObject);
}

BOOL CImportBank::VirtualFreeEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
	return pfnVirtualFreeEx(hProcess, lpAddress, dwSize, dwFreeType);
}

BOOL CImportBank::VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
	return pfnVirtualFree(lpAddress, dwSize, dwFreeType);
}

LPVOID CImportBank::VirtualAllocEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
	return pfnVirtualAllocEx(hProcess, lpAddress, dwSize, flAllocationType, flProtect);
}

LPVOID CImportBank::VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
	return pfnVirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
}

HANDLE CImportBank::CreateMutexA(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName)
{
	return pfnCreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
}

BOOL CImportBank::ReleaseMutex(HANDLE hMutex)
{
	return pfnReleaseMutex(hMutex);
}

DWORD CImportBank::WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds)
{
	return pfnWaitForSingleObject(hHandle, dwMilliseconds);
}

LPSTR CImportBank::GetCommandLineA()
{
	return pfnGetCommandLineA();
}

BOOL CImportBank::SetCurrentDirectoryA(LPCTSTR lpPathName)
{
	return pfnSetCurrentDirectoryA(lpPathName);
}

DWORD CImportBank::GetCurrentDirectoryA(DWORD nBufferLength, LPTSTR lpBuffer)
{
	return pfnGetCurrentDirectoryA(nBufferLength, lpBuffer);
}

DWORD CImportBank::GetCurrentDirectoryW(DWORD nBufferLength, LPWSTR lpBuffer)
{
	return pfnGetCurrentDirectoryW(nBufferLength, lpBuffer);
}

int CImportBank::WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, 
									 int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
	return pfnWideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
}

int CImportBank::MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, 
									 int cchWideChar)
{
	return pfnMultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}

//-----------------------------------------------------------------------------------------------
//
// ws2_32.dll
// 
//-----------------------------------------------------------------------------------------------

int CImportBank::WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData)
{
	return pfnWSAStartup(wVersionRequired, lpWSAData);
}

int CImportBank::WSACleanup()
{
	return pfnWSACleanup();
}

//-----------------------------------------------------------------------------------------------
//
// advapi32.dll
// 
//-----------------------------------------------------------------------------------------------

LSTATUS CImportBank::RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE* lpData, DWORD cbData)
{
	return pfnRegSetValueExA(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

LSTATUS CImportBank::RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	return pfnRegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

LSTATUS CImportBank::RegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired,
									 const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition)
{
	return pfnRegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

LSTATUS CImportBank::RegCloseKey(HKEY hKey)
{
	return pfnRegCloseKey(hKey);
}

//-----------------------------------------------------------------------------------------------
//
// Shell32.dll
// 
//-----------------------------------------------------------------------------------------------

HRESULT CImportBank::SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath)
{
	return pfnSHGetKnownFolderPath(rfid, dwFlags, hToken, ppszPath);
}

//-----------------------------------------------------------------------------------------------
//
// ole32.dll
// 
//-----------------------------------------------------------------------------------------------

HRESULT CImportBank::CoTaskMemFree(LPVOID pv)
{
	return pfnCoTaskMemFree(pv);
}
