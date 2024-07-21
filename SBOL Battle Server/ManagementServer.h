#pragma once
#include "Logger.h"
#include "serverpacket.h"
#include "structures.h"
#include <mutex>
#include <queue>

class SERVER;

class MANAGEMENTSERVER
{
	std::mutex _muManagement;
	std::mutex _muPackets;
public:
	MANAGEMENTSERVER();
	~MANAGEMENTSERVER();
	SERVER* server;
	uint32_t Start(SERVER* serverptr);
	void Restart();
	uint32_t Stop();
	static void managementServerThread(void* parg);
	void CheckManagementPackets(uint32_t rcvcopy, uint8_t* tmprcv);
	int32_t ServerSocket;
	bool todc;
	bool isAuth;
	time_t lastHeartBeat;
	void setAuth() { isAuth = true; }
	void clearAuth() { isAuth = false; }
	int32_t snddata;
	uint32_t connection_index;
	uint16_t rcvread;
	uint16_t packetsize;
	uint16_t packetoffset;
	bool hassize;
	bool packetready;
	uint8_t IP_Address[16]; // Text version
	uint8_t ipaddr[4]; // Binary version
	uint8_t rcvbuf[CLIENT_BUFFER_SIZE];
	uint8_t sndbuf[CLIENT_BUFFER_SIZE];
	uint8_t key[32];
	uint8_t iv[16];
	SERVERPACKET inbuf;
	SERVERPACKET outbuf;
	void initialize();
	uint32_t Send(SERVERPACKET* src = nullptr);
	void Disconnect();
	void ProcessPacket();
	int32_t encrypt(uint8_t* src, uint8_t* dst, int32_t len);
	int32_t decrypt(uint8_t* src, uint8_t* dst, int32_t len);
	int32_t compress(uint8_t* src, uint8_t* dst, int32_t len);
	int32_t decompress(uint8_t* src, uint8_t* dst, int32_t len);
	bool DataSend();
	bool DataRecv();
	bool ProcessRecv(int32_t len);
	bool DataRecvToProcess();
	bool isRunning() { return running; }
	bool isConfigured() { return configured; }
	bool shouldRetry();
	void setServerHandle(HANDLE* ptr) { serverHandle = ptr; }
	Logger* logger;
	void addToMessageQueue(MESSAGE_QUEUE& in);
	MESSAGE_QUEUE getTopFromMessageQueue();
	uint32_t messagesInQueue();
	void addToSendQueue(SERVERPACKET* src);
	SEND_MANAGEMENT_QUEUE getFromSendQueue();
	uint32_t messagesInSendQueue();
	void clearSendQueue();
#pragma region Packets
	void TeamCreate(uint32_t license, std::string& teamname);
	void TeamDelete(uint32_t license);
	void TeamRemoveTeamMember(uint32_t license, uint32_t memberID);
	void TeamUpdateInviteOnly(uint32_t license, uint8_t invitestatus);
	void TeamUpdateComment(uint32_t license, std::string& comment);
	void TeamSetAreaAccess(uint32_t license, uint32_t memberID, uint8_t access);
	void TeamGetData(uint32_t license);
	void SendAuth();
	void SendHeartBeat();
	void SwapCar(uint32_t license, uint8_t isSwapping, uint32_t fromBay, uint32_t toBay);
	void RemoveCar(uint32_t license, uint32_t bay);
#pragma endregion
private:
	std::queue<MESSAGE_QUEUE> messagequeue;
	std::queue<SEND_MANAGEMENT_QUEUE> sendQueue;
	uint32_t LoadKey();
	HANDLE serverHandle;
	time_t lastConnectAttempt;
	std::wstring toWide(std::string in);
	std::string toNarrow(std::wstring in);
	int32_t tcp_set_nonblocking(int32_t fd);
	bool running;
	bool configured;
};

