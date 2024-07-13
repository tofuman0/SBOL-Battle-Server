#include <Windows.h>
#include "..\server.h"

void ClientPacketPlayerDetails(CLIENT* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x1000:
	{	// Players details
		client->SendPlayerStats();
		return;
	}
	break;
	default:
		return;
	}
	client->Send();
}
