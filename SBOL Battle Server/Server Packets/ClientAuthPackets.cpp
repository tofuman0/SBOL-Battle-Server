#include <Windows.h>
#include "..\managementserver.h"
#include "..\server.h"
#include "..\structures.h"

void ClientAuth(MANAGEMENTSERVER* managementserver)
{
	CLIENT* client = nullptr;
	MESSAGE_QUEUE entry = { 0 };
	switch (managementserver->inbuf.getSubType())
	{
	case 0x0000: // Client authentication
	case 0x0001: // Check Handle
	{
		entry.socket = managementserver->inbuf.get<int32_t>(0x06);
		CopyMemory(&entry.buffer[0x00], &managementserver->inbuf.buffer[0x00], managementserver->inbuf.getSize() + 2);
		managementserver->addToMessageQueue(entry);
	}
	break;
	}
}
