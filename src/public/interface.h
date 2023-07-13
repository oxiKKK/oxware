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

#ifndef INTERFACE_H
#define INTERFACE_H
#pragma once

#include "platform.h"

// All interfaces derive from this.
class IBaseInterface
{
public:
	virtual ~IBaseInterface() {}
};

#define INTERFACEINSTANCEGETTER_PROCNAME "GetInterfaceInstance"

typedef IBaseInterface* (*GetInterfaceInstanceFn)(const char *identifier);
typedef IBaseInterface* (*InstantiateInterfaceFn)();

// Used internally to register classes.
class CInterfaceRegistrator
{
public:
	CInterfaceRegistrator(InstantiateInterfaceFn fn, const char *identifier);

public:
	InstantiateInterfaceFn	m_create_fn;
	const char				*m_interface_id; // aka version or name

	CInterfaceRegistrator*				 m_next;
	inline static CInterfaceRegistrator* s_registered_interfaces;
};

// Use this to expose a singleton interface with a global variable you've created.
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, interfaceID, globalVarName) \
	static CInterfaceRegistrator __g_exposed_interface_##className##interfaceName##([]() { return (IBaseInterface *)&globalVarName; }, interfaceID);

// Use this to expose a singleton interface. This creates the global variable for you automatically.
#define EXPOSE_SINGLE_INTERFACE(className, interfaceName, interfaceID) \
	static className __g_##className##_singleton;\
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, interfaceID, __g_##className##_singleton)

// expose interface late (lazy-initialize)
#define LAZY_EXPOSE_SINGLE_INTERFACE(className, interfaceName, interfaceID) \
	void lazy_expose_single_interface_##className() \
	{ \
		EXPOSE_SINGLE_INTERFACE(className, interfaceName, interfaceID); \
	}

// use this for declaration
#define LAZY_EXPOSED_INTERFACE_DECL(className) \
	extern void lazy_expose_single_interface_##className();

// use this to instantiate the interface at runtime
#define CALL_LAZY_EXPOSED_INTERFACE(className) \
	LAZY_EXPOSED_INTERFACE_DECL(className) \
	lazy_expose_single_interface_##className();

// Used to retreive interface pointers by client modules
extern "C"
{
	DLLEXPORT IBaseInterface* GetInterfaceInstance(const char* identifier);
}

GetInterfaceInstanceFn LocateExportedInterfaceGetterFn(const wchar_t* module_name);

// Locates exported interface inside the module
template <typename T>
T* LocateExportedInterface(const wchar_t* module_name, const char* identifier)
{
	auto GetInterfaceInstanceFn = LocateExportedInterfaceGetterFn(module_name);
	if (!GetInterfaceInstanceFn)
		return nullptr;

	return reinterpret_cast<T*>(GetInterfaceInstanceFn(identifier));
}

// Valve interface system
namespace hl
{

typedef IBaseInterface* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

CreateInterfaceFn Sys_GetFactory(HMODULE hModule);

} // namespace hl

#endif // INTERFACE_H