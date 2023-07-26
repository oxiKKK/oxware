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

#include <interface/IBaseModule.h>
#include <interface/IRegistry.h>
#include <interface/IUserInput.h>
#include <interface/IWindowMsgHandler.h>
#include <interface/IAppDataManager.h>
#include <interface/IConfigManager.h>
#include <interface/ICodePerformaceProfiler.h>
#include <interface/IBytePatternBank.h>
#include <interface/IInCommands.h>
#include <interface/ICheatInfo.h>
#include <interface/ITextureManager.h>

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

#include <gsdecrypt/gsdecrypt.h>

#include <gui/UIStatusWidget.h>

#include <assets/textures.h>

#include <multiemulator/multiemulator.h>

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
// SOIL
// 
//-------------------------------------------------------------------
#include <soil/soil.h>

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
#include <set>
#include <future>

//-------------------------------------------------------------------
//
// GLFW
// 
//-------------------------------------------------------------------
#include <GLFW/glfw3.h>

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
#include <hlsdk/common/Color.h>

// engine
#include <hlsdk/engine/keydefs.h>
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
#include <hlsdk/interface/vgui2/IScheme.h>
#include <hlsdk/interface/appframework/IAppSystem.h>
#include <hlsdk/interface/ISurface.h>
#include <hlsdk/interface/GameUI/IGameUI.h>
#include <hlsdk/interface/GameUI/IBaseUI.h>
#include <hlsdk/interface/GameUI/IClientVGUI.h>
#include <hlsdk/interface/IEngineVGui.h>
#include <hlsdk/interface/IFileSystem.h>
#include <hlsdk/interface/IVideoMode.h>

// steam
#include <hlsdk/steam/isteamfriends.h>
#include <hlsdk/steam/isteamutils.h>
#include <hlsdk/steam/isteamuser.h>

// game_shared
#include <hlsdk/game_shared/shareddefs.h>
#include <hlsdk/game_shared/cstrike/cs_shareddefs.h>

// cstrike
#include <hlsdk/dlls/enginecallback.h>
#include <hlsdk/dlls/cstrike/cbase.h>
#include <hlsdk/dlls/cstrike/weapons.h>
} // namespace hl

// include after hlsdk.
#include "custom_format_cheat.h"

enum EPlayerHull
{
	HULL_STANDING,
	HULL_DUCKING,
	HULL_POINT,
};

//-------------------------------------------------------------------
//
// Current project
// 
//-------------------------------------------------------------------
#include "oxware.h"

// hooks
#include "hook/tests/HookTests.h"
#include "hook/MemoryHook.h"
#include "hook/function_hooks/MemoryFnHook.h"
#include "hook/function_hooks/MemoryFnDetour.h"
#include "hook/function_hooks/SVCFuncDetour.h"
#include "hook/function_hooks/HLCommandDetour.h"
#include "hook/function_hooks/UserMSGDetour.h"
#include "hook/HLInterfaceHook.h"
#include "hook/SecurityModuleHook.h"
#include "hook/client_dll/MemoryHookCBaseStuff.h"

// util
#include "util/GameUtil.h"
#include "util/MathUtil.h"
#include "util/SteamIDUtil.h"
#include "util/engine/HLNetMessageIO.h"
#include "util/engine/build_num.h"
#include "util/engine/EngineInputManager.h"
#include "util/engine/GoldSrcMungificationAlgorigthms.h"
#include "util/engine/EngineRendering.h"
#include "util/engine/GoldSrcCommandManager.h"
#include "util/engine/parsemsg.h"
#include "util/engine/EngineSoundPlayer.h"
#include "util/engine/EngineSpeedControl.h"
#include "util/engine/FakePlayerRenderer.h"

// game
#include "game/model/StudioModelData.h"
#include "game/model/SpriteModelData.h"
#include "game/weapons/weapondefs.h"
#include "game/weapons/Weapons.h"
#include "game/GenericMdl.h"
#include "game/GenericEnt.h"
#include "game/GenericPlayer.h"
#include "game/EntityManager.h"
#include "game/SpriteManager.h"
#include "game/LocalState.h"
#include "game/ClientMovementPacket.h"
#include "game/VideoModeUtil.h"
#include "game/ParticlemanMiniMemEmulation.h"
#include "game/GameEvents.h"
#include "game/NetchanSequenceHistory.h"

// features
#include "features/FeatureManager.h"
#include "features/FieldOfViewChanger.h"
#include "features/BypassGameConstrains.h"
#include "features/FlashbangFadeModifier.h"
#include "features/ModelChams.h"
#include "features/Removals.h"
#include "features/ViewmodelOffset.h"
#include "features/CvarSandbox.h"
#include "features/SmokeVisuals.h"
#include "features/BulletTrace.h"
#include "features/FrameSkipper.h"
#include "features/ESP.h"
#include "features/VanillaCrosshair.h"
#include "features/StuffCmdFilter.h"
#include "features/ResourceConsistencyBypass.h"
#include "features/IngameScreenRendering.h"
#include "features/ThirdPerson.h"
#include "features/NonSteamFpsFixer.h"
#include "features/AntiDemoRecorder.h"
#include "features/WorldVisuals.h"
#include "features/AntiScreen.h"
#include "features/AspectRatioChanger.h"
#include "features/Automation.h"
#include "features/EnvironmentalEffects.h"
#include "features/BackTrack.h"
#include "features/Panic.h"
#include "features/SIDSpoofer.h"
#include "features/movement/BunnyHop.h"
#include "features/movement/AirStuck.h"
#include "features/movement/GroundStrafe.h"
#include "features/movement/EdgeBug.h"
#include "features/movement/StrafeHack.h"
#include "features/movement/StrafeHelper.h"
#include "features/movement/FastRun.h"
#include "features/movement/AutoJOF.h"
#include "features/movement/MovementVisualInfo.h"
#include "features/movement/Movement.h"

// ui
#include "ui/UIWindowPopups.h"
#include "ui/rendering_contexts/IRendererContext.h"
#include "ui/rendering_contexts/menu/MenuWidgets.h"
#include "ui/rendering_contexts/menu/MenuStyle.h"
#include "ui/rendering_contexts/menu/Menu.h"
#include "ui/rendering_contexts/MenuBackground.h"
#include "ui/rendering_contexts/FeatureList.h"
#include "ui/rendering_contexts/Console.h"
#include "ui/rendering_contexts/BackgroundRendering.h"
#include "ui/rendering_contexts/PerformanceProfilerVisualization.h"
#include "ui/KeyBinding.h"
#include "ui/ThemeEditor.h"
#include "ui/InCommandBinding.h"
#include "ui/CheatSettings.h"
#include "ui/PlayerList.h"
#include "ui/OxWareUI.h"

#endif // PRECOMPILED_H