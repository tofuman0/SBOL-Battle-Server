#include "course.h"
#include "client.h"

COURSE::COURSE()
{
	courseClient.clear();
	courseClient.resize(COURSE_PLAYER_LIMIT);
	for (auto& client : courseClient)
	{
		client = nullptr;
	}
	course = -1;
	index = 0;
	temp1 = 0;
	logger = nullptr;
	nextSend = 0;
}
COURSE::~COURSE()
{
}
void COURSE::sendToCourse(PACKET* src, int32_t exclude)
{
	for (auto& client : courseClient)
	{
		if (client != nullptr)
		{
			if (client->driverslicense != exclude && client->hasPlayers == true)
			{
				uint16_t size = src->getSize();
				if (size == 0) return;
				if (src->getType() == 0x700 && client->inCourse == false)
				{
					uint16_t clientID = src->get<uint16_t>(0x04);
				}
#ifdef PACKET_OUTPUT_DIS
				if (src->getType() != 0x0A00)
				{
					logger->Log(Logger::LOGTYPE_PACKET, L"Packet: Server -> Course");
					logger->Log(Logger::LOGTYPE_PACKET, logger->packet_to_text(&src->buffer[0x00], src->getSize()));
				}
#endif
				client->addToSendQueue(src);
			}
		}
	}
}
void COURSE::sendToProximity(PACKET* src, float x, float y, int32_t exclude)
{
	for (auto& client : courseClient)
	{
		if (client != nullptr)
		{
			float proximity = truncf(sqrtf(
				powf(abs(client->position.posX1 - x), 2) +
				powf(abs(client->position.posY1 - y), 2)
			) * 10) / 10;
			if (client->driverslicense != exclude && proximity < COURSE_PROXIMITY && client->inCourse == true)
			{
				uint16_t size = src->getSize();
				if (size == 0) return;
				if (src->getType() == 0x700 && client->inCourse == false)
				{
					uint16_t clientID = src->get<uint16_t>(0x04);
				}
#ifdef PACKET_OUTPUT_DIS
				if (src->getType() != 0x0A00)
				{
					logger->Log(Logger::LOGTYPE_PACKET, L"Packet: Server -> Course");
					logger->Log(Logger::LOGTYPE_PACKET, logger->packet_to_text(&src->buffer[0x00], src->getSize()));
				}
#endif
				client->addToSendQueue(src);
			}
		}
	}
}
void COURSE::sendToClient(PACKET* src, int32_t driverslicense)
{
	for (auto& client : courseClient)
	{
		if (client != nullptr)
		{
			if (client->driverslicense == driverslicense)
			{
				uint16_t size = src->getSize();
				if (size == 0) return;
				client->addToSendQueue(src);
				return;
			}
		}
	}
}
void COURSE::sendToID(PACKET* src, int16_t clientID)
{
	if (courseClient[clientID] == nullptr) return;
	uint16_t size = src->getSize();
	if (size == 0) return;
	courseClient[clientID]->addToSendQueue(src);
	return;
}
uint32_t COURSE::getClientCount()
{
	uint32_t count = 0;
	for (auto& client : courseClient)
	{
		if (client != nullptr) count++;
	}
	return count;
}
Client* COURSE::getClient(uint16_t clientID)
{
	for (auto& client : courseClient)
	{
		if (client != nullptr && client->courseID == clientID) return client;
	}
	return nullptr;
}
int32_t COURSE::addClient(Client* in)
{
	int32_t freeClient = -1;
	if (findClient(in) == -1)
	{
		freeClient = getFree();
		if (freeClient != -1)
		{
			courseClient[freeClient] = in;
			in->course = this;
			in->courseID = freeClient + 0x10;
		}
	}
	return freeClient;
}
void COURSE::removeClient(Client* in)
{
	int32_t clientIndex = findClient(in);
	if (clientIndex != -1)
	{
		if (in->battle.status == Client::BATTLESTATUS::BS_IN_BATTLE && in->battle.challenger != nullptr)
		{
			in->battle.status = Client::BATTLESTATUS::BS_LOST;
			in->battle.challenger->battle.status = Client::BATTLESTATUS::BS_WON;
			in->battle.challenger->SendBattleCheckStatus();
		}
		else if (in->battle.status != Client::BATTLESTATUS::BS_IN_BATTLE && in->battle.challenger != nullptr)
		{
			in->battle.challenger->SendBattleAbort(0);
		}
		in->clearBattle();
		//removeClientID(courseClient[clientIndex]->id);
		coursebuf.clearBuffer();
		coursebuf.setSize(0x06);
		coursebuf.setOffset(0x06);
		coursebuf.setType(0x400);
		coursebuf.setSubType(0x481);
		coursebuf.append<uint16_t>(courseClient[clientIndex]->courseID); // ID to remove
		sendToCourse(&coursebuf, courseClient[clientIndex]->driverslicense);
		courseClient[clientIndex]->course = nullptr;
		courseClient[clientIndex] = nullptr;
	}
}
int32_t COURSE::findClient(Client* in)
{
	for (uint32_t i = 0; i < courseClient.size(); i++)
	{
		if (courseClient[i] != nullptr)
		{
			if (courseClient[i] == in) return i;
		}
	}
	return -1;
}
Client* COURSE::findClient(int32_t in)
{
	for (auto& client : courseClient)
	{
		if (client != nullptr)
		{
			if (client->driverslicense == in) return client;
		}
	}
	return nullptr;
}
int32_t COURSE::getFree()
{
	for (uint32_t i = 0; i < courseClient.size(); i++)
	{
		if (courseClient[i] == nullptr) return i;
	}
	return -1;
}
void COURSE::sendCourseRacers(int32_t driverslicense)
{
	if (courseClient.size() > 0)
	{
		Client* workClient = findClient(driverslicense);
		if (workClient == nullptr) return;
		for (auto& client : courseClient)
		{
			if (client != nullptr)
			{
				if (client->driverslicense != driverslicense && client->inCourse == true)
				{
					coursebuf.clearBuffer();
					coursebuf.setSize(0x06);
					coursebuf.setOffset(0x06);
					coursebuf.setType(0x400);
					coursebuf.setSubType(0x480);
					coursebuf.append<uint16_t>(client->courseID); // ID
					coursebuf.appendString(client->handle, 0x10); // Player Name
					coursebuf.append<uint32_t>(0); // ????
					coursebuf.append<uint8_t>(client->careerdata.level); // Level
					// 0x6C byte array - Car Data
					coursebuf.append<uint32_t>(0, false); // ????
					coursebuf.append<uint32_t>(client->garagedata.activeCar->carID, false); // Car ID
					coursebuf.appendArray((uint8_t*)&client->garagedata.activeCar->carMods, sizeof(CARMODS) - sizeof(CARMODS::tuner));
					coursebuf.appendArray((uint8_t*)&client->garagedata.activeCar->carSettings, sizeof(CARSETTINGS));
					coursebuf.append<uint16_t>(0x00, false); // ????
					// Positioning
					coursebuf.append<uint16_t>(client->position.location1 ? client->position.location1 : client->startJunction, false); // Junction
					coursebuf.append<uint16_t>(client->position.location2 ? client->position.location2 : client->startDistance, false); // Distance
					coursebuf.append<uint16_t>(client->position.location3 ? client->position.location3 : 0x32, false); // ??
					coursebuf.append<uint16_t>(0xFFFF, false); // Set 0xFFFF to auto place in area
					// Flags
					coursebuf.append<uint8_t>(0x01); // npc stuff must be below 0x20. 0x00 = Self, 0x01 = Player, 0x02 = NPC
					coursebuf.append<uint8_t>(0x00); // 1st bit arrow is flashin (if player), 7th bit arrow is green, 8th bit arrow is red
					coursebuf.append<uint16_t>(client->status, false); // 1st bit car is in safe mode, When 3rd bit set car is transparent
					coursebuf.append<uint8_t>(0x00); // ???? 
					coursebuf.append<uint8_t>(0x00); // ????
					// 0x6C byte array - Team Data
					coursebuf.appendArray((uint8_t*)&client->teamdata, sizeof(TEAMDATA));
					coursebuf.append<uint8_t>(0); // If set with player notification of player entering is displayed.
					coursebuf.append<uint16_t>(0); // Count for something requires the shorts as many as these
					coursebuf.append<uint32_t>(0); // Icon next to name. if player.
					sendToClient(&coursebuf, driverslicense);
				}
			}
		}
	}
}
void COURSE::sendRacerPositions()
{
	if (courseClient.size() > 0 && nextSend < time(NULL))
	{
		nextSend = time(NULL) + COURSE_PACKET_TIME;
		int16_t count = 0;
		coursebuf.clearBuffer();
		coursebuf.setSize(0x06);
		coursebuf.setOffset(0x06);
		coursebuf.setType(0x700);
		coursebuf.setSubType(0x780);
		coursebuf.append<uint8_t>(course); // Course Number
		coursebuf.append<uint16_t>(count); // Place holder countCount
		for (auto& client : courseClient)
		{
			if (client != nullptr && (client->position.location1 + client->position.location2 + client->position.location3 != 0) && client->inCourse == true)
			{
				count++;
				coursebuf.append<uint16_t>(client->courseID); // ID
				coursebuf.append<uint16_t>(client->position.location2); // 700 / 0x3BC limit - Junction
				coursebuf.append<uint16_t>(client->position.location3); // Shifted right 5 - Position
				coursebuf.append<uint16_t>(client->position.location1); // 0x7FFF limit
				coursebuf.append<uint32_t>(client->position.time); // Epoch Time
				client->SendRivalPosition();
			}
		}
		if (count < 1) return;
		coursebuf.set<uint16_t>(count, 0x07); // Corrected Count
		sendToCourse(&coursebuf);
	}
}