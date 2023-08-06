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

#ifndef DEPENDENCYLOADER_H
#define DEPENDENCYLOADER_H
#pragma once

#include "interface/IBaseModule.h"
#include "interface/IFileSystem.h"
#include "interface/IVariableManager.h"
#include "interface/ILibraryLoader.h"

#include "injection/IBaseInjector.h"
#include "injection/mmap/ManualMappedDllCurrentProcess.h"

#include "tier/StringTools.h"
#include "tier/GenericUtil.h"

#include <functional>

enum ELoadType
{
	LOADTYPE_UNINITIALIZED,

	LOADTYPE_NATIVE,		// native LoadLibraryEx load
	LOADTYPE_MANUALMAP,		// manual mapped module
};

// represents a loadable module managed by the dependency loader
class LoadableModuleObject
{
public:
	inline bool has_any_interfaces() const { return m_getinterfaceinstance_fn != nullptr; }

	// Just some comparisons on strings...
	inline bool operator==(LoadableModuleObject& a) { return a.m_name == m_name; }
	inline bool operator!=(LoadableModuleObject& a) { return a.m_name != m_name; }
	inline bool operator>(LoadableModuleObject& a) { return a.m_name > m_name; }
	inline bool operator<(LoadableModuleObject& a) { return a.m_name < m_name; }

	template<typename T> inline T* get_interface(const char* identifier) const
	{
		assert(has_any_interfaces());

		return reinterpret_cast<T*>(m_getinterfaceinstance_fn(identifier));
	}

	inline auto get_name() const { return m_name; }
	inline auto get_module_handle() const { return m_handle; }
	inline auto get_basemodule_interface() const { return m_basemodule_ptr; }

	std::string get_name_utf8() const;

	bool load_and_initialize(ELoadType type, const std::filesystem::path& module_path, const std::wstring& module_name);
	void unload();

	bool is_dll_loaded() const;

	bool update();

private:
	bool load_by_type();
	void unload_by_type();

	ExposeModuleFn find_exposemodule_func();
	GetInterfaceInstanceFn find_getinterfaceinstance_func();

private:
	std::filesystem::path m_filepath = {}; // path to the module
	std::wstring m_name = L"none";

	// NOTE that if the module unexpectedly unloads, this points to invalid memory!
	IBaseModule* m_basemodule_ptr = nullptr;

	HMODULE m_handle = NULL;

	ELoadType m_load_type = LOADTYPE_UNINITIALIZED;
	IInjectableModuleObject* m_injected_module = nullptr;

	GetInterfaceInstanceFn m_getinterfaceinstance_fn = nullptr;
};

// Load dlls that we need in order to function properly
class CDependencyLoader
{
public:
	DECL_BASIC_CLASS(CDependencyLoader);

public:
	bool load_and_initialize_module(ELoadType type, const std::filesystem::path& module_path, const std::wstring& module_name,
									std::function<bool(const LoadableModuleObject& mod)> extended_init_callback = nullptr);
	void unload_all();

	void unload_module(const std::wstring& module_name);

	bool update();

	inline const LoadableModuleObject* get_module(const std::wstring& module_name) const
	{
		for (auto& mod : m_loaded_modules)
		{
			if (module_name == mod.get_name())
			{
				return &mod;
			}
		}

		return nullptr;
	}

	inline void for_each(const std::function<void(const LoadableModuleObject* module, size_t i)> callback) const
	{
		size_t n = 0;
		for (auto& mod : m_loaded_modules)
		{
			callback(&mod, n++);
		}
	}

private:
	bool update_modules();

private:
	std::vector<LoadableModuleObject> m_loaded_modules;

	IBaseInjector* m_injector = nullptr;
};

#endif // DEPENDENCYLOADER_H