//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef IENGINEAPI_H
#define IENGINEAPI_H

#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: The engine run result data. The engine (hw.dll/sw.dll) returns this, 
//			so we can determine changing video mode is failed or we want to quit
//-----------------------------------------------------------------------------
enum EngineRunResult
{
	ENGINE_RUN_QUITTING = 0,
	ENGINE_RUN_CHANGED_VIDEOMODE = 1,
	ENGINE_RUN_UNSUPPORTED_VIDEOMODE = 2
};

//-----------------------------------------------------------------------------
// Purpose: Engine/launcher interface
//-----------------------------------------------------------------------------
class IEngineAPI : public IBaseInterface
{
public:
	virtual EngineRunResult Run(void* instance, char* basedir, char* cmdline, char* postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory) = 0;
};

#define VENGINE_LAUNCHER_API_VERSION "VENGINE_LAUNCHER_API_VERSION002"

#endif // IENGINEAPI_H