#include "HTTP.h"
#include <WinSock.h>

#pragma comment(lib, "Ws2_32.lib")

const char* g_pszHosts[] = 
{
	"http://myexternalip.com/raw",
	"http://api.ipify.org",
	"http://ipinfo.io/ip"
};

/*
* Extracts external IP of the current machine via internet
* resources.
*
* @output       IPv4 address as string.
*/
char *GetExternalIPString()
{
	int iResSize = 0;
	int iRetCode = 0;
	void *pData = nullptr;

	for (auto &&host : g_pszHosts)
	{
		pData = HTTPGetRequest(host, &iResSize, &iRetCode);

		if (pData && iResSize && iRetCode == 200)
			break;
	}

	if (!pData || !iResSize || iRetCode != 200)
		return nullptr;

	static char c_szIP[256];
	strcpy(c_szIP, (char *)pData);

	auto psz = c_szIP;
	char c;
	while (c = *psz, c != '\0')
	{
		if (c == '\n')
		{
			*psz = '\0';
			break;
		}

		psz++;
	}
	free(pData);

	return c_szIP;
}

/*
* Extracts external IP of the current machine via internet
* resources.
*
* @output       IPv4 address as integer.
*/
long GetExternalIPLong()
{
	auto pszIP = GetExternalIPString();
	return inet_addr(pszIP);
}