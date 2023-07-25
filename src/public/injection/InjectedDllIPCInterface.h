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

#ifndef INJECTEDDLLIPCINTERFACE_H
#define INJECTEDDLLIPCINTERFACE_H
#pragma once

#include "Console.h"

#include "injection/VirtualMemoryManager.h"

enum EClient2Injector
{
	C2I_None = 0, 

	// initialization
	C2I_Init_OK,			// module initialized correctly
	C2I_Init_Failed,		// module failed to initialize

	// runtime
	C2I_Unloading,			// the client module is unloading
	C2I_Restarting,			// the client module want to restart

	C2I_Max
};

enum EInjector2Client
{
	I2C_None = 0,

	// runtime
	I2C_RequestUnload,		// injector is unloading the dll. Module is responsible for sending C2I_Unloading within short amount of time.

	I2C_Max
};

// Optional exported functions by loadable DLLs that want to setup something before constructors are invoked and
// before DllMain and CommunicativeDllEntryPoint is called.
#define PRE_DLL_LOAD_PROCNAME "PreDllLoad"
typedef bool(__cdecl*pfnPreDllLoad_t)();

// this procedure exists only within parent client modules that want to communicate with the injector.
#define COMMUNICATIVEDLLENTRYPOINT_PROCNAME "CommunicativeDllEntryPoint"
struct injector_information_package_t;
typedef bool(__cdecl*pfnCommunicativeDllEntryPoint_t)(injector_information_package_t* ifp);

// generic dll entry point. used in mainly loadlibrary as an entry point.
typedef BOOL(APIENTRY*pfnDllMain_t)(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

enum EInjectionTechnique
{
	INJECT_UNINITIALIZED,

	INJECT_LOADLIBRARY,		// This exists really only for debugging purposes, because when doing mmap, we cannot debug the cheat dll.
	INJECT_MANUALMAP,
};

static constexpr const int k_IPCInterface_ErrorMessage_MaxLength = 1024;

// communication interface between the injector and the client module
// this interface is only provided if the injected module is communicative
struct IPCInterface_t
{
	EClient2Injector m_c2m = C2I_None;
	EInjector2Client m_m2c = I2C_None;

	// increpemented by the injector each # amount of time, to keep clients alive.
	size_t m_heartbeat_sequence = 0;

	EInjectionTechnique m_technique = INJECT_UNINITIALIZED;

	// error message that is send over to the loader from the module. For example can
	// be used when the dll fails to initialize etc, so that the loader can display it
	// to the user.
	char error_message[k_IPCInterface_ErrorMessage_MaxLength];

	// put data to be shared between loader and module here
};

// held by the injector to communicate with the target dll
class InjectedDllCommunicationInterface
{
public:
	inline bool is_communicative_dll() const { return m_is_communicative; }

	inline const char* get_errorneous_message()
	{
		if (!m_ipc_block.error_message || !m_ipc_block.error_message[0])
		{
			return "";
		}

		return m_ipc_block.error_message;
	}

protected:
	void initialize_communication_interface(HANDLE target_process_handle, bool is_comunicative)
	{
		m_is_communicative = is_comunicative;
		m_target_process_handle = target_process_handle;
		m_current_process = false;
	}

	void initialize_communication_interface(bool is_comunicative)
	{
		m_is_communicative = is_comunicative;
		m_target_process_handle = CGenericUtil::the().get_current_process_handle();
		m_current_process = true;
	}

	virtual uintptr_t get_target_process_ipc_block_addr() const = 0;

	bool write_ipc_block(LPVOID where);

	// runs every frame, reads target process memory for the ipc block
	bool listen_to_injected_dll(EClient2Injector& code);
	bool tell_to(EInjector2Client code);
	bool listen_from(EClient2Injector& code);

private:
	bool send_heartbeat();
	bool read_block(IPCInterface_t* ipc);

	// checks if we're communicating to a target process or the current one.
	template<typename T>
	bool write_generic(PVOID at, T* what);
	template<typename T>
	bool read_generic(PVOID at, T* what);

private:
	IPCInterface_t m_ipc_block;

	HANDLE m_target_process_handle = NULL; // unusable when we're current process
	bool m_current_process = false;

	bool m_is_communicative = false; // true if this client module contains CommunicativeDllEntryPoint routine.
};

template<typename T>
inline bool InjectedDllCommunicationInterface::write_generic(PVOID at, T* what)
{
	if (m_current_process)
	{
		memcpy(at, what, sizeof(T));
	}
	else
	{
		if (!CVirtualMemoryManager::the().write(
			m_target_process_handle,
			at, (PVOID)what, sizeof(T)))
		{
			CConsole::the().error("Couldn't write to target process memory while trying to communicate with it.");
			return false; // sudden target process crash or mapped dll unload
		}
	}

	return true;
}

template<typename T>
inline bool InjectedDllCommunicationInterface::read_generic(PVOID at, T* what)
{
	if (m_current_process)
	{
		memcpy(what, at, sizeof(T));
	}
	else
	{
		if (!CVirtualMemoryManager::the().read<T>(
			m_target_process_handle,
			at, what))
		{
			CConsole::the().error("Couldn't read target process memory while trying to communicate with it.");
			return false; // sudden target process crash or mapped dll unload
		}
	}

	return true;
}

// maximum amount of seconds to not receive an heartbeat update, this is being checked inside the communicative dll every frame
// so we keep the communicaton between it and the injector up.
static constexpr size_t k_havent_received_hbeat_in_seconds = 5;

#define k_IPCLayerStatus_Ok					0x00000000

// injector specific
#define k_IPCLayerStatus_TimedOut			0xA0000000 // client module failed to respond after initialization
#define k_IPCLayerStatus_CannotReach		0xA0000001 // injector couldn't communicate with the client module

// client parent module specific
#define k_IPCLayerStatus_Unloading			0xF0000000 // injector requested dll unload
#define k_IPCLayerStatus_InjectorStopBeat	0xF0000001 // injector stopped sending heart beats, exit

typedef int IPCLayerStatus;

// client uses this
class CInjectedDllIPCLayerClient
{
public:
	DECL_BASIC_CLASS(CInjectedDllIPCLayerClient)

public:
	void establish_communication(IPCInterface_t* ipc);

	IPCLayerStatus dispatch();

	void write_code(EClient2Injector code);
	EInjector2Client read_code() const;

	// send error message to loader
	template <class... _Types>
	void report_error(const std::format_string<_Types...> _Fmt, _Types&&... _Args)
	{
		strcpy(m_ipc->error_message, std::vformat(_Fmt.get(), std::make_format_args(_Args...)).c_str());

		CConsole::the().error("{}", m_ipc->error_message);
	}

private:
	// true if the injector is sending heartbeats.
	bool heart_beating();

private:
	IPCInterface_t* m_ipc;
};

#endif // INJECTEDDLLIPCINTERFACE_H
