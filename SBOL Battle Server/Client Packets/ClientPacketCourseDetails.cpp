#include <Windows.h>
#include "..\server.h"

void ClientPacketCourseDetails(Client* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x300:
	{	// Request Course Info
		uint32_t count = 8;
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x300);
		client->outbuf.setSubType(0x380);
		client->outbuf.append<uint8_t>(static_cast<uint8_t>(count)); // Number of courses
		for (uint32_t i = 0; i < count; i++)
		{
			char thisName[32] = { 0 };
			if (client->notBeginner == false) sprintf(&thisName[0], "Beginners (%u)", i + 1);
			else sprintf(&thisName[0], coursenames[i % count]);
			//else sprintf(&thisName[0], coursenames[i % count], client->course->getIndex() + 1);
			client->outbuf.appendString(std::string(&thisName[0]), 0x10);
			// Value in here causes client to reconnect to another server
			client->outbuf.append<uint8_t>(0); // No affect
			client->outbuf.append<uint8_t>(0); // No affect
			client->outbuf.append<uint16_t>(COURSE_NPC_LIMIT); // Rival Limit
			client->outbuf.append<uint16_t>(COURSE_NPC_LIMIT); // Liveview Limit
			client->outbuf.append<uint16_t>(COURSE_PLAYER_LIMIT); // Player Limit
			client->outbuf.append<uint16_t>(COURSE_PLAYER_LIMIT); // No affect - Crashes beginner client if set and course is not 0
			client->outbuf.append<uint32_t>(COURSE_PLAYER_LIMIT); // No affect
			for (int32_t j = 0; j < 0x10; j++) client->outbuf.append<uint8_t>(0);
			//client->outbuf.appendString("Live View?", 0x10); // Server name ???
			client->outbuf.append<uint32_t>(0); // No affect
			client->outbuf.append<uint16_t>(1); // If 0 additional 0xE4 Bytes - NPC????
										   //for (int32_t j = 0; j < 0xe4; j++) client->outbuf.append<uint8_t>(0);
		}
	}
	break;
	case 0x301:
	{
		client->inbuf.getArray((PBYTE)&client->garagedata.activeCar->carSettings, sizeof(CARSETTINGS), 0x05);
		for (int32_t i = 0; i < sizeof(CARSETTINGS); i++)
		{
			if (abs(((char*)&client->garagedata.activeCar->carSettings)[0]) > 15)
			{
				client->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has provided invalid car settings.",
					client->logger->toWide(client->handle).c_str(),
					client->driverslicense,
					client->logger->toWide((char*)&client->IP_Address).c_str());
				client->Disconnect();
				return;
			}
		}
		uint8_t courseNum = client->inbuf.get<uint8_t>(0x04);
		client->currentCourse = static_cast<uint32_t>(courseNum);
		client->position.location1 = client->inbuf.get<uint16_t>(0x17, false);
		client->position.location2 = client->inbuf.get<uint16_t>(0x19, false);
		client->position.location3 = client->inbuf.get<uint16_t>(0x1B, false);
		if (client->joinCourse() == 0)
			return;
		client->outbuf.clearBuffer();
		client->outbuf.setOffset(0x06);
		client->outbuf.setSize(0x06);
		client->outbuf.setType(0x300);
		client->outbuf.setSubType(0x381);
		client->outbuf.append<uint16_t>(client->courseID); // Client ID appears in all 0x400 / 0x700 packets
		client->outbuf.append<uint32_t>(1); // Value gets converted to float then multiplied by 6 then 1
		client->outbuf.append<uint16_t>(1); // if 0 0xE4 bytes sent below
		//for (int32_t i = 0; i < 0xE4 / 2; i++) client->outbuf.append<uint16_t>(i, false);
	}
	break;
	case 0x302:
	{	// responding to this crashes client but this appears to specify which shop the client has entered.
		client->inCourse = false;
		client->hasPlayers = false;
		client->SendRemoveRivals();
		client->inbuf.setOffset(0x04);
		uint32_t value1 = client->inbuf.get<uint32_t>();
		uint16_t courseJunction = client->inbuf.get<uint16_t>();
		uint16_t courseDistance = client->inbuf.get<uint16_t>();
		uint16_t value4 = client->inbuf.get<uint16_t>();
		uint16_t value5 = client->inbuf.get<uint16_t>();
		uint8_t courseNumber = client->inbuf.get<uint8_t>();
#ifdef _DEBUG
		client->logger->Log(Logger::LOGTYPE_Client, L"Client entering course: Float to Int ???: %u - Section: %02X - Distance: %02X - ???: %u - ???: %u - Course: %u.", value1, courseJunction, courseDistance, value4, value5, courseNumber);
#endif

		// Entering Course

		// Leaving Beginners course
		if (client->notBeginner == false && value4 == 0 && courseNumber != 0x15) client->notBeginner = true;
		if (courseNumber == 0x15) courseNumber = 9;
		else if (courseNumber > COURSE_COUNT || courseNumber == 0)
		{
			client->logger->Log(Logger::LOGTYPE_Client, L"Client sent tried to join invalid course.");
			client->Disconnect();
		}
		if (client->course != nullptr) client->course->removeClient(client);
		client->startJunction = courseJunction;
		client->startDistance = courseDistance;
		client->currentCourse = courseNumber - 1;
		client->position.posX1 = 0.0f;
		client->position.posY1 = 0.0f;

		//if (value4 == 0 && client->currentCourse == 0 && client->notBeginner == true)
		//{	// Live cam stuff. Only send while entering main course
		//	client->outbuf.clearBuffer();
		//	client->outbuf.setSize(0x06);
		//	client->outbuf.setOffset(0x06);
		//	client->outbuf.setType(0x300);
		//	client->outbuf.setSubType(0x382);
		//}
		//else
		return;
	}
	break;
	default:
		return;
	}
	client->Send();
}
