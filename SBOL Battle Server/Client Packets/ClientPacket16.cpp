#include <Windows.h>
#include "..\server.h"

void ClientPacket16(CLIENT* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x1600: // Icons
	{
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1600);
		client->outbuf.setSubType(0x1680);
		client->outbuf.appendShort(3);
		for (int32_t i = 0; i < 3; i++) client->outbuf.appendInt(i);
	}
	break;
	}
	client->Send();
}
