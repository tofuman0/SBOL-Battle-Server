#define _WINSOCKAPI_
//#include <openssl/sha.h>
//#include <openssl/conf.h>
//#include <openssl/evp.h>
//#include <openssl/err.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "globals.h"
#include "server.h"
#include "serverpackets.h"
#include "managementserver.h"
#include <fstream>

MANAGEMENTSERVER::MANAGEMENTSERVER()
{
	initialize();
}
MANAGEMENTSERVER::~MANAGEMENTSERVER()
{
}
uint32_t MANAGEMENTSERVER::Start(Server* serverptr)
{
	initialize();
	server = serverptr;
	logger = &server->logger;
	if (LoadKey())
	{
		logger->Log(Logger::LOGTYPE_MANAGEMENT, L"Management key failed to load.");
		return 1;
	}
	if (!server)
		return 1;
	configured = true;
	running = true;
	std::thread mThread(managementServerThread, this);
	mThread.detach();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	logger->Log(Logger::LOGTYPE_MANAGEMENT, L"Management Server Thread Started.");

	return 0;
}
void MANAGEMENTSERVER::Restart()
{
	if (serverHandle) return;
	lastConnectAttempt = time(0);
	running = true;
	std::thread mThread(managementServerThread, this);
	mThread.detach();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
uint32_t MANAGEMENTSERVER::Stop()
{
	running = false;
	return 0;
}
uint32_t MANAGEMENTSERVER::LoadKey()
{
	std::ifstream keyfile(".\\serverkey.bin", std::ios::binary);
	if (!keyfile.is_open()) return 1;
	size_t size = (size_t)keyfile.tellg();
	keyfile.seekg(0, std::ios::end);
	size = (uint32_t)keyfile.tellg() - size;
	if (size != 48)
		return 1;
	keyfile.seekg(std::ios::beg);
	keyfile.read((char*)&key[0], 32);
	keyfile.read((char*)&iv[0], 16);
	keyfile.close();
	logger->Log(Logger::LOGTYPE_MANAGEMENT, L"Management key loaded.");
	return 0;
}
void MANAGEMENTSERVER::initialize()
{
	ServerSocket = -1;
	todc = false;
	isAuth = false;
	lastHeartBeat = 0;
	snddata = 0;
	connection_index = 0;
	rcvread = 0;
	packetsize = 0;
	packetoffset = 0;
	hassize = false;
	packetready = false;
	ZeroMemory(&IP_Address[0], sizeof(IP_Address));
	ZeroMemory(&ipaddr[0], sizeof(ipaddr));
	inbuf.clearBuffer();
	outbuf.clearBuffer();
	ZeroMemory(&rcvbuf[0], sizeof(rcvbuf));
	ZeroMemory(&sndbuf[0], sizeof(sndbuf));
	ZeroMemory(&key[0], sizeof(key));
	ZeroMemory(&iv[0], sizeof(iv));
	clearSendQueue();
}
uint32_t MANAGEMENTSERVER::Send(SERVERPACKET* src)
{
	if (src == nullptr)
		src = &outbuf;
	if (ServerSocket >= 0)
	{
		if (isAuth == false && src->getType() > 0x0000)
		{
			logger->Log(Logger::LOGTYPE_ERROR, L"Unable to send packet to management server %s as not authenticated.", logger->toWide((char*)IP_Address).c_str());
			return 2;
		}
		if (Client_BUFFER_SIZE < ((int32_t)src->getSize() + 15))
		{
			Disconnect();
			return 1;
		}
		else
		{
			uint16_t size = src->getSize();
			uint16_t newsize = 0;
			if ((size - 4) < 0 || size > Client_BUFFER_SIZE)
			{
				logger->Log(Logger::LOGTYPE_ERROR, L"Unable to send packet to client %s as packet will be %ubytes", logger->toWide((char*)IP_Address).c_str(), size);
				return 1;
			}
			src->setSize(size - 2);
			newsize = src->getSize();
			// Compress Packet
#ifndef DISABLE_COMPRESSION
			std::string compressed;
			compressed.resize(newsize);
			CopyMemory((char*)compressed.data(), &src->buffer[0x02], newsize);
			compressed = compress(compressed);
			newsize = compressed.size();
			if (newsize > Client_BUFFER_SIZE)
			{
				logger->Log(Logger::LOGTYPE_ERROR, L"Unable to send packet to client %s as packet will be %ubytes", logger->toWide((char*)IP_Address).c_str(), newsize);
				return 1;
			}
			if (newsize)
			{
				inbuf.clearBuffer();
				inbuf.setArray((uint8_t*)compressed.data(), newsize, 0x02);
			}
			src->setSize(newsize);
#endif
			while (src->getSize() % 16 || src->getSize() <= 16)
				src->append<uint8_t>(0);
			newsize = src->getSize();
			if (isAuth == true)
			{
				if (newsize + 2 + BLOCK_SIZE > Client_BUFFER_SIZE)
				{
					logger->Log(Logger::LOGTYPE_ERROR, L"Unable to send packet to client %s as packet will be %ubytes", logger->toWide((char*)IP_Address).c_str(), newsize + 2 + BLOCK_SIZE);
					return 1;
				}
				newsize = encrypt(&src->buffer[0x02], &src->buffer[0x02], static_cast<int32_t>(newsize));
				if (newsize < 16) return 1;
			}
			addToSendQueue(src);
		}
	}
	return 0;
}
void MANAGEMENTSERVER::Disconnect()
{
	todc = true;
}
bool MANAGEMENTSERVER::shouldRetry()
{
	time_t now = time(0);
	time_t result = now - lastConnectAttempt;
	return (result > 10);
}
void MANAGEMENTSERVER::ProcessPacket()
{
	if (!todc)
	{
		try {
			int16_t size = inbuf.getSize();
			if (isAuth == true)
			{
				size = decrypt(&inbuf.buffer[0x02], &inbuf.buffer[0x02], static_cast<int32_t>(size));
				if (size < 16) return;
				inbuf.setSize(size);
			}

			// Decompress Packet
#ifndef DISABLE_COMPRESSION
			std::string decompressed;
			decompressed.resize(size);
			CopyMemory((char*)decompressed.data(), &inbuf.buffer[0x02], size);
			decompressed = decompress(decompressed);
			size = decompressed.size();
			if (size)
			{
				inbuf.clearBuffer();
				inbuf.setArray((uint8_t*)decompressed.data(), size, 0x02);
			}
			else
				return;
			inbuf.setSize(size);
#endif
		}
		catch (std::exception ex)
		{
			logger->Log(Logger::LOGTYPE_COMM, L"Invalid Packet Message: %04X from client %s", inbuf.getType(), logger->toWide((char*)IP_Address).c_str());
			Disconnect();
			return;
		}

		if (inbuf.getType() > sizeof(PacketFunctions) / 4 || (isAuth == false && inbuf.getType() != SERVERPACKET::PACKETTYPE::PACKETTYPE_AUTH))
		{
			logger->Log(Logger::LOGTYPE_COMM, L"Invalid Packet Message: %04X from client %s", inbuf.getType(), logger->toWide((char*)IP_Address).c_str());
			Disconnect();
			return;
		}
#ifdef PACKET_OUTPUT
		if (inbuf.getType() != 0x0000 && inbuf.getSubType() != 0x0001)
		{
			logger->Log(Logger::LOGTYPE_MANAGEMENT, L"Packet: Management -> Server");
			logger->Log(Logger::LOGTYPE_MANAGEMENT, logger->packet_to_text(&inbuf.buffer[0], inbuf.getSize()).c_str());
		}
#endif
		PacketFunctions[inbuf.getType()](this);
	}
}
void MANAGEMENTSERVER::managementServerThread(void* parg)
{
	MANAGEMENTSERVER* managementserver = (MANAGEMENTSERVER*)parg;
	Server* gameserver = (Server*)managementserver->server;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int32_t wserror, selectcount;
	fd_set ReadFDs, WriteFDs;
	std::string managementPort;

	struct timeval select_timeout = {
		0,
		1
	};

	wserror = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wserror != 0)
	{
		managementserver->logger->Log(Logger::LOGTYPE_MANAGEMENT, L"WSAStartup failed for management server with error %u", wserror);
		goto end;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	managementPort = static_cast<std::ostringstream*>(&(std::ostringstream() << gameserver->getMANAGEMENTSERVERPort()))->str();
	wserror = getaddrinfo(gameserver->getMANAGEMENTSERVERAddress().c_str(), managementPort.c_str(), &hints, &result);
	if (wserror != 0) {
		managementserver->logger->Log(Logger::LOGTYPE_MANAGEMENT, L"getaddrinfo failed for management server with error %u", wserror);
		goto end;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			managementserver->logger->Log(Logger::LOGTYPE_MANAGEMENT, L"socket failed for management server with error %ld", WSAGetLastError());
			goto end;
		}
		wserror = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (wserror == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		managementserver->logger->Log(Logger::LOGTYPE_MANAGEMENT, L"unable to connect to management server on port %u", gameserver->getMANAGEMENTSERVERPort());
		goto end;
	}
	managementserver->logger->Log(Logger::LOGTYPE_MANAGEMENT, L"Connected to management server on port %u", gameserver->getMANAGEMENTSERVERPort());
	gameserver->setConnectedToMANAGEMENTSERVER();
	managementserver->ServerSocket = ConnectSocket;
	managementserver->SendAuth();
	managementserver->tcp_set_nonblocking(ConnectSocket);

	managementserver->running = true;

	while (managementserver->isRunning())
	{
		FD_ZERO(&ReadFDs);
		FD_ZERO(&WriteFDs);
		FD_SET(managementserver->ServerSocket, &ReadFDs);

		if(managementserver->snddata > 0 || managementserver->messagesInSendQueue())
			FD_SET(managementserver->ServerSocket, &WriteFDs);

		selectcount = select(0, &ReadFDs, &WriteFDs, NULL, &select_timeout);

		// Send
		if (FD_ISSET(managementserver->ServerSocket, &WriteFDs))
		{
			FD_CLR(managementserver->ServerSocket, &WriteFDs);
			if (managementserver->DataSend() == false)
			{
				managementserver->logger->Log(Logger::LOGTYPE_MANAGEMENT, L"Disconnected from management server");
				gameserver->clearConnectedToMANAGEMENTSERVER();
				break;
			}
		}
		// Receive
		//if (FD_ISSET(managementserver->ServerSocket, &ReadFDs))
		{
			//FD_CLR(managementserver->ServerSocket, &ReadFDs);
			if (managementserver->DataRecv() == false)
			{
				managementserver->logger->Log(Logger::LOGTYPE_MANAGEMENT, L"Disconnected from management server");
				gameserver->clearConnectedToMANAGEMENTSERVER();
				break;
			}
		}

		if (managementserver->lastHeartBeat < time(NULL) + 10)
		{
			managementserver->lastHeartBeat = time(NULL) + HEARTBEAT_TIME;
			managementserver->SendHeartBeat();
		}

		if (selectcount == 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

	}
end:
	managementserver->isAuth = false;
	managementserver->running = false;
	closesocket(ConnectSocket);
	WSACleanup();
	managementserver->setServerHandle(nullptr);
}
void MANAGEMENTSERVER::CheckManagementPackets(uint32_t rcvcopy, uint8_t* tmprcv)
{
	if (rcvcopy >= 6)
	{
		inbuf.clearBuffer();
		inbuf.appendArray(tmprcv, rcvcopy);
		inbuf.setSize(inbuf.getSizeFromBuffer());
		if (inbuf.getSize() > Client_BUFFER_SIZE)
		{
			// Packet too big, disconnect the client.
			logger->Log(Logger::LOGTYPE_MANAGEMENT, L"Management server %s sent an invalid packet.", toWide((char*)IP_Address).c_str());
			Disconnect();
		}
		else
		{
			ProcessPacket();
		}
	}
}
std::wstring MANAGEMENTSERVER::toWide(std::string in)
{
	std::wstring temp(in.length(), L' ');
	copy(in.begin(), in.end(), temp.begin());
	return temp;
}
std::string MANAGEMENTSERVER::toNarrow(std::wstring in)
{
	std::string temp(in.length(), ' ');
	copy(in.begin(), in.end(), temp.begin());
	return temp;
}
int32_t MANAGEMENTSERVER::encrypt(uint8_t* src, uint8_t* dst, int32_t len)
{
#ifndef DISABLE_ENCRYPTION
	try {
		int32_t _len;
		int32_t _ciphertext_len;
		EVP_CIPHER_CTX *ctx = nullptr;
		ctx = EVP_CIPHER_CTX_new();
		int error = 0;
		if (ctx != nullptr)
		{
			if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, &key[0], &iv[0]) != 1) goto crypterror;
			EVP_CIPHER_CTX_set_padding(ctx, 0);
			if (EVP_EncryptUpdate(ctx, dst, &_len, src, len) != 1) goto crypterror;
			_ciphertext_len = _len;
			if (EVP_EncryptFinal_ex(ctx, dst + len, &_len) != 1) goto crypterror;
			_ciphertext_len += _len;
			EVP_CIPHER_CTX_free(ctx);

			return _ciphertext_len;
		}
		else
		{
		crypterror:
			ERR_print_errors_fp(stderr);
			logger->Log(Logger::LOGTYPE_ERROR, L"Unable to encrypt data. %u", error);
			return -1;
		}
	}
	catch (exception ex) {
		ERR_print_errors_fp(stderr);
		logger->Log(Logger::LOGTYPE_ERROR, L"Unable to encrypt data: %s", ex.what());
		return -1;
	}
#else
	CopyMemory(dst, src, len);
	return len;
#endif
}
int32_t MANAGEMENTSERVER::decrypt(uint8_t* src, uint8_t* dst, int32_t len)
{
#ifndef DISABLE_ENCRYPTION
	try {
		int32_t _len = 0;
		int32_t _ciphertext_len;
		EVP_CIPHER_CTX *ctx = nullptr;
		ctx = EVP_CIPHER_CTX_new();
		
		if (ctx != nullptr)
		{
			if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, &key[0], &iv[0]) != 1) goto crypterror;
			EVP_CIPHER_CTX_set_padding(ctx, 0);
			if (EVP_DecryptUpdate(ctx, dst, &_len, src, len) != 1) goto crypterror;
			_ciphertext_len = _len;
			if (EVP_DecryptFinal_ex(ctx, dst + _len, &_len) != 1) goto crypterror;
			_ciphertext_len += _len;
			EVP_CIPHER_CTX_free(ctx);

			return _ciphertext_len;
		}
		else
		{
		crypterror:
			ERR_print_errors_fp(stderr);
			logger->Log(Logger::LOGTYPE_ERROR, L"Unable to decrypt data.");
			return -1;
		}

	}
	catch (exception ex) {
		ERR_print_errors_fp(stderr);
		logger->Log(Logger::LOGTYPE_ERROR, L"Unable to decrypt data: %s", ex.what());
		return -1;
	}
#else
	CopyMemory(dst, src, len);
	return len;
#endif
}
int32_t MANAGEMENTSERVER::compress(uint8_t* src, uint8_t* dst, int32_t len)
{	// TODO: Implement compression
	int32_t result = 0;
	return result;
}
int32_t MANAGEMENTSERVER::decompress(uint8_t* src, uint8_t* dst, int32_t len)
{	// TODO: Implement decompression
	int32_t result = 0;
	return result;
}
bool MANAGEMENTSERVER::DataSend()
{
	int32_t wserror, send_len, max_send;
	uint8_t sendbuf[Client_BUFFER_SIZE];
	SEND_MANAGEMENT_QUEUE entry;

	if (snddata > 0 || messagesInSendQueue())
	{
		if (snddata <= 0)
		{
			entry = getFromSendQueue();
			snddata = *(int16_t*)&entry.sndbuf[0] + 2;
			CopyMemory(sndbuf, entry.sndbuf, Client_BUFFER_SIZE);
		}

		if (snddata > Client_BUFFER_SIZE)
			max_send = Client_BUFFER_SIZE;
		else
			max_send = snddata;

		if ((send_len = send(ServerSocket, (char *)&sndbuf[0], max_send, 0)) == SOCKET_ERROR)
		{
			wserror = WSAGetLastError();
			if (wserror != WSAEWOULDBLOCK)
			{
				logger->Log(Logger::LOGTYPE_MANAGEMENT, L"Could not send data to management server. Socket Error %d", wserror);
				return false;
			}
		}
		else
		{
			snddata -= send_len;

			if (snddata > 0)
			{
				CopyMemory(&sendbuf[0], &sndbuf[send_len], snddata);
				CopyMemory(&sndbuf[0], &sendbuf[0], snddata);
			}
		}
		return true;
	}

	return true;
}
bool MANAGEMENTSERVER::DataRecv()
{
	int32_t wserror, recv_len, max_send;
	
	max_send = Client_BUFFER_SIZE - rcvread;

	if((recv_len = recv(ServerSocket, (char*)&rcvbuf[rcvread], max_send, 0)) == SOCKET_ERROR)
	{
		wserror = WSAGetLastError();
		if (wserror != WSAEWOULDBLOCK)
		{
			logger->Log(Logger::LOGTYPE_MANAGEMENT, L"Could not read data from management server. Socket Error %d", wserror);
			return false;
		}
	}
	else
	{
	_processmanagementpacket:
		if (recv_len == 0)
		{
			wserror = WSAGetLastError();
			if (wserror == WSAECONNRESET)
				return false;
		}
		ProcessRecv(recv_len);
		return true;
	}
	if (rcvread && recv_len == -1)
	{
		recv_len = 0;
		goto _processmanagementpacket;
	}
	return true;
}
bool MANAGEMENTSERVER::ProcessRecv(int32_t len)
{
	if (len < 0)
		return false;
	uint8_t recvbuf[Client_BUFFER_SIZE];
	rcvread += len;
	if ((hassize == false || packetsize == 0) && rcvread >= 2)
	{
		packetsize = *(uint16_t*)&rcvbuf[0] + 2;
		hassize = true;
	}
	if (hassize == true && rcvread >= packetsize && packetsize >= 6)
	{
		CopyMemory(&recvbuf[0], &rcvbuf[0], packetsize);
		CheckManagementPackets(packetsize, recvbuf);
		CopyMemory(&recvbuf[0], &rcvbuf[packetsize], Client_BUFFER_SIZE - packetsize);
		CopyMemory(&rcvbuf[0], &recvbuf[0], Client_BUFFER_SIZE - packetsize);
		hassize = false;
		rcvread -= packetsize;
		packetsize = 0;
		return true;
	}
	return false;
}
bool MANAGEMENTSERVER::DataRecvToProcess()
{
	return rcvread ? true : false;
}
int32_t MANAGEMENTSERVER::tcp_set_nonblocking(int32_t  fd)
{
	u_long flags = 1;
	if (ioctlsocket(fd, FIONBIO, &flags))
		return 1;
	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&flags, sizeof(flags));
}
void MANAGEMENTSERVER::addToMessageQueue(MESSAGE_QUEUE& in)
{
	std::lock_guard<std::mutex> locker(_muManagement);
	messagequeue.push(in);
}
MESSAGE_QUEUE MANAGEMENTSERVER::getTopFromMessageQueue()
{
	MESSAGE_QUEUE current = { 0 };
	std::lock_guard<std::mutex> locker(_muManagement);
	if (messagequeue.size())
	{
		current = messagequeue.front();
		messagequeue.pop();
		return current;
	}
	else
	{
		return current;
	}
}
uint32_t MANAGEMENTSERVER::messagesInQueue()
{
	std::lock_guard<std::mutex> locker(_muManagement);
	return messagequeue.size();
}
void MANAGEMENTSERVER::addToSendQueue(SERVERPACKET* src)
{
	*(uint16_t*)&src->buffer[0x00] = src->getSize();
	SEND_MANAGEMENT_QUEUE entry = { 0 };
	CopyMemory(&entry.sndbuf[0], &src->buffer[0x00], min(src->getSize() + 2, sizeof(sndbuf) - 2));
	std::lock_guard<std::mutex> locker(_muPackets);
	sendQueue.push(entry);
}
SEND_MANAGEMENT_QUEUE MANAGEMENTSERVER::getFromSendQueue()
{
	SEND_MANAGEMENT_QUEUE current = { 0 };
	std::lock_guard<std::mutex> locker(_muPackets);
	if (sendQueue.size())
	{
		current = sendQueue.front();
		sendQueue.pop();
		return current;
	}
	return current;
}
uint32_t MANAGEMENTSERVER::messagesInSendQueue()
{
	std::lock_guard<std::mutex> locker(_muPackets);
	if (sendQueue.size() > 10)
	{
		logger->Log(Logger::LOGTYPE_COMM, L"Send Queue exceeds 10 for management server. %u in queue",
			sendQueue.size()
		);
	}
	return sendQueue.size();
}

void MANAGEMENTSERVER::clearSendQueue()
{
	std::lock_guard<std::mutex> locker(_muPackets);
	std::queue<SEND_MANAGEMENT_QUEUE> q;
	sendQueue.swap(q);
}

#pragma region Packets
void MANAGEMENTSERVER::TeamCreate(uint32_t license, std::string& teamname)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0002);
	outbuf.setSubType(0x0003);
	outbuf.append<uint32_t>(license);
	outbuf.appendString(teamname, 0x10);
	Send();
}
void MANAGEMENTSERVER::TeamDelete(uint32_t license)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0002);
	outbuf.setSubType(0x0006);
	outbuf.append<uint32_t>(license);
	Send();
}
void MANAGEMENTSERVER::TeamRemoveTeamMember(uint32_t license, uint32_t memberID)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0002);
	outbuf.setSubType(0x0009);
	outbuf.append<uint32_t>(license);
	outbuf.append<uint32_t>(memberID);
	Send();
}
void MANAGEMENTSERVER::TeamUpdateInviteOnly(uint32_t license, uint8_t invitestatus)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0002);
	outbuf.setSubType(0x0004);
	outbuf.append<uint32_t>(license);
	outbuf.append<uint8_t>(invitestatus);
	Send();
}
void MANAGEMENTSERVER::TeamUpdateComment(uint32_t license, std::string& comment)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0002);
	outbuf.setSubType(0x0005);
	outbuf.append<uint32_t>(license);
	outbuf.appendString(comment, 0x28);
	Send();
}
void MANAGEMENTSERVER::TeamSetAreaAccess(uint32_t license, uint32_t memberID, uint8_t access)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0002);
	outbuf.setSubType(0x0008);
	outbuf.append<uint32_t>(license);
	outbuf.append<uint32_t>(memberID);
	outbuf.append<uint8_t>(access);
	Send();
}
void MANAGEMENTSERVER::TeamGetData(uint32_t license)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0002);
	outbuf.setSubType(0x0007);
	outbuf.append<uint32_t>(license);
	Send();
}
void MANAGEMENTSERVER::SendAuth()
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0000);
	outbuf.setSubType(0x0000);
	outbuf.appendArray(&iv[0], sizeof(iv));
	Send();
	isAuth = true;
}
void MANAGEMENTSERVER::SendHeartBeat()
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0000);
	outbuf.setSubType(0x0001);
	uint32_t currTime = timeGetTime();
	outbuf.append<uint32_t>(currTime);
	Send();
}
void MANAGEMENTSERVER::SwapCar(uint32_t license, uint8_t isSwapping, uint32_t fromBay, uint32_t toBay)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0002);
	outbuf.setSubType(0x0001);
	outbuf.append<uint32_t>(license);
	outbuf.append<uint8_t>(isSwapping);
	outbuf.append<uint32_t>(fromBay);
	outbuf.append<uint32_t>(toBay);
	Send();
}
void MANAGEMENTSERVER::RemoveCar(uint32_t license, uint32_t bay)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x0002);
	outbuf.setSubType(0x0002);
	outbuf.append<uint32_t>(license);
	outbuf.append<uint32_t>(bay);
	Send();
}
#pragma endregion