#include <Windows.h>
#include "..\server.h"

void ClientPacketPositioning(CLIENT* client)
{
	uint16_t pType = client->inbuf.getType();
	uint16_t clientID;
	if (client->course == nullptr) return;

	switch (pType)
	{
	case 0x700:
	{	// Player location
		clientID = client->inbuf.get<uint16_t>(0x04);
		if (clientID == client->courseID)
		{
			//client->inbuf.getArray((uint8_t*)&client->rivals[0].position, sizeof(POSITION), 0x06);
			if (clientID == client->courseID)
			{
				if (client->position.posX1 != 0.0f && client->position.posY1 != 0.0f)
				{
					float
						_x = client->inbuf.get<float>(0x0E, false),
						_y = client->inbuf.get<float>(0x12, false),
						dist = max(truncf(sqrtf(
							powf(abs(client->position.posX1 - _x), 2) +
							powf(abs(client->position.posY1 - _y), 2)
						) * 10) / 10,
							truncf(sqrtf(powf(abs(client->position.posX2 - _x), 2) +
								powf(abs(client->position.posY2 - _y), 2)
							) * 10) / 10);
					client->garagedata.activeCar->KMs += dist;
					if (client->battle.status == CLIENT::BATTLESTATUS::BS_IN_BATTLE) client->battle.KMs += dist;
				}
				client->inbuf.getArray((uint8_t*)&client->position, sizeof(POSITION), 0x06);
				if(client->logPosition)
					client->logger->Log(LOGGER::LOGTYPE_CLIENT, L"0x%04X,0x%04X,0x%04X", client->position.location1, client->position.location2, client->position.location3);
				//client->position.time += 1000;
				//Relay current position
				//if (client->battle.status == BATTLESTATUS::NOT_IN_BATTLE) 
				client->SendPosition();
				//else 
				//	client->SendPositionToOpponent();

				// Resend 780 to those close
				//client->SendPositionBrief();
			}
			else
			{
				client->logger->Log(LOGGER::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u has tried to spoof player ID %u",
					client->logger->toWide(client->handle).c_str(),
					client->driverslicense,
					client->logger->toWide((char*)&client->IP_Address).c_str(),
					clientID);
				client->Disconnect();
				return;
			}

			//client->logger->Log(LOGGER::LOGTYPE_PACKET, L"Packet: Server -> Course");
			//client->logger->Log(LOGGER::LOGTYPE_PACKET, client->logger->packet_to_text(&client->outbuf.buffer[0x00], client->outbuf.getSize()));

			return;
		}
		else
		{
			client->logger->Log(LOGGER::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u has tried to spoof player ID %u",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				clientID);
			client->Disconnect();
			return;
		}
	}
	break;
	default:
		return;
	}
	client->Send();
}
