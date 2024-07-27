#include <Windows.h>
#include "..\server.h"

void ClientPacketRivalDetails(Client* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0xC00:
	{
		client->SendCareerRecord();
	}
	return;
	case 0xC01:
	{
		client->SendRivalRecords();
	}
	return;
	default:
		return;
	}
	client->Send();
}
