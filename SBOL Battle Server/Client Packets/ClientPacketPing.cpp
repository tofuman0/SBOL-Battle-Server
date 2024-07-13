#include <Windows.h>
#include "..\server.h"

void ClientPacketPing(CLIENT* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0xA00:
	{	// Ping
		if (client->pokes < 2)
		{
			client->SendPing();
			client->pokes++;
			return;
		}
		else client->pokes = 0;
	}
	return;
	default:
		return;
	}
	client->Send();
}
