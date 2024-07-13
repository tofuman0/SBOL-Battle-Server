#include <Windows.h>
#include "..\managementserver.h"

void ServerAuth(MANAGEMENTSERVER* managementserver)
{
	switch (managementserver->inbuf.getSubType())
	{
	case 0x0000: // Server
	{
		if (managementserver->inbuf.get<uint32_t>(0x0A) == managementserver->inbuf.get<uint32_t>(0x06) / 8 && managementserver->inbuf.getString(0x0E, 0x10) == "Tofuman")
		{
			managementserver->logger->Log(LOGGER::LOGTYPE_MANAGEMENT, L"Authenticated with management server");
		}
		else
		{
			managementserver->isAuth = false;
			managementserver->Disconnect();
			managementserver->logger->Log(LOGGER::LOGTYPE_MANAGEMENT, L"Failed to authenticate with management server");
		}
	}
	break;
	case 0x0001: // Heartbeat
	{
		managementserver->lastHeartBeat = time(NULL) + HEARTBEAT_TIME;
		managementserver->SendHeartBeat();
	}
	break;
	}
}
