#pragma once
#include <mutex>
#include <vector>
#include <Windows.h>
#include "structures.h"
#include "packet.h"
#include "Logger.h"

class Client;

class Course
{
private:
	int32_t course;
	int32_t index;
	time_t nextSend;
	int32_t tempValue1 = 0;
public:
	Course();
	~Course();
	Logger* logger;
	std::vector<Client*> courseClient;
	PACKET coursebuf;

	int32_t temp1;

	void sendToCourse(PACKET* src, int32_t exclude = -1);
	void sendToProximity(PACKET* src, float x, float y, int32_t exclude = -1);
	void sendToClient(PACKET* src, int32_t driverslicense);
	void sendToID(PACKET* src, int16_t clientID);
	void setCourse(int32_t in) { course = in; };
	void setIndex(int32_t in) { index = in; };
	int32_t getCourse() { return course; };
	int32_t getIndex() { return index; };
	uint32_t getClientCount();
	Client* getClient(uint16_t clientID);
	int32_t addClient(Client* in);
	void removeClient(Client* in);
	int32_t findClient(Client* in);
	Client* findClient(int32_t in);
	//void removeClientID(uint16_t id);
	int32_t getFree();
#pragma region Course Packets
	void sendCourseRacers(int32_t driverslicense);
	void sendRacerPositions();
#pragma endregion
};

