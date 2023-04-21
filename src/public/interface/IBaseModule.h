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
// IBaseModule.h -- Base class for modules. Every module must implement this class and export it via ExposeModule function.
//

#ifndef IBASEMODULE_H
#define IBASEMODULE_H
#pragma once

#include "interface.h"

#include <string>

class ILibraryLoader;
class IVariableManager;
class IDeveloperConsole;

enum EUpdateStatus
{
	UPDATE_CONTINUE, // continue execution
	UPDATE_APP_EXIT, // request application exit
};

// data forwarded by the DependencyLoader to the module that just got loaded.
struct ModuleInitializationContext
{
	ILibraryLoader*		libloader_i;
	IDeveloperConsole*	devconsole_i;
};

// This actually isn't a classic interface with an identifier, but just a pure virtual
// class that each module that will be loaded by the loader must derive from.
class IBaseModule
{
public:
	virtual bool initialize(ModuleInitializationContext* context) = 0;	// called on module load
	virtual void destroy() = 0;				// called on module unload
	virtual EUpdateStatus update() = 0;		// called every tick by DependencyLoader. 

	virtual std::string get_module_name() = 0;

	virtual void kill_module() = 0;

	// TODO: module version
};

extern IBaseModule* g_thismodule_i;

typedef bool (*ExposeModuleFn)(IBaseModule** module_interface, ModuleInitializationContext* context);

#define EXPOSEMODULE_PROCNAME "ExposeModule"

#endif // IBASEMODULE_H