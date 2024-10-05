#include <Windows.h>
#include "..\server.h"

void ClientPacketNotifications(Client* client)
{
	uint16_t pType = client->inbuf.getType();
	uint16_t clientID;

	if (client->course == nullptr) return;

	switch (pType)
	{
	case 0x400:
	{
		clientID = client->inbuf.get<uint16_t>(0x04);
		if (clientID == client->courseID)
		{	// Relay Player status to course
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x400);
			client->outbuf.setSubType(0x400);
			client->outbuf.appendArray(&client->inbuf.buffer[0x04], client->inbuf.getSize() - 0x04);
			client->SendToCourse(&client->outbuf, true);
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u has tried to spoof player ID %u",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				clientID);
			client->Disconnect();
		}
		return;
	}
	break;
	case 0x401:
	{
		clientID = client->inbuf.get<uint16_t>(0x04);

		if (clientID == client->courseID)
		{	// Relay Player status to course
			client->status = client->inbuf.get<uint16_t>(0x06);

			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x400);
			client->outbuf.setSubType(0x401);
			client->outbuf.appendArray(&client->inbuf.buffer[0x04], client->inbuf.getSize() - 0x04);
			client->SendToCourse(&client->outbuf, true);
			return;
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u has tried to spoof player ID %u",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				clientID);
			client->Disconnect();
			return;
		}
	}
	break;
	case 0x402:
	{	// Entered highway???
		clientID = client->inbuf.get<uint16_t>(0x04);
		if (clientID == client->courseID && client->inCourse == false)
		{
			client->SendCourseJoin();
			client->inCourse = true;
		}
		if (clientID == client->courseID)
		{
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x400);
			client->outbuf.setSubType(0x402);
			client->outbuf.appendArray(&client->inbuf.buffer[0x04], client->inbuf.getSize() - 0x04);
			client->SendToCourse(&client->outbuf);
		}
		std::stringstream ss;
		ss << "You have entered lobby " << client->course->getIndex() + 1 << ". (" << client->course->getClientCount() << "/" << COURSE_PLAYER_LIMIT << ")";
		client->SendAnnounceMessage(ss.str(), RGB(50, 100, 250), client->driverslicense);
		return;
	}
	break;
	default:
		return;
	}
	client->Send();
}
