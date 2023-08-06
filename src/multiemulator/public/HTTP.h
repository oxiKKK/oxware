#pragma once

#include <Windows.h>
#include <wininet.h>
#include <iostream>

#pragma comment(lib, "wininet.lib")

#define URL_INVALID 0
#define URL_HTTP    1
#define URL_HTTPS   2

static int IsURL(LPCSTR lpszURL)
{
	if (lpszURL != nullptr)
	{
		char c = lpszURL[4];

		if (c == ':')
			return URL_HTTP;
		else if (c == 's')
			return URL_HTTPS;
		else 
			return URL_INVALID;
	}

	return URL_INVALID;
}

static BOOL ParseURL(PCHAR pszURL, PCHAR pszHost, PCHAR pszObject, bool *bIsHTTPS = nullptr)
{
	if (pszURL == nullptr || pszHost == nullptr || pszObject == nullptr) return false;

	switch (IsURL(pszURL))
	{
	case URL_INVALID:
		return false;

	case URL_HTTP:
		if (bIsHTTPS) *bIsHTTPS = false;
		break;

	case URL_HTTPS:
		if (bIsHTTPS) *bIsHTTPS = true;
		break;
	}

	PCHAR pszLocalURL = _strdup(pszURL);
	PCHAR pszSeparator;

	pszSeparator = strstr(pszLocalURL, "://");
	if (pszSeparator == nullptr) return false;

	PCHAR pszHostBegin = pszSeparator + 3;

	pszSeparator = strstr(pszHostBegin, "/");
	if (pszSeparator == nullptr) return false;

	*pszSeparator = '\0';
	PCHAR pszObjectBegin = pszSeparator + 1;

	strcpy(pszHost, pszHostBegin);
	strcpy(pszObject, pszObjectBegin);

	free(pszLocalURL);

	return true;
}

static PBYTE HTTPGetRequest(LPCSTR lpszURL, INT* lpiResultSize = nullptr, INT* lpiReturnCode = nullptr)
{
	PBYTE lpResult = nullptr;

	if (lpiResultSize != nullptr) *lpiResultSize = 0;
	if (lpiReturnCode != nullptr) *lpiReturnCode = 0;

	CHAR lpszServerName[512], lpszObjectName[512];
	bool bIsHTTPS = false;

	if (!ParseURL((CHAR *)lpszURL, lpszServerName, lpszObjectName, &bIsHTTPS)) return nullptr;

	WORD wPort = bIsHTTPS ? (INTERNET_DEFAULT_HTTPS_PORT) : (INTERNET_DEFAULT_HTTP_PORT);
	DWORD iFlags = bIsHTTPS ? (INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD) : (INTERNET_FLAG_RELOAD);

	auto hIntSession = InternetOpenA("WinInet", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	auto hHttpSession = InternetConnectA(hIntSession, lpszServerName, wPort, 0, 0, INTERNET_SERVICE_HTTP, 0, NULL);
	auto hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", lpszObjectName, 0, 0, 0, iFlags, 0);

	auto szHeaders = "Content-Type: text/html";
	CHAR szReq[1024] = "";
	if (!HttpSendRequestA(hHttpRequest, szHeaders, strlen(szHeaders), szReq, strlen(szReq)))
	{
		lpResult = nullptr;
		goto Finally;
	}

	INT iResultSize;
	iResultSize = 0;

	CHAR szBuffer[4096 + 1];
	DWORD dwRead;
	dwRead = 0;
	while (InternetReadFile(hHttpRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) && dwRead)
	{
		iResultSize += dwRead;
		szBuffer[dwRead] = 0;

		lpResult = (PBYTE)realloc(lpResult, iResultSize + 1); // 1 for '\0'
		memcpy((PVOID)&lpResult[iResultSize - dwRead], szBuffer, dwRead);

		dwRead = 0;
	}

	if (lpResult) lpResult[iResultSize] = '\0';

	if (lpiResultSize != nullptr)
	{
		*lpiResultSize = iResultSize;
	}

	if (lpiReturnCode != nullptr)
	{
		CHAR responseText[256];
		DWORD responseTextSize = sizeof(responseText);

		if (HttpQueryInfoA(hHttpRequest, HTTP_QUERY_STATUS_CODE, &responseText, &responseTextSize, nullptr))
		{
			*lpiReturnCode = atoi(responseText);
		}
	}

Finally:
	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hIntSession);

	return lpResult;
}