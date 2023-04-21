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

#include "interface.h"

#include "interface/ILibraryLoader.h"

// This gets called whenever client module exposes an interface through the macro. Also, this is
// called when constructors are invoked, i.e. soon as the module is loaded.
CInterfaceRegistrator::CInterfaceRegistrator(InstantiateInterfaceFn fn, const char *identifier) :
	m_interface_id(identifier)
{
	m_create_fn = fn;
	m_next = s_registered_interfaces;
	s_registered_interfaces = this;
}

IBaseInterface* GetInterfaceInstance(const char *identifier)
{
	for (auto cur = CInterfaceRegistrator::s_registered_interfaces; cur; cur = cur->m_next)
	{
		if (!strcmp(cur->m_interface_id, identifier))
		{
			return cur->m_create_fn();
		}
	}

	return NULL;
}

// Modules export the 'GetInterfaceInstance' in order for other modules to use it.
GetInterfaceInstanceFn LocateExportedInterfaceGetterFn(const wchar_t* module_name)
{
	return (GetInterfaceInstanceFn)g_libloader_i->find_proc_in_target_library(module_name, INTERFACEINSTANCEGETTER_PROCNAME);
}

namespace hl
{

CreateInterfaceFn Sys_GetFactory(HMODULE hModule)
{
	if (!hModule)
	{
		return NULL;
	}

	return (CreateInterfaceFn)g_libloader_i->find_proc_in_target_library((uintptr_t)hModule, "CreateInterface");
}

} // namespace hl
