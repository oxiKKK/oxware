#ifndef MULTIEMULATOR_H
#define MULTIEMULATOR_H
#pragma once

extern int GenerateAVSMP(void *pDest, int nSteamID, bool bUniverse = true);
extern int GenerateOldRevEmu(void* pDest, int nSteamID);
extern int GenerateRevEmu(void *pDest, int nSteamID);
extern int GenerateRevEmu2013(void *pDest, int nSteamID);
extern int GenerateSC2009(void* pDest, int nSteamID);
extern int GenerateSetti(void *pDest);
extern int GenerateSmartSteamEmu(void *pDest, int nSteamID);
extern int GenerateSteamEmu(void *pDest, int nSteamID);

#endif // MULTIEMULATOR_H