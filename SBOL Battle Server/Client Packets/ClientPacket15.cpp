#include <Windows.h>
#include "..\server.h"

void ClientPacket15(Client* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x1500:
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1500);
		client->outbuf.setSubType(0x1580);
		client->outbuf.append<uint16_t>(0);
		//client->outbuf.append<uint32_t>(client->sign);
		break;
	}
	client->Send();
}
