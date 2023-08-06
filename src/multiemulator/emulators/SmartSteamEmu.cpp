#include "..\Public\StrUtils.h"
#include "..\Public\RevSpoofer.h"
#include "..\Public\crypto\CRijndael.h"
#include "..\Public\crypto\SHA.h"
#include "..\Public\ExternIP.h"
#include <WinSock.h>
#include <Windows.h>
#include <time.h>

#define SSE3_DATA_SIZE 32

/* 
 * MurmurHash3 non-cryptographic hash function. Used 
 * in SSE3 emulator as an alternative to RevHash().
 *
 * @param pData  Pointer to 32-byte array which will be hashed.
 *
 * @output       Result hash (steamid).
 */
int SmartSteamEmuHash(void* pData)
{
	if (!pData)
		return 0;

	auto seed = 47;
	auto p = (int*)pData;

	for (int i = 0; i < 8; i++)
	{
		auto h1 = _rotl(0xCC9E2D51 * p[i], 15);
		auto h2 = _rotl(0x1B873593 * h1 ^ seed, 13);
		seed = 5 * (h2 - 0x052250EC);
	}

	auto hash = seed ^ SSE3_DATA_SIZE;
	hash ^= hash >> 16;
	hash *= 0x85EBCA6B;
	hash ^= hash >> 13;
	hash *= 0xC2B2AE35;
	hash ^= hash >> 16;

	return hash;
}

// TODO: Encryption and routine validation
int GenerateSmartSteamEmu(void *pDest, int nSteamID)
{
	auto pTicket = (int *)pDest;
	auto pbTicket = (unsigned char *)pDest;

	auto uIP = GetExternalIPLong();

	pTicket[0] = 'UMEH';   //  +0, header
	pTicket[1] = 315;      //  +4, method of ticket checking, but dproto/reunion just want this value to be >= 315
	pTicket[2] = uIP;      //  +8, IP address of ticket's owner; needs to be set, because reunion checks it
	pTicket[3] = ' ESS';   // +12, magic number
	pTicket[4] = 0;        // +16
	pTicket[5] = 0;        // +20

	/* This part of ticket encrypted with RSA-256 algorithm */

	*(long long *)&pbTicket[6] = 0; // +24, key for AES part; GetVersionExW() stuff

	/* End of encrypted part */

	pTicket[8] = 0;                           // +32
	pTicket[9] = 0;                           // +36
	pTicket[10] = 0;                          // +40
	pTicket[11] = 0;                          // +44
	pTicket[12] = SmartSteamEmuHash(nullptr); // +48, SteamId, Low part
	pTicket[13] = 0x01100001;                 // +52, SteamId, High part
	pTicket[14] = 32;                         // +56, length of next encrypted data, dproto/reunion validate this value (!= 0, < 64)

	/* This part of ticket encrypted with AES algorithm */

	pTicket[15] = rand() + (rand() << 16);   // +60, unknown random int32 value, probably needs for encrypt things
	pTicket[16] = pTicket[3];                // +64, looks like it must to be ' ESS' too
											 
	pTicket[17] = pTicket[12];               // +68, SteamId, Low part
	pTicket[18] = pTicket[13];               // +72, SteamId, High part
											 
	pTicket[19] = pTicket[2];                // +76, our IP again
	pTicket[20] = 0;                         // +80, unused
	*(long long *)&pTicket[21] = _time64(0); // +84, ticket generation time

	/* End of encrypted part */

	pTicket[23] = 0;                         // +92, htonl()
	pTicket[24] = 0;                         // +96 (word), htons()

	return 104;
}