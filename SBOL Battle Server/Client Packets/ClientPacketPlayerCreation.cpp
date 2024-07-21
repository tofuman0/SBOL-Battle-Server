#include <Windows.h>
#include "..\server.h"

void ClientPacketPlayerCreation(Client* client)
{
	uint16_t pType = client->inbuf.getType();
	Server* server = (Server*)client->server;

	if (!server)
	{
		client->logger->Log(Logger::LOGTYPE_ERROR, L"No Server Pointer for client %s", client->logger->toWide((char*)client->IP_Address).c_str());
		return;
	}

	switch (pType)
	{
	case 0xE00:
	{
		//Client requested if username is taken.
		std::string tempHandle;
		tempHandle.assign(client->inbuf.getString(0x14, 0x10));
		if (tempHandle.find(" ") != std::string::npos)
		{
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0xE00);
			client->outbuf.setSubType(0xE80);
			client->outbuf.append<uint8_t>(0x04); // Spaces in handlename
		}
		client->setHandle(tempHandle);

		Server* server = (Server*)client->server;

		server->managementserver.outbuf.clearBuffer();
		server->managementserver.outbuf.setSize(0x06);
		server->managementserver.outbuf.setOffset(0x06);
		server->managementserver.outbuf.setType(0x0001);
		server->managementserver.outbuf.setSubType(0x0001);
		server->managementserver.outbuf.append<int32_t>(client->ClientSocket);
		server->managementserver.outbuf.setArray((uint8_t*)tempHandle.data(), tempHandle.size(), server->managementserver.outbuf.getOffset());
		server->managementserver.outbuf.setSize(server->managementserver.outbuf.getSize() + 16);
		server->managementserver.outbuf.setOffset(server->managementserver.outbuf.getOffset() + 16);
		if (server->managementserver.Send())
		{
			client->SendAuthError(Server::AUTHLIST::AUTH_BUSY);
			//client->Disconnect();
		}
		return;
	}
	break;
	case 0xE01:
	{
		//Client chosen starting car
		int32_t selectedCar = client->inbuf.get<uint32_t>(0x14);
		int32_t found = 0;
		for (uint32_t i = 0; i < server->startingCars.size(); i++)
		{
			if (selectedCar == server->startingCars[i])
			{
				// Initialize Garage
				client->initializeGarage();
				client->garagedata.garageType.push_back(0);
				client->garagedata.garageCount = 1;
				if (client->setActiveCar(0))
				{
					client->Disconnect();
					client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u) failed to set active car on new account.", client->logger->toWide(client->handle).c_str(), client->driverslicense);
					return;
				}

				// Add car to garage
				client->inbuf.setOffset(0x18);
				float r1 = client->inbuf.get<float>();
				float g1 = client->inbuf.get<float>();
				float b1 = client->inbuf.get<float>();
				float r2 = client->inbuf.get<float>();
				float g2 = client->inbuf.get<float>();
				float b2 = client->inbuf.get<float>();
				client->addCar(selectedCar, 0, r1, g1, b1, r2, g2, b2);

				client->outbuf.clearBuffer();
				client->outbuf.setSize(0x06);
				client->outbuf.setOffset(0x06);
				client->outbuf.setType(0xE00);
				client->outbuf.setSubType(0xE80);
				client->outbuf.append<uint8_t>(0x02);
				client->outbuf.append<uint32_t>(client->getActiveCar() + 1); // Active Car slot new accounts would be 0. Moved to 0x6d0ea4
				found = 1;
				client->canSave = true;
				server->saveClientData(client);
				client->enableStandardPackets(); // To disable 0xe00 packets again.
				break;
			}
		}
		if (!found)
		{
			//Illegal car choice
			client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u) has made an illegal initial car choice: Car %u.", client->logger->toWide(client->handle).c_str(), client->driverslicense, selectedCar);
			client->Disconnect();
			return;
		}
	}
	break;
	case 0xE02:
	{
		//Client is requesting Purchase list for first time play.
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0xE00);
		client->outbuf.setSubType(0xE81);
		client->outbuf.append<uint16_t>(static_cast<uint16_t>(server->startingCars.size()));
		for (uint32_t i = 0; i < server->startingCars.size(); i++) client->outbuf.append<uint32_t>(server->startingCars[i]);
	}
	break;
	default:
		return;
	}
	client->Send();
}
