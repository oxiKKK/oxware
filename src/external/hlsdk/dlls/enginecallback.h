/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef ENGINECALLBACK_H
#define ENGINECALLBACK_H
#pragma once

// Must be provided by user of this code
extern enginefuncs_t g_engfuncs;

// The actual engine callbacks
#define EngGetPlayerUserId				(*g_engfuncs.pfnGetPlayerUserId)
#define EngPrecacheModel				(*g_engfuncs.pfnPrecacheModel)
#define EngPrecacheSound				(*g_engfuncs.pfnPrecacheSound)
#define EngPrecacheGeneric				(*g_engfuncs.pfnPrecacheGeneric)
#define EngSetModel						(*g_engfuncs.pfnSetModel)
#define EngModelIndex					(*g_engfuncs.pfnModelIndex)
#define EngModelFrames					(*g_engfuncs.pfnModelFrames)
#define EngSetSize						(*g_engfuncs.pfnSetSize)
#define EngChangeLevel					(*g_engfuncs.pfnChangeLevel)
#define EngGetSpawnParms				(*g_engfuncs.pfnGetSpawnParms)
#define EngSaveSpawnParms				(*g_engfuncs.pfnSaveSpawnParms)
#define EngVecToYaw						(*g_engfuncs.pfnVecToYaw)
#define EngVecToAngles					(*g_engfuncs.pfnVecToAngles)
#define EngMoveToOrigin					(*g_engfuncs.pfnMoveToOrigin)
#define EngChangeYaw					(*g_engfuncs.pfnChangeYaw)
#define EngChangePitch					(*g_engfuncs.pfnChangePitch)
#define EngMakeVectors					(*g_engfuncs.pfnMakeVectors)
#define EngCreateEntity					(*g_engfuncs.pfnCreateEntity)
#define EngRemoveEntity					(*g_engfuncs.pfnRemoveEntity)
#define EngCreateNamedEntity			(*g_engfuncs.pfnCreateNamedEntity)
#define EngMakeStatic					(*g_engfuncs.pfnMakeStatic)
#define EngEntIsOnFloor					(*g_engfuncs.pfnEntIsOnFloor)
#define EngDropToFloor					(*g_engfuncs.pfnDropToFloor)
#define EngWalkMove						(*g_engfuncs.pfnWalkMove)
#define EngSetOrigin					(*g_engfuncs.pfnSetOrigin)
#define EngEmitSound					(*g_engfuncs.pfnEmitSound)
#define EngBuildSoundMsg				(*g_engfuncs.pfnBuildSoundMsg)
#define EngTraceLine					(*g_engfuncs.pfnTraceLine)
#define EngTraceToss					(*g_engfuncs.pfnTraceToss)
#define EngTraceMonsterHull				(*g_engfuncs.pfnTraceMonsterHull)
#define EngTraceHull					(*g_engfuncs.pfnTraceHull)
#define EngGetAimVector					(*g_engfuncs.pfnGetAimVector)
#define EngServerCommand				(*g_engfuncs.pfnServerCommand)
#define EngServerExecute				(*g_engfuncs.pfnServerExecute)
#define EngClientCommand				(*g_engfuncs.pfnClientCommand)
#define EngParticleEffect				(*g_engfuncs.pfnParticleEffect)
#define EngLightStyle					(*g_engfuncs.pfnLightStyle)
#define EngDecalIndex					(*g_engfuncs.pfnDecalIndex)
#define EngPointContents				(*g_engfuncs.pfnPointContents)
#define EngCRC32_Init					(*g_engfuncs.pfnCRC32_Init)
#define EngCRC32_ProcessBuffer			(*g_engfuncs.pfnCRC32_ProcessBuffer)
#define EngCRC32_ProcessByte			(*g_engfuncs.pfnCRC32_ProcessByte)
#define EngCRC32_Final					(*g_engfuncs.pfnCRC32_Final)
#define EngRandomLong					(*g_engfuncs.pfnRandomLong)
#define EngRandomFloat					(*g_engfuncs.pfnRandomFloat)
#define EngGetPlayerAuthId				(*g_engfuncs.pfnGetPlayerAuthId)

inline void EngMessageBegin(int msg_dest, int msg_type, const float* pOrigin = NULL, edict_t* ed = NULL)
{
	(*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ed);
}

#define EngMessageEnd					(*g_engfuncs.pfnMessageEnd)
#define EngWriteByte					(*g_engfuncs.pfnWriteByte)
#define EngWriteChar					(*g_engfuncs.pfnWriteChar)
#define EngWriteShort					(*g_engfuncs.pfnWriteShort)
#define EngWriteLong					(*g_engfuncs.pfnWriteLong)
#define EngWriteAngle					(*g_engfuncs.pfnWriteAngle)
#define EngWriteCoord					(*g_engfuncs.pfnWriteCoord)
#define EngWriteString					(*g_engfuncs.pfnWriteString)
#define EngWriteEntity					(*g_engfuncs.pfnWriteEntity)
#define EngCVarRegister					(*g_engfuncs.pfnCVarRegister)
#define EngCVarGetFloat					(*g_engfuncs.pfnCVarGetFloat)
#define EngCVarGetString				(*g_engfuncs.pfnCVarGetString)
#define EngCVarSetFloat					(*g_engfuncs.pfnCVarSetFloat)
#define EngCVarSetString				(*g_engfuncs.pfnCVarSetString)
#define EngCVarGetPointer				(*g_engfuncs.pfnCVarGetPointer)
#define EngAlertMessage					(*g_engfuncs.pfnAlertMessage)
#define EngEngineFprintf				(*g_engfuncs.pfnEngineFprintf)
#define EngPvAllocEntPrivateData		(*g_engfuncs.pfnPvAllocEntPrivateData)

inline void *EngGetPrivate( edict_t *pent )
{
	if ( pent )
		return pent->pvPrivateData;

	return NULL;
}

#define EngFreeEntPrivateData			(*g_engfuncs.pfnFreeEntPrivateData)
#define EngSzFromIndex					(*g_engfuncs.pfnSzFromIndex)
#define EngAllocString					(*g_engfuncs.pfnAllocString)
#define EngFindEntityByString			(*g_engfuncs.pfnFindEntityByString)
#define EngGetEntityIllum				(*g_engfuncs.pfnGetEntityIllum)
#define EngFindEntityInSphere			(*g_engfuncs.pfnFindEntityInSphere)
#define EngFindClientInPVS				(*g_engfuncs.pfnFindClientInPVS)
#define EngEmitAmbientSound				(*g_engfuncs.pfnEmitAmbientSound)
#define EngGetModelPtr					(*g_engfuncs.pfnGetModelPtr)
#define EngRegUserMsg					(*g_engfuncs.pfnRegUserMsg)
#define EngGetBonePosition				(*g_engfuncs.pfnGetBonePosition)
#define EngFunctionFromName				(*g_engfuncs.pfnFunctionFromName)
#define EngNameForFunction				(*g_engfuncs.pfnNameForFunction)
#define EngTraceTexture					(*g_engfuncs.pfnTraceTexture)
#define EngClientPrintf					(*g_engfuncs.pfnClientPrintf)
#define EngCmd_Args						(*g_engfuncs.pfnCmd_Args)
#define EngCmd_Argc						(*g_engfuncs.pfnCmd_Argc)
#define EngCmd_Argv						(*g_engfuncs.pfnCmd_Argv)
#define EngGetAttachment				(*g_engfuncs.pfnGetAttachment)
#define EngSetView						(*g_engfuncs.pfnSetView)
#define EngCrosshairAngle				(*g_engfuncs.pfnCrosshairAngle)
#define EngLoadFileForMe				(*g_engfuncs.pfnLoadFileForMe)
#define EngFreeFile						(*g_engfuncs.pfnFreeFile)
#define EngCompareFileTime				(*g_engfuncs.pfnCompareFileTime)
#define EngGetGameDir					(*g_engfuncs.pfnGetGameDir)
#define EngIsMapValid					(*g_engfuncs.pfnIsMapValid)
#define EngNumberOfEntities				(*g_engfuncs.pfnNumberOfEntities)
#define EngIsDedicatedServer			(*g_engfuncs.pfnIsDedicatedServer)
#define EngPrecacheEvent				(*g_engfuncs.pfnPrecacheEvent)
#define EngPlaybackEvent				(*g_engfuncs.pfnPlaybackEvent)
#define EngSetFatPVS					(*g_engfuncs.pfnSetFatPVS)
#define EngSetFatPAS					(*g_engfuncs.pfnSetFatPAS)
#define EngCheckVisibility				(*g_engfuncs.pfnCheckVisibility)
#define EngDeltaSetField				(*g_engfuncs.pfnDeltaSetField)
#define EngDeltaUnsetField				(*g_engfuncs.pfnDeltaUnsetField)
#define EngDeltaAddEncoder				(*g_engfuncs.pfnDeltaAddEncoder)
#define EngGetCurrentPlayer				(*g_engfuncs.pfnGetCurrentPlayer)
#define	EngCanSkipPlayer				(*g_engfuncs.pfnCanSkipPlayer)
#define EngDeltaFindField				(*g_engfuncs.pfnDeltaFindField)
#define EngDeltaSetFieldByIndex			(*g_engfuncs.pfnDeltaSetFieldByIndex)
#define EngDeltaUnsetFieldByIndex		(*g_engfuncs.pfnDeltaUnsetFieldByIndex)
#define EngGetPhysicsInfoString			(*g_engfuncs.pfnGetPhysicsInfoString)
#define EngSetGroupMask					(*g_engfuncs.pfnSetGroupMask)
#define EngCreateInstancedBaseline		(*g_engfuncs.pfnCreateInstancedBaseline)
#define EngForceUnmodified				(*g_engfuncs.pfnForceUnmodified)
#define EngGetPlayerStats				(*g_engfuncs.pfnGetPlayerStats)

// Bot
#define EngCreateFakeClient				(*g_engfuncs.pfnCreateFakeClient)
#define EngGetInfoKeyBuffer				(*g_engfuncs.pfnGetInfoKeyBuffer)
#define EngSetKeyValue					(*g_engfuncs.pfnSetKeyValue)
#define EngSetClientKeyValue			(*g_engfuncs.pfnSetClientKeyValue)

#endif // ENGINECALLBACK_H
