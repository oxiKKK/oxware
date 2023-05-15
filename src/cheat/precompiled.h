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

#ifndef PRECOMPILED_H
#define PRECOMPILED_H
#pragma once

//-------------------------------------------------------------------
//
// Public headers
// 
//-------------------------------------------------------------------
#include "platform.h"

#include <interface/ILibraryLoader.h>
#include <interface/IDeveloperConsole.h>
#include <interface/IBaseModule.h>
#include <interface/IImportBank.h>
#include <interface/IRegistry.h>
#include <interface/IUserInput.h>
#include <interface/IWindowMsgHandler.h>
#include <interface/IAppDataManager.h>
#include <interface/IConfigManager.h>
#include <interface/ICodePerformaceProfiler.h>
#include <interface/IBytePatternBank.h>

#include <interface/gui/IGLFWApp.h>
#include <interface/gui/IGUIWidgets.h>
#include <interface/gui/IGUIFontManager.h>
#include <interface/gui/IGUIThemeManager.h>
#include <interface/gui/IImGUIPlatformLayer.h>
#include <interface/gui/IGUIWindowRendering.h>

#include <BytePattern.h>

#include <tier/StringTools.h>
#include <tier/GenericUtil.h>
#include <tier/MessageBox.h>

#include <injection/InjectedDllIPCInterface.h>

#include <injection/IBaseInjector.h>
#include <injection/BaseInjector.h>

#include <DependencyLoader.h>

//-------------------------------------------------------------------
//
// Shared
// 
//-------------------------------------------------------------------
#include "Console.h"
#include "BaseVariable.h"

//-------------------------------------------------------------------
//
// detours
// 
//-------------------------------------------------------------------
#include <detours/detours.h>

//-------------------------------------------------------------------
//
// C++ Standard
// 
//-------------------------------------------------------------------
#include <functional>
#include <unordered_map>
#include <deque>
#include <numbers>
#include <thread>

//-------------------------------------------------------------------
//
// GLFW
// 
//-------------------------------------------------------------------
#include <GLFW/glfw3.h>

//-------------------------------------------------------------------
//
// JSON
// 
//-------------------------------------------------------------------
#include <nlohmann/json.hpp>

//-------------------------------------------------------------------
//
// OpenGL
// 
//-------------------------------------------------------------------
#include <gl/GL.h>

//-------------------------------------------------------------------
//
// HLSDK
// 
//-------------------------------------------------------------------
namespace hl
{
// utils
#include <hlsdk/tier0/basetypes.h>

// common
#include <hlsdk/common/goldsrclimits.h>
#include <hlsdk/common/protocol.h>
#include <hlsdk/common/VGuiVertex.h>
#include <hlsdk/common/filesystemdefs.h>

// engine
#include <hlsdk/engine/engine_math.h>
#include <hlsdk/engine/wrect.h>
#include <hlsdk/engine/const.h>
#include <hlsdk/engine/host.h>
#include <hlsdk/engine/con_nprint.h>
#include <hlsdk/engine/usercmd.h>
#include <hlsdk/engine/cvardef.h>
#include <hlsdk/engine/ref_params.h>
#include <hlsdk/engine/kbutton.h>
#include <hlsdk/engine/zone_cache.h>
#include <hlsdk/engine/models.h>
#include <hlsdk/engine/cl_entity.h>
#include <hlsdk/engine/net.h>
#include <hlsdk/engine/entity_state.h>
#include <hlsdk/engine/consistency.h>
#include <hlsdk/engine/delta_packet.h>
#include <hlsdk/engine/player_info.h>
#include <hlsdk/engine/r_efx.h>
#include <hlsdk/engine/studio.h>
#include <hlsdk/engine/usermsg.h>
#include <hlsdk/engine/events.h>
#include <hlsdk/engine/screenfade.h>
#include <hlsdk/engine/command.h>
#include <hlsdk/engine/sound.h>
#include <hlsdk/engine/APIProxy.h>
#include <hlsdk/engine/progdefs.h>
#include <hlsdk/engine/edict.h>
#include <hlsdk/engine/userid.h>
#include <hlsdk/engine/client.h>
#include <hlsdk/engine/server.h>
#include <hlsdk/engine/deltadef.h>
#include <hlsdk/engine/eiface.h>
#include <hlsdk/engine/triangleapi.h>
#include <hlsdk/engine/netbuf.h>
#include <hlsdk/engine/sys_engine.h>
#include <hlsdk/engine/pmtrace.h>

// particleman
#include <hlsdk/interface/ParticleMan/IParticleMan.h>
#include <hlsdk/particleman/pman_frustum.h>
#include <hlsdk/particleman/pman_main.h>
#include <hlsdk/particleman/pman_particlemem.h>
#include <hlsdk/particleman/pman_triangleffect.h>

// misc
#include <hlsdk/cstrike_tri.h>

// vgui2
#include <hlsdk/vgui2/VGUI2.h>
#include <hlsdk/vgui2/KeyCode.h>
#include <hlsdk/vgui2/MouseCode.h>
#include <hlsdk/vgui2/Cursor.h>

// playermove
#include <hlsdk/playermove/pm_movevars.h>
#include <hlsdk/playermove/pm_defs.h>

// interfaces
#include <hlsdk/interface/vgui2/IHTML.h>
#include <hlsdk/interface/vgui2/IPanel.h>
#include <hlsdk/interface/appframework/IAppSystem.h>
#include <hlsdk/interface/ISurface.h>
#include <hlsdk/interface/GameUI/IGameUI.h>
#include <hlsdk/interface/GameUI/IBaseUI.h>
#include <hlsdk/interface/GameUI/IClientVGUI.h>
#include <hlsdk/interface/IEngineVGui.h>
#include <hlsdk/interface/IFileSystem.h>

// game_shared
#include <hlsdk/game_shared/shareddefs.h>
#include <hlsdk/game_shared/cstrike/cs_shareddefs.h>
#include <hlsdk/game_shared/cstrike/weapondefs.h>

// cstrike
#include <hlsdk/dlls/enginecallback.h>
#include <hlsdk/dlls/cstrike/cbase.h>
#include <hlsdk/dlls/cstrike/weapons.h>
} // namespace hl

// include after hlsdk.
#include "custom_format_cheat.h"

//-------------------------------------------------------------------
//
// Current project
// 
//-------------------------------------------------------------------
#include "oxware.h"

// hooks
#include "hook/HookTests.h"
#include "hook/MemoryHook.h"
#include "hook/MemoryFnHook.h"
#include "hook/MemoryFnDetour.h"
#include "hook/SVCFuncDetour.h"
#include "hook/UserMSGDetour.h"
#include "hook/HLInterfaceHook.h"
#include "hook/client_dll/MemoryHookCBaseStuff.h"

// util
#include "GameUtil.h"
#include "MathUtil.h"
#include "HLNetMessageIO.h"
#include "build_num.h"
#include "EngineInputManager.h"
#include "GoldSrcMungificationAlgorigthms.h"
#include "EngineRendering.h"

// game
#include "game/model/StudioModelData.h"
#include "game/model/SpriteModelData.h"
#include "game/GenericMdl.h"
#include "game/GenericEnt.h"
#include "game/GenericPlayer.h"
#include "game/EntityManager.h"
#include "game/SpriteManager.h"

// features
#include "features/FeatureManager.h"
#include "features/FieldOfViewChanger.h"
#include "features/BypassGameConstrains.h"
#include "features/FlashbangFadeModifier.h"
#include "features/ModelChams.h"
#include "features/Removals.h"
#include "features/ViewmodelOffset.h"
#include "features/ServerLiar.h"
#include "features/SmokeVisuals.h"
#include "features/BulletTrace.h"
#include "features/FrameSkipper.h"
#include "features/ESP.h"
#include "features/VanillaCrosshair.h"
#include "features/StuffCmdFilter.h"
#include "features/ResourceConsistencyBypass.h"
#include "features/IngameScreenRendering.h"

// ui
#include "ui/IRendererContext.h"
#include "ui/UIMenuWidgets.h"
#include "ui/UIMenu.h"
#include "ui/UIMenuBackground.h"
#include "ui/UIFeatureList.h"
#include "ui/UIConsole.h"
#include "ui/UIBackgroundRendering.h"
#include "ui/UIPerformanceProfilerVisualization.h"
#include "ui/OxWareUI.h"

#endif // PRECOMPILED_H