#include "globals.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "mstcpip.h"
#include "server.h"
#ifdef _DEBUG
#include "debug.h"
#endif

const char *coursenames[] = {
	"Main",
	"Car Shop",
	"Parts",
	"Freeway A",
	"Freeway B",
	"Survival",
	"Time Attack A",
	"Time Attack B",
	"Spare 1",
	"Spare 2",
	"Spare 3",
	"Spare 4",
	"Spare 5",
	"Spare 6",
	"Spare 7",
	"Spare 8",
	"Spare 9",
	"Spare 10",
	"Spare 11",
	"Spare 12",
};

Server::Server()
{
	managementServerAddress = "localhost";
	managementServerPort = 7946;
	serverPort = 47701;
	serverClientLimit = SERVER_LIMIT;
	serverWelcomeMessage = "";
	running = false;
	connectedToManagement = false;
	status = SERVERSTATUS::SS_RUNNING;
	serverNumConnections = 0;
	ServerSocket = -1;
	TempSocket = -1;
	currentClientID = 0;
	ZeroMemory((uint8_t*)&pDAT, sizeof(P_DAT));
	ZeroMemory((uint8_t*)&sDAT, sizeof(S_DAT));
	ZeroMemory((uint8_t*)&shopData, sizeof(PARTSHOPDATA));
	ZeroMemory((uint8_t*)&itemData, sizeof(ITEMDATA));
	logger = new Logger();
	startingCars = {
		/*AE86_T_3_1985,
		SILVIA_Q_1988,*/
		AE86_L_3_1985,
		AE86_T_3_1985,
		AE86_L_2_1985,
		AE86_T_2_1985,
		MR2_GT_1997,
		SUPRA_RZ_1993,
		SUPRA_RZ_1997,
		SUPRA_SZR_1997,
		ALTEZZA_R200_Z_1997,
		CHASER_TOURER_V_1996,
		CHASER_TOURER_V_1998,
		MARKII_TOURER_V_1998,
		SILVIA_K_1988,
		SILVIA_Q_1988,
		SILVIA_K_1991,
		SILVIA_K_1993,
		SILVIA_K_1996,
		SILVIA_SPECR_1999,
		SILVIA_SPECS_1999,
		S180SX_TYPEIII_1994,
		SKYLINE_GTR_VSPECII_1994,
		SKYLINE_GTR_1989,
		SKYLINE_GTST_TYPEM_1991,
		SKYLINE_GTR_VSPEC_1997,
		SKYLINE_25GT_TURBO_1998,
		SKYLINE_25GT_TURBO_2000,
		FAIRLADY_Z_S_TT_1998,
		FAIRLADY_Z_TBAR_1998,
		CEDRIC_BROUGHAM_VIP_1997,
		GLORIA_GRANTURISMO_ULTIMA_1997,
		LANCER_GSR_EVOIII_1995,
		LANCER_GSR_EVOIV_1996,
		SAVANNA_RX7_INFIIII_1989,
		RX7_TYPERS_1999,
		RX7_TYPERZ_1998,
		IMPREZA_WRX_STI_VERIV_1997,
		IMPREZA_WRX_STI_VERV_1998,
		IMPREZA_WRX_STI_VERVI_1999,
		FAIRLADY_Z_240ZG_1972,
		MX5_ROADSTER_RS_1998,
		MX5_ROADSTER_RS_2000,
		CELICA_GTFOUR_1996,
		CELICA_SSIII_1996,
		GTO_TT_1993,
		FTO_GP_VERR_1997,
		PULSAR_SERIE_VZN1_VER2_1998,
		LEGACY_TOURING_WAGON_GTB_1998,
		CELICA_SSII_SSPACK_1999,
		MRS_1999,
		CEDRIC_300VIP_1999,
		GLORIA_300_ULTIMA_1999,
		AZ1_1992,
		CAPPUCCINO_1991,
		CEFIRO_CRUISING_1990,
		STARION_GSRVR_1988,
		SAVANNA_RX3_GT_1973,
		SKYLINE_2000GTR_1971,
		SAVANNA_RX7_GTX_1983,
		ECLIPSE_GST_1999,
		BB15Z_2000,
		SUPRA_30GT_T_1989,
		SUPRA_25GT_TT_1991,
		LANCER_GSR_EVOVII_2001,
		IMPREZA_WRX_NB_2000,
		IMPREZA_WRX_STI_2000,
		LANTIS_COUPE_TYPER_1993
	};
	expToLevel = {
		0,
		500,
		2000,
		4500,
		8000,
		12500,
		18000,
		24500,
		32000,
		40500,
		50000,
		60500,
		72000,
		84500,
		98480,
		114270,
		132520,
		153640,
		177850,
		205390,
		236440,
		271180,
		309790,
		352450,
		399240,
		450220,
		505480,
		565150,
		629350,
		698160,
		771650,
		849930,
		933090,
		1021260,
		1114550,
		1213100,
		1316980,
		1426250,
		1541030,
		1661450,
		1787620,
		1919620,
		2057560,
		2201530,
		2351660,
		2508070,
		2670860,
		2840120,
		3015950,
		3198470,
		3387790,
		3584040,
		3787300,
		3997660,
		4215240,
		4440160,
		4672490,
		4912380,
		5159910,
		5415130,
		5678150,
		5949080,
		6228000,
		6515020,
		6810230,
		7113750,
		7425660,
		7746040,
		8075020,
		8412710,
		8759200,
		9114600,
		9479040,
		9852590,
		10235330,
		10627370,
		11028820,
		11439800,
		11860400,
		12290710,
		12730810,
		13180920,
		13641250,
		14112010,
		14593480,
		15085990,
		15589830,
		16105360,
		16633000,
		17173210,
		17726470,
		18293300,
		18874280,
		19470040,
		20081260,
		20708670,
		21353100,
		22015390,
		22696480,
		23397350
	};
}
Server::~Server()
{
	for (auto& c : connections)
		delete c;
	connections.clear();
	if (pDAT.pdat) free(pDAT.pdat);
	if (sDAT.sdat) free(sDAT.sdat);
	if (shopData.data) free(shopData.data);
	if (itemData.data) free(itemData.data);
	for (uint32_t i = 0; i < COURSE_DIMENSIONS; i++)
	{	
		for (auto& course : courses[i])
		{
			delete course;
		}
		courses[i].clear();
	}
}
void Server::initialize() {
	for (auto& c : connections)
		delete c;
	connections.clear();
	connections.shrink_to_fit();
	authenticatedClients.clear();
	authenticatedClients.shrink_to_fit();
	for (uint32_t i = 0; i < COURSE_DIMENSIONS; i++)
	{
		int32_t courseNum = 0;
		courses[i].clear();
		courses[i].resize(COURSE_COUNT);
		for (auto& course : courses[i])
		{
			course = new Course();
			course->setIndex(i);
			course->logger = logger;
			course->setCourse(courseNum++);
		}
	}
	serverConnections.resize(serverClientLimit);
	LoadRivalFile();
}
char* Server::HEXString(uint8_t* in, char* out, uint32_t length = 0)
{
	uint32_t offset = 0;
	if (!in) return out;
	uint8_t* getLength = in;
	while ((*getLength && (getLength - in) < MAX_MESG_LEN) || ((length > 0) && (offset / 2 < length)))
	{
		sprintf(&out[offset], "%02X", getLength[0]);
		offset += 2;
		getLength++;
		if (length && (offset / 2 >= length)) break;
	}
	out[offset] = 0;
	return out;
}
char* Server::HEXString(uint8_t* in, uint32_t length = 0)
{
	char* out = &hexStr[0];
	uint32_t offset = 0;
	if (!in) return out;
	uint8_t* getLength = in;
	while ((*getLength && (getLength - in) < MAX_MESG_LEN) || (length && ((offset / 2) < length)))
	{
		sprintf(&out[offset], "%02X", getLength[0]);
		offset += 2;
		getLength++;
		if (length && (offset / 2 >= length)) break;
	}
	out[offset] = 0;
	return out;
}
int32_t Server::Start()
{
	if (VerifyConfig())
		return 1;

	if (loadPDat())
		return 1;

	if (loadSDat())
		return 1;

	if (loadPartShopData())
		return 1;

	if (loadItemData())
		return 1;

	initialize();

	std::thread sThread(serverThread, this);
	sThread.detach();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	return managementserver.Start(this);
}
int32_t Server::Stop()
{
	if (running == false)
		return 1;

	for (auto& c : connections)
	{
		if (c && c->ClientSocket > 0)
		{
			initialize_connection(c);
			delete c;
		}
	}
	connections.clear();
	running = false;
	return 0;
}
int32_t Server::LoadConfig(const wchar_t* filename)
{
	bool malformed = false;
	std::wstring error = L"";
	std::ifstream inFile(filename);
	if (inFile.fail())
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Configuration file missing: %s. Default config is being created.", filename);
		std::ofstream outfile(filename);
		if (outfile.fail())
		{
			logger->Log(Logger::LOGTYPE_ERROR, L"Failure to create default configuration file: %s.", filename);
			return 1;
		}
		outfile << DEFAULT_JSON_CONFIG;
		outfile.close();
		inFile.open(filename);
	}
	if (!inFile.is_open())
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Loading configuration file: %s.", filename);
		return 1;
	}

	json in;

	try {
		inFile >> in;
	}
	catch (...) {
		logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error\n");
		inFile.close();
		return 1;
	}

	inFile.close();

	if (in.empty())
	{
		error = L"file is empty";
		malformed = true;
	}
	else
	{
		if (in["serverport"].is_number_integer())
		{
			try {
				serverPort = in["serverport"].get<unsigned short>();
				logger->Log(Logger::LOGTYPE_DEBUG, L"CONFIG: Server port is %u.", serverPort);
			}
			catch (...) {
				logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error\n");
				return 1;
			}
		}
		else
		{
			error = L"serverport value is not integer";
			malformed = true;
		}

		if (in["servername"].is_string())
		{
			try {
				serverName = in["servername"].get<std::string>();
				logger->Log(Logger::LOGTYPE_DEBUG, L"CONFIG: Server Name is %s.", toWide(serverName).c_str());
			}
			catch (...) {
				logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error\n");
				return 1;
			}
		}
		else
		{
			error = L"servername value is not string";
			malformed = true;
		}

		if (in["serverclientlimit"].is_number_integer())
		{
			try {
				serverClientLimit = in["serverclientlimit"].get<unsigned short>();
				logger->Log(Logger::LOGTYPE_DEBUG, L"CONFIG: Server Client Limit is %u.", serverClientLimit);
			}
			catch (...) {
				logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error\n");
				return 1;
			}
		}
		else
		{
			error = L"serverclientlimit value is not integer";
			malformed = true;
		}

		if (in["serverwelcomemessage"].is_string())
		{
			try {
				serverWelcomeMessage = in["serverwelcomemessage"].get<std::string>();
				logger->Log(Logger::LOGTYPE_DEBUG, L"CONFIG: Server Welcome Message is %s.", toWide(serverWelcomeMessage).c_str());
			}
			catch (...) {
				logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error\n");
				return 1;
			}
		}
		else
		{
			error = L"serverwelcomemessage value is not string";
			malformed = true;
		}

		if (in["managementserverport"].is_number_integer())
		{
			try {
				managementServerPort = in["managementserverport"].get<unsigned short>();
				logger->Log(Logger::LOGTYPE_DEBUG, L"CONFIG: Management Server port is %u.", managementServerPort);
			}
			catch (...) {
				logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error\n");
				return 1;
			}
		}
		else
		{
			error = L"managementserverport value is not integer";
			malformed = true;
		}

		if (in["managementserveraddress"].is_string())
		{
			try {
				managementServerAddress = in["managementserveraddress"].get<std::string>();
				logger->Log(Logger::LOGTYPE_DEBUG, L"CONFIG: Management Server Address is %s.", toWide(managementServerAddress).c_str());
			}
			catch (...) {
				logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error\n");
				return 1;
			}
		}
		else
		{
			error = L"managementserveraddress value is not string";
			malformed = true;
		}

		if (in["logpath"].is_string())
		{
			try {
				logger->setLogPath(in["logpath"].get<std::string>());
				logger->Log(Logger::LOGTYPE_DEBUG, L"CONFIG: Log path is %s.", logger->isLogPathSet() ? toWide(logger->getLogPath()).c_str() : L"EMPTY");
			}
			catch (...) {
				logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error\n");
				return 1;
			}
		}
		else
		{
			error = L"logpath value is not string";
			malformed = true;
		}
	}

	if (malformed)
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Configuration file: %s is malformed (%s).", filename, error.c_str());
		return 1;
	}
	else
		return 0;
}
std::wstring Server::toWide(std::string in)
{
	std::wstring temp(in.length(), L' ');
	copy(in.begin(), in.end(), temp.begin());
	return temp;
}
std::string Server::toNarrow(std::wstring in)
{
	std::string temp(in.length(), ' ');
	copy(in.begin(), in.end(), temp.begin());
	return temp;
}
void Server::CheckClientPackets(Client* client, uint32_t rcvcopy, uint8_t* tmprcv)
{
	if (rcvcopy >= 4)
	{
		client->inbuf.setArray(tmprcv, rcvcopy, 0x00);
		uint16_t size = client->inbuf.getSizeFromBuffer();
		client->inbuf.setSize(SWAP_SHORT(size) + 2);
		if (client->inbuf.getSize() > CLIENT_BUFFER_SIZE)
		{
			// Packet too big, disconnect the client.
			logger->Log(Logger::LOGTYPE_COMM, L"Client %s sent a invalid packet.", toWide((char*)client->IP_Address).c_str());
			client->Disconnect();
		}
		else
		{
			client->ProcessPacket();
		}
	}
}
void Server::initialize_connection(Client* connect)
{
	if (connect->ClientSocket >= 0)
	{
		saveClientData(connect);
		std::wstring ip = toWide((char*)connect->IP_Address);
		logger->Log(Logger::LOGTYPE_COMM, L"Client %s has disconnected.", ip.c_str());
		logger->Log(Logger::LOGTYPE_COMM, L"Player count: %u", playerCount() - 1);
		if(connect->course != nullptr) connect->course->removeClient(connect);
		closesocket(connect->ClientSocket);
	}
	removeClient(connect);

	char titleBuf[0x100];
	snprintf(titleBuf, sizeof(titleBuf), "SBOL Battle Server v%s. Clients: %u", logger->toNarrow(VERSION_STRING).c_str(), playerCount() - 1);
	SetConsoleTitleA(titleBuf);
}
void Server::removeClient(Client* connect)
{
	for (auto& c : connections)
	{
		if (c && c->ClientSocket == connect->ClientSocket)
		{
			delete c;
			c = nullptr;
			break;
		}
	}
}
void Server::organizeClients()
{
	// Removes initialised clients
	uint32_t connectioncount = connections.size();
	for (uint32_t i = 0; i < connectioncount;)
	{
		if (connections[i] == nullptr)
		{
			connections.erase(connections.begin() + i);
			connectioncount--;
		}
		else
			i++;
	}
}
void Server::setConnectedToManagementServer()
{
	connectedToManagement = true;
}
void Server::clearConnectedToManagementServer()
{
	connectedToManagement = false;
}
void Server::addAuthenticatedClient(uint32_t license)
{
	std::vector<uint32_t>::iterator it = find(authenticatedClients.begin(), authenticatedClients.end(), license);
	if (it == authenticatedClients.end()) authenticatedClients.push_back(license);
}
void Server::removeAuthenticatedClient(uint32_t license)
{
	std::vector<uint32_t>::iterator it = find(authenticatedClients.begin(), authenticatedClients.end(), license);
	if (it != authenticatedClients.end())
	{
		int32_t index = distance(authenticatedClients.begin(), it);
		authenticatedClients.erase(authenticatedClients.begin() + index);
	}
}
bool Server::clientIsAuthenticated(uint32_t license)
{
	std::vector<uint32_t>::iterator it = find(authenticatedClients.begin(), authenticatedClients.end(), license);
	if (it != authenticatedClients.end()) return true;
	else return false;
}
void Server::disconnectLoggedInUser(uint32_t license)
{
	Client* foundUser = findUser(license);
	if (foundUser != nullptr)
	{
		foundUser->SendChatMessage(Client::CHATTYPE::CHATTYPE_NOTIFICATION, std::string("NULL"), std::string("Your account has been logged in so have been logged out."), license);
		foundUser->Disconnect();
	}
}
void Server::disconnectAllUsers(uint32_t exclude)
{
	for (auto& c : connections)
	{
		if (c->ClientSocket >= 0 && c->driverslicense != exclude) c->Disconnect();
	}
}
Client* Server::findUser(std::string& in)
{
	for (auto& c : connections)
	{
		if (c->handle.compare(in) == 0) return c;
	}
	return nullptr;
}
Client* Server::findUser(uint32_t in)
{
	for (auto& c : connections)
	{
		if (c->driverslicense == in) return c;
	}
	return nullptr;
}
void Server::saveClientData(Client* client)
{
	if (client->canSave == true && client->driverslicense)
	{
		managementserver.outbuf.clearBuffer();
		managementserver.outbuf.setSize(0x06);
		managementserver.outbuf.setOffset(0x06);
		managementserver.outbuf.setType(0x0002);
		managementserver.outbuf.setSubType(0x0000);
		managementserver.outbuf.append<uint32_t>(client->driverslicense);
		managementserver.outbuf.setString(client->handle, managementserver.outbuf.getOffset());
		managementserver.outbuf.addOffset(0x10);
		managementserver.outbuf.addSize(0x10);
		managementserver.outbuf.append<int64_t>(client->careerdata.CP);
		managementserver.outbuf.append<uint8_t>(client->careerdata.level);
		managementserver.outbuf.append<uint32_t>(client->careerdata.experiencePoints);
		managementserver.outbuf.append<uint32_t>(client->careerdata.playerWin);
		managementserver.outbuf.append<uint32_t>(client->careerdata.playerLose);
		managementserver.outbuf.append<uint32_t>(client->careerdata.rivalWin);
		managementserver.outbuf.append<uint32_t>(client->careerdata.rivalLose);
		managementserver.outbuf.appendArray((uint8_t*)&client->careerdata.rivalStatus[0], sizeof(client->careerdata.rivalStatus));
		managementserver.outbuf.append<uint8_t>(static_cast<uint8_t>(client->getActiveCar()));
		managementserver.outbuf.append<uint8_t>(client->privileges);
		managementserver.outbuf.append<uint8_t>(static_cast<uint8_t>(client->notBeginner));
		managementserver.outbuf.append<uint32_t>(client->teamdata.teamID);

		managementserver.outbuf.append<uint8_t>(client->garagedata.garageCount);
		for(uint8_t i = 0; i < client->garagedata.garageCount; i++) managementserver.outbuf.append<uint8_t>(client->garagedata.garageType[i]);

		int8_t carCount = client->getCarCount();
		managementserver.outbuf.append<uint8_t>(carCount);
		for (uint32_t i = 0; i < client->garagedata.car.size(); i++)
		{
			if (client->garagedata.car[i].carID != 0xFFFFFFFF)
			{
				managementserver.outbuf.append<uint32_t>(client->garagedata.car[i].carID);
				managementserver.outbuf.append<float>(client->garagedata.car[i].KMs);
				managementserver.outbuf.appendArray((uint8_t*)&client->garagedata.car[i].carMods, sizeof(client->garagedata.car[i].carMods));
				managementserver.outbuf.appendArray((uint8_t*)&client->garagedata.car[i].parts, sizeof(client->garagedata.car[i].parts));
				managementserver.outbuf.append<uint32_t>(client->garagedata.car[i].engineCondition);
				managementserver.outbuf.append<uint32_t>(i); // Bay
			}
		}

		managementserver.outbuf.append<uint32_t>(client->itembox.size());
		if (client->itembox.size() > 0)
			managementserver.outbuf.appendArray((uint8_t*)client->itembox.data(), sizeof(ITEMBOX_ITEM) * client->itembox.size());

		managementserver.outbuf.appendArray(&client->sign[0], sizeof(client->sign));

		managementserver.Send();
	}
}
int32_t Server::tcp_sock_open(struct in_addr ip, uint16_t port)
{
	int32_t fd, bufsize, turn_on_option_flag = 1, rcSockopt, keepAlive = 0;
	struct sockaddr_in sa;

	ZeroMemory((void *)&sa, sizeof(sa));

	fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (fd < 0) {
		return -1;
	}

	sa.sin_family = AF_INET;
	CopyMemory((void *)&sa.sin_addr, (void *)&ip, sizeof(struct in_addr));
	sa.sin_port = htons(port);

	/* Reuse port */

	rcSockopt = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&turn_on_option_flag, sizeof(turn_on_option_flag));

	/* Increase the TCP/IP buffer size */
	bufsize = TCP_BUFFER_SIZE;
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&bufsize, sizeof(bufsize));
	setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&bufsize, sizeof(bufsize));

	// As per client
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepAlive, sizeof(keepAlive));

	//struct tcp_keepalive alive = {
	//	true,
	//	7200000,
	//	6000
	//};
	//
	//DWORD dwBytesRet = 0;
	//
	//if (WSAIoctl(fd, SIO_KEEPALIVE_VALS, &alive, sizeof(alive), NULL, 0, &dwBytesRet, NULL, NULL) == SOCKET_ERROR) return -3;

	/* bind() the socket to the interface */
	if (::bind(fd, (struct sockaddr *)&sa, sizeof(struct sockaddr)) < 0) {
		return -2;
	}
	return fd;
}
int32_t Server::tcp_set_nonblocking(int32_t fd)
{
	u_long flags = 1;
	if (ioctlsocket(fd, FIONBIO, &flags))
		return 1;
	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&flags, sizeof(flags));
}
int32_t Server::tcp_listen(int32_t sockfd)
{
	if (listen(sockfd, 10) < 0)
	{
		return 1;
	}
	tcp_set_nonblocking(sockfd);
	return 0;
}
int32_t Server::tcp_accept(int32_t sockfd, struct sockaddr *client_addr, int32_t *addr_len)
{
	int32_t fd, bufsize;

	if ((fd = accept(sockfd, client_addr, addr_len)) != INVALID_SOCKET)
	{
		bufsize = TCP_BUFFER_SIZE;
		setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&bufsize, sizeof(bufsize));
		setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&bufsize, sizeof(bufsize));
		tcp_set_nonblocking(fd);
		return fd;
	}
	else
	{
		logger->Log(Logger::LOGTYPE_COMM, L"Could not accept connection.");
		return -1;
	}
}
bool Server::freeConnection()
{
	if (connections.size() < serverClientLimit) return true;
	return false;
}
bool Server::InitServerSocket()
{
	struct in_addr server_in;
	WSADATA winsock_data;
	WSAStartup(MAKEWORD(2, 2), &winsock_data);

	server_in.s_addr = INADDR_ANY;
	ServerSocket = tcp_sock_open(server_in, serverPort);

	if (ServerSocket < 0)
	{
		switch (ServerSocket)
		{
		case -1:
			logger->Log(Logger::LOGTYPE_ERROR, L"Could not create socket.");
			break;
		case -2:
			logger->Log(Logger::LOGTYPE_ERROR, L"Could not bind to port %u.", serverPort);
			break;
		default:
			break;
		}
		return false;
	}

	if (tcp_listen(ServerSocket))
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Could not listen to port %u.", serverPort);
		return false;
	}
	else
		logger->Log(Logger::LOGTYPE_COMM, L"Listening on port %u.", serverPort);

	logger->Log(Logger::LOGTYPE_SERVER, L"Server Started.");
	return true;
}
bool Server::AcceptConnection()
{
	struct sockaddr_in listen_in;
	int32_t listen_length;
	
	listen_length = sizeof(listen_in);

	while ((TempSocket = tcp_accept(ServerSocket, (struct sockaddr*) &listen_in, &listen_length)) != -1)
	{
		if (freeConnection())
		{
			Client* c = new Client();
			c->initialize();
			c->server = this;
			c->logger = logger;
			connections.push_back(c);
			c->ClientSocket = TempSocket;
			InetNtopA(AF_INET, &listen_in.sin_addr, (char*)&c->IP_Address[0], 16);
			*(uint32_t*)&c->ipaddr = *(uint32_t*)&listen_in.sin_addr;
			std::wstring ip = toWide((char*)c->IP_Address);
			logger->Log(Logger::LOGTYPE_COMM, L"Accepted Client connection from %s:%u", ip.c_str(), listen_in.sin_port);
			logger->Log(Logger::LOGTYPE_COMM, L"Player count: %u", playerCount());
			c->sendWelcome = time(NULL) + SEND_WELCOME_TIME;
			c->packetEnable(0x00);
			c->packetEnable(0x01);

			char titleBuf[0x100];
			snprintf(titleBuf, sizeof(titleBuf), "SBOL Battle Server v%s. Clients: %u", logger->toNarrow(VERSION_STRING).c_str(), playerCount());
			SetConsoleTitleA(titleBuf);

			return true;
		}
	}
	return false;
}
bool Server::Send(Client * client)
{
	int32_t sent_len, wserror, max_send;
	uint8_t sendbuf[CLIENT_BUFFER_SIZE];
	SEND_QUEUE entry;

	if (client->snddata > 0 || client->messagesInSendQueue())
	{
		if (client->snddata <= 0)
		{
			uint32_t queueSize = client->messagesInSendQueue();
			uint32_t sendCount = PACKETSEND_LIMIT;
			if (queueSize > 0)
			{
#ifdef _DEBUG
				if (queueSize > 1)
				{
					DEBUGSTR("Queue Size: %u\n", queueSize);
				}
#endif
				while (client->messagesInSendQueue() && sendCount--)
				{
					uint16_t packetSize;
					entry = client->getFromSendQueue();
					packetSize = SWAP_SHORT(*(uint16_t*)&entry.sndbuf[0]) + 2;
					if (client->snddata + packetSize < CLIENT_BUFFER_SIZE)
					{
						CopyMemory((char*)&client->sndbuf[client->snddata], entry.sndbuf, packetSize);
						client->snddata += packetSize;
					}
					else
					{
						// Ran out of buffer push the packet back into the queue
						client->addToSendQueue(&entry);
						break;
					}
				}
			}
		}

		if (client->snddata > CLIENT_BUFFER_SIZE)
			max_send = CLIENT_BUFFER_SIZE;
		else
			max_send = client->snddata;

		if((sent_len = send(client->ClientSocket, (char *)&client->sndbuf[0], max_send, 0)) == SOCKET_ERROR)
		{
			wserror = WSAGetLastError();
			if (wserror != WSAEWOULDBLOCK)
			{
				if (wserror != 0)
					logger->Log(Logger::LOGTYPE_COMM, L"Could not write data to client %s. Socket Error %X08", toWide((char*)client->IP_Address).c_str(), wserror);
				client->Disconnect();
			}
		}
		else
		{
			client->snddata -= sent_len;

			if (client->snddata > 0)
			{
				CopyMemory(&sendbuf[0], &client->sndbuf[sent_len], client->snddata);
				CopyMemory(&client->sndbuf[0], &sendbuf[0], client->snddata);
			}
		}
		return true;
	}
	return false;
}
bool Server::Recv(Client * client)
{
	int32_t  wserror, recv_len, max_read;
	max_read = CLIENT_BUFFER_SIZE - client->rcvread;

	if ((recv_len = recv(client->ClientSocket, (char *)&client->rcvbuf[client->rcvread], max_read, 0)) == SOCKET_ERROR)
	{
		wserror = WSAGetLastError();
		if (wserror != WSAEWOULDBLOCK)
		{
			if (wserror != 0)
				logger->Log(Logger::LOGTYPE_COMM, L"Could not read data from client %s. Socket Error %d", toWide((char*)client->IP_Address).c_str(), wserror);
			client->Disconnect();
		}
	}
	else
	{
	_processpacket:
		if (recv_len == 0)
		{
			wserror = WSAGetLastError();
			if (wserror == WSAECONNRESET)
				return false;
		}
		ProcessRecv(client, recv_len);
		return true;
	}
	if (client->rcvread && recv_len == -1)
	{
		recv_len = 0;
		goto _processpacket;
	}
	return false;
}
bool Server::ProcessRecv(Client * client, int32_t len)
{
	uint8_t recvbuf[CLIENT_BUFFER_SIZE];

	if (!client || len < 0)
		return false;

	client->rcvread += len;
	if ((client->hassize == false || client->packetsize == 0) && client->rcvread >= 2)
	{
		client->packetsize = SWAP_SHORT(*(uint16_t*)&client->rcvbuf[0]) + 2;
		client->hassize = true;
	}
	if (client->hassize == true && client->rcvread >= client->packetsize && client->packetsize >= 4)
	{
		CopyMemory(&recvbuf[0], &client->rcvbuf[0], client->packetsize);
		CheckClientPackets(client, client->packetsize, recvbuf);
		CopyMemory(&recvbuf[0], &client->rcvbuf[client->packetsize], CLIENT_BUFFER_SIZE - client->packetsize);
		CopyMemory(&client->rcvbuf[0], &recvbuf[0], CLIENT_BUFFER_SIZE - client->packetsize);
		client->hassize = false;
		client->rcvread -= client->packetsize;
		client->packetsize = 0;
		return true;
	}
	return false;
}
bool Server::RecvToProcess(Client * client)
{
	if (!client || client->todc == true)
		return false;
	return client->rcvread ? true : false;
}
bool Server::ClientChecks(Client * client)
{
	if (!client)
		return false;

	if(client->todc == true)
		goto _disconnectClient;

	if (client->sendWelcome > 0 && client->sendWelcome < time(NULL) && client->isClient == false)
	{
		client->Disconnect();
		goto _disconnectClient;
	}
	// Sending ping when the client stops sending packets doesn't resolve the issue.
	//if (client->timeoutCount > 0 && client->timeoutCount - (CLIENT_TIMEOUT / 2) < time(NULL))
	//{
	//	client->SendPing();
	//}
	if (client->timeoutCount > 0 && client->timeoutCount < time(NULL))
	{
		client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has timed out so was disconnected.",
			client->logger->toWide(client->handle).c_str(),
			client->driverslicense,
			client->logger->toWide((char*)&client->IP_Address).c_str()
		);
		client->Disconnect();
		goto _disconnectClient;
	}
	if (client->lastSave < time(NULL))
	{
		client->lastSave = time(NULL) + AUTOSAVE_TIME;
		saveClientData(client);
	}
	if (client->packetResend != 0 && client->packetResend  < time(NULL))
	{
		client->packetResend = 0;
		client->Send();
	}
	if (client->battle.timeout != 0 && client->battle.timeout < time(NULL) && (client->battle.challenger != nullptr || client->battle.isNPC == true))
	{
		client->battle.timeout = 0;
		client->battle.status = Client::BATTLESTATUS::BS_NOT_IN_BATTLE;
		client->battle.challenger = nullptr;
		client->battle.isNPC = false;
		client->SendBattleAbort(0);
	}
_disconnectClient:
	if (client->todc)
	{
		initialize_connection(client);
		return false;
	}
	return true;
}
int32_t Server::VerifyConfig()
{
	if (LoadConfig(CONFIG_FILENAME))
		return 1;

	if (serverName == "" || serverPort == 0 || managementServerPort == 0 || managementServerAddress == "")
		return 1;

	return 0;
}
void Server::serverThread(void* parg)
{
	Server* server = (Server*)parg;
	int32_t selectcount;
	fd_set ReadFDs, WriteFDs;
	struct timeval select_timeout = {
		0,
		1
	};

	if (!server)
		return;

	server->running = server->InitServerSocket();
	
	// TODO: Should only get here if connected to a DB server

	while (server->running)
	{
		// Zero descriptors.

		FD_ZERO(&ReadFDs);
		FD_ZERO(&WriteFDs);

		// Read/write data from clients.
		for (auto& c : server->connections)
		{
			if ((c) && (c->ClientSocket >= 0))
			{
				if (!c->todc)
					FD_SET(c->ClientSocket, &ReadFDs);
				if (c->snddata > 0 || c->messagesInSendQueue())
					FD_SET(c->ClientSocket, &WriteFDs);
			}
		}

		// Set Read on server socket
		FD_SET(server->ServerSocket, &ReadFDs);

		selectcount = select(0, &ReadFDs, &WriteFDs, NULL, &select_timeout);

		// Accept incomming connection
		if (FD_ISSET(server->ServerSocket, &ReadFDs))
		{
			FD_CLR(server->ServerSocket, &ReadFDs);
			server->AcceptConnection();
		}

		for (auto& c : server->connections)
		{
			if ((c) && (c->ClientSocket >= 0))
			{
				// Send
				if (FD_ISSET(c->ClientSocket, &WriteFDs))
				{
					FD_CLR(c->ClientSocket, &WriteFDs);
					server->Send(c);
				}

				// Receive - Always be receiving
				{
					server->Recv(c);
				}
				// Check if client has or needs to be disconnected
				server->ClientChecks(c);
			}
		}
		// Remove initialized clients from connections
		server->organizeClients();
		
		if (server->managementserver.messagesInQueue())
		{
			MESSAGE_QUEUE message = server->managementserver.getTopFromMessageQueue();
			for (auto& c : server->connections)
			{
				if (c->ClientSocket == message.socket)
				{
					c->serverbuf.clearBuffer();
					c->serverbuf.appendArray(&message.buffer[0x00], *(uint16_t*)&message.buffer[0x00] + 2);
					c->ProcessManagementPacket();
					break;
				}
			}
		}

		// Course packets
		for (uint32_t i = 0; i < COURSE_DIMENSIONS; i++)
		{
			for (uint32_t j = 0; j < server->courses[i].size(); j++)
			{
				server->courses[i][j]->sendRacerPositions();
			}
		}

		if (selectcount == 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	server->logger->Log(Logger::LOGTYPE_SERVER, L"Server Stopped.");
	server->running = false;
}
std::vector<std::string> Server::split(std::string& in, std::string& delimit)
{
	std::vector<std::string> out;
	uint32_t offset = 0;
	uint32_t found = 0;
	while (in.find(delimit, offset) != std::string::npos)
	{
		found = in.find(delimit, offset);
		out.push_back(in.substr(offset, found - offset));
		if (in.find(delimit, found + 1) == std::string::npos) break;
		offset += in.find(delimit, offset) + 1;
	}
	if (found != 0 && found < in.length())
	{
		found++;
		out.push_back(in.substr(found, (in.length() - found)));
	}
	if (offset == 0 && found == 0 && in.length() > 0) out.push_back(in);
	return out;
}
uint32_t Server::getNextClientID()
{
	return currentClientID++;
}
bool Server::isValidCar(uint16_t carID)
{
	switch (carID)
	{
		case AE86_L_3_1985:
		case AE86_T_3_1985:
		case AE86_L_2_1985:
		case AE86_T_2_1985:
		case MR2_GT_1997:
		case SUPRA_RZ_1993:
		case SUPRA_RZ_1997:
		case SUPRA_SZR_1997:
		case ALTEZZA_R200_Z_1997:
		case CHASER_TOURER_V_1996:
		case CHASER_TOURER_V_1998:
		case MARKII_TOURER_V_1998:
		case SILVIA_K_1988:
		case SILVIA_Q_1988:
		case SILVIA_K_1991:
		case SILVIA_K_1993:
		case SILVIA_K_1996:
		case SILVIA_SPECR_1999:
		case SILVIA_SPECS_1999:
		case S180SX_TYPEIII_1994:
		case SKYLINE_GTR_VSPECII_1994:
		case SKYLINE_GTR_1989:
		case SKYLINE_GTST_TYPEM_1991:
		case SKYLINE_GTR_VSPEC_1997:
		case SKYLINE_25GT_TURBO_1998:
		case SKYLINE_25GT_TURBO_2000:
		case FAIRLADY_Z_S_TT_1998:
		case FAIRLADY_Z_TBAR_1998:
		case CEDRIC_BROUGHAM_VIP_1997:
		case GLORIA_GRANTURISMO_ULTIMA_1997:
		case LANCER_GSR_EVOIII_1995:
		case LANCER_GSR_EVOIV_1996:
		case SAVANNA_RX7_INFIIII_1989:
		case RX7_TYPERS_1999:
		case RX7_TYPERZ_1998:
		case IMPREZA_WRX_STI_VERIV_1997:
		case IMPREZA_WRX_STI_VERV_1998:
		case IMPREZA_WRX_STI_VERVI_1999:
		case FAIRLADY_Z_240ZG_1972:
		case MX5_ROADSTER_RS_1998:
		case MX5_ROADSTER_RS_2000:
		case CELICA_GTFOUR_1996:
		case CELICA_SSIII_1996:
		case GTO_TT_1993:
		case FTO_GP_VERR_1997:
		case PULSAR_SERIE_VZN1_VER2_1998:
		case LEGACY_TOURING_WAGON_GTB_1998:
		case CELICA_SSII_SSPACK_1999:
		case MRS_1999:
		case CEDRIC_300VIP_1999:
		case GLORIA_300_ULTIMA_1999:
		case AZ1_1992:
		case CAPPUCCINO_1991:
		case CEFIRO_CRUISING_1990:
		case STARION_GSRVR_1988:
		case SAVANNA_RX3_GT_1973:
		case SKYLINE_2000GTR_1971:
		case SAVANNA_RX7_GTX_1983:
		case ECLIPSE_GST_1999:
		case BB15Z_2000:
		case SUPRA_30GT_T_1989:
		case SUPRA_25GT_TT_1991:
		case LANCER_GSR_EVOVII_2001:
		case IMPREZA_WRX_NB_2000:
		case IMPREZA_WRX_STI_2000:
		case LANTIS_COUPE_TYPER_1993:
		//case 35:
		//case 141:
			return true;
		default:
			return false;
	}
	return false;
}
bool Server::isValidItem(uint16_t itemID)
{
	for (int32_t i = 0; i < itemData.count; i++)
	{
		if (itemData.data[i].itemID == itemID) return true;
	}
	return false;
}
uint32_t Server::playerCount()
{
	//int32_t count = 0;
	//for (uint32_t i = 0; i < connections.size(); i++)
	//{
	//	if (connections[i]->plySockfd >= 0) count++;
	//}
	//return count;
	return connections.size();
}
void Server::LoadRivalFile()
{
	std::vector<std::string> rivalfiles = GetRivalFiles();

	for (const auto& rivalfile : rivalfiles) {
		std::ifstream inFile(rivalfile);
		if (!inFile.is_open())
		{
			logger->Log(Logger::LOGTYPE_ERROR, L"Error Loading rival file: %s.", rivalfile.c_str());
		}
		else
		{
			json in;

			try {
				inFile >> in;
			}
			catch (json::exception ex) {
				logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error %s on file %s.", logger->toWide(ex.what()).c_str(), logger->toWide(rivalfile).c_str());
			}
			catch (...) {
				logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error");
				inFile.close();
			}
			inFile.close();
			if (!in.empty())
			{
				RIVALDATA rival;
				ZeroMemory(&rival, sizeof(RIVALDATA));
				try {
					rival.teamData.teamID = in["id"].get<uint32_t>();
					rival.customTeam = in["customteam"].get<bool>();
					memcpy(&rival.teamData.name[0], in["teamname"].get<std::string>().c_str(), min(16, in["teamname"].get<std::string>().length()));
					rival.carMods.stickers = in["stickers"].get<uint32_t>();

					size_t teamSize = in["members"].size();
					rival.teamData.memberCount = static_cast<uint8_t>(teamSize);
					for (auto& member : in["members"])
					{
						rival.teamData.memberID = member["id"].get<uint32_t>();
						rival.rivalID = (rival.teamData.teamID * 8) + (rival.teamData.memberID % 8);

						rival.level = 1;
						ZeroMemory(&rival.name[0], sizeof(rival.name));
						memcpy(&rival.name[0], member["name"].get<std::string>().c_str(), min(16, member["name"].get<std::string>().length()));
						rival.carID = member["carid"].get<uint32_t>();
						rival.leader = member["leader"].get<bool>();
						rival.cp = member["cp"].get<uint32_t>();
						rival.routeTable = member["routetable"].get<uint32_t>();
						rival.rewardTable = member["rewardtable"].get<uint32_t>();
						rival.commonTable = member["commontable"].get<uint32_t>();
						rival.rewardChance = member["rewardchance"].get<float>();
						uint32_t colour1 = strtoul(member["carmods"]["carcolour1"].get<std::string>().c_str(), NULL, 16);
						uint32_t colour2 = strtoul(member["carmods"]["carcolour2"].get<std::string>().c_str(), NULL, 16);
						rival.carMods.carcolour.R1 = TO_FLOAT_COLOUR(GET_R_COLOUR(colour1));
						rival.carMods.carcolour.G1 = TO_FLOAT_COLOUR(GET_G_COLOUR(colour1));
						rival.carMods.carcolour.B1 = TO_FLOAT_COLOUR(GET_B_COLOUR(colour1));
						rival.carMods.carcolour.R2 = TO_FLOAT_COLOUR(GET_R_COLOUR(colour2));
						rival.carMods.carcolour.G2 = TO_FLOAT_COLOUR(GET_G_COLOUR(colour2));
						rival.carMods.carcolour.B2 = TO_FLOAT_COLOUR(GET_B_COLOUR(colour2));
						rival.carMods.engine = member["carmods"]["engine"].get<uint32_t>();
						rival.carMods.muffler = member["carmods"]["muffler"].get<uint32_t>();
						rival.carMods.transmission = member["carmods"]["transmission"].get<uint32_t>();
						rival.carMods.differential = member["carmods"]["differential"].get<uint32_t>();
						rival.carMods.tyrebrakes = member["carmods"]["tyreandbrakes"].get<uint32_t>();
						rival.carMods.suspension = member["carmods"]["suspension"].get<uint32_t>();
						rival.carMods.body = member["carmods"]["body"].get<uint32_t>();
						rival.carMods.overfenders = member["carmods"]["overfenders"].get<uint32_t>();
						rival.carMods.frontbumper = member["carmods"]["frontbumper"].get<uint32_t>();
						rival.carMods.bonnet = member["carmods"]["bonnet"].get<uint32_t>();
						rival.carMods.mirrors = member["carmods"]["mirrors"].get<uint32_t>();
						rival.carMods.sideskirts = member["carmods"]["sideskirts"].get<uint32_t>();
						rival.carMods.rearbumper = member["carmods"]["rearbumper"].get<uint32_t>();
						rival.carMods.rearspoiler = member["carmods"]["rearspoiler"].get<uint32_t>();
						rival.carMods.grill = member["carmods"]["grill"].get<uint32_t>();
						rival.carMods.lights = member["carmods"]["lights"].get<uint32_t>();
						rival.carMods.wheels = member["carmods"]["wheels"].get<uint32_t>();
						rival.carMods.colourwheels = member["carmods"]["colourwheels"].get<uint32_t>();
						colour1 = strtoul(member["carmods"]["wheelcolour1"].get<std::string>().c_str(), NULL, 16);
						colour2 = strtoul(member["carmods"]["wheelcolour2"].get<std::string>().c_str(), NULL, 16);
						rival.carMods.wheelcolour.R1 = TO_FLOAT_COLOUR(GET_R_COLOUR(colour1));
						rival.carMods.wheelcolour.G1 = TO_FLOAT_COLOUR(GET_G_COLOUR(colour1));
						rival.carMods.wheelcolour.B1 = TO_FLOAT_COLOUR(GET_B_COLOUR(colour1));
						rival.carMods.wheelcolour.R2 = TO_FLOAT_COLOUR(GET_R_COLOUR(colour2));
						rival.carMods.wheelcolour.G2 = TO_FLOAT_COLOUR(GET_G_COLOUR(colour2));
						rival.carMods.wheelcolour.B2 = TO_FLOAT_COLOUR(GET_B_COLOUR(colour2));
						rival.carSettings.handle = member["carsettings"]["handle"].get<int8_t>();
						rival.carSettings.acceleration = member["carsettings"]["acceleration"].get<int8_t>();
						rival.carSettings.brake = member["carsettings"]["brake"].get<int8_t>();
						rival.carSettings.brakeBalance = member["carsettings"]["brakebalance"].get<int8_t>();
						rival.carSettings.springRateFront = member["carsettings"]["springratefront"].get<int8_t>();
						rival.carSettings.springRateRear = member["carsettings"]["springraterear"].get<int8_t>();
						rival.carSettings.damperRateFront = member["carsettings"]["damperratefront"].get<int8_t>();
						rival.carSettings.damperRateRear = member["carsettings"]["damperraterear"].get<int8_t>();
						rival.carSettings.gear1 = member["carsettings"]["gear1"].get<int8_t>();
						rival.carSettings.gear2 = member["carsettings"]["gear2"].get<int8_t>();
						rival.carSettings.gear3 = member["carsettings"]["gear3"].get<int8_t>();
						rival.carSettings.gear4 = member["carsettings"]["gear4"].get<int8_t>();
						rival.carSettings.gear5 = member["carsettings"]["gear5"].get<int8_t>();
						rival.carSettings.gear6 = member["carsettings"]["gear6"].get<int8_t>();
						rival.carSettings.rideHeightFront = member["carsettings"]["rideheightfront"].get<int8_t>();
						rival.carSettings.rideHeightRear = member["carsettings"]["rideheightrear"].get<int8_t>();
						rival.carSettings.finalDrive = member["carsettings"]["finaldrive"].get<int8_t>();
						rival.carSettings.boostPressure = member["carsettings"]["boostpressure"].get<int8_t>();
						for (uint32_t i = 0; i < 11; i++)
						{
							rival.difficulty.f[i] = member["difficulty"]["floats"][i].get<float>();
						}
						for (uint32_t i = 0; i < 12; i++)
						{
							rival.difficulty.b[i] = member["difficulty"]["bytes"][i].get<uint8_t>();
						}
						rival.requirements.level = member["requirements"]["level"].get<int8_t>();
						rival.requirements.wins = member["requirements"]["wins"].get<int32_t>();
						rival.requirements.kms = member["requirements"]["kms"].get<int32_t>();
						rival.requirements.day = member["requirements"]["day"].get<int8_t>();
						rival.requirements.time = member["requirements"]["time"].get<int32_t>();
						for (uint32_t i = 0; i < member["requirements"]["previousrivals"].size(); i++)
						{
							rival.requirements.previousRivals[i] = member["requirements"]["previousrivals"][i].get<int16_t>();
						}
						rival.requirements.time = member["requirements"]["level"].get<int8_t>();
						rival.requirements.driveTrain = member["requirements"]["drivetrain"].get<int8_t>();
						rival.requirements.engineCylinders = member["requirements"]["enginecylinders"].get<int8_t>();
						rival.requirements.aspiration = member["requirements"]["aspiration"].get<int8_t>();

						rivalData.push_back(rival);
					}
#ifdef _DEBUG
					DEBUGSTR("Loaded rival file: %s\n", rivalfile.c_str());
#endif
				}
				catch (json::exception ex) {
					logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error %s on file %s.", logger->toWide(ex.what()).c_str(), logger->toWide(rivalfile).c_str());
				}
				catch (...) {
					logger->Log(Logger::LOGTYPE_ERROR, L"JSON Parse Error on file %s.", logger->toWide(rivalfile).c_str());
				}
			}
		}
	}
}
RIVALDATA * Server::GetRivalData(int32_t RivalID, Client* client)
{
	if (RivalID == -1)
	{	// Set rival based on rules and progress of client
		if (!client) return nullptr;
	}
	else
	{	// Set specific rival
		for (auto& rival : rivalData)
		{
			if (rival.rivalID == RivalID)
				return &rival;
		}
	}
	return nullptr;
}
void Server::SendChatMessage(CHATTYPE type, std::string& handle, std::string& message)
{
	PACKET chatBuf;
	if (handle.empty())
		handle = "SERVER";
	chatBuf.clearBuffer();
	chatBuf.setSize(0x06);
	chatBuf.setOffset(0x06);
	chatBuf.setType(0x600);
	chatBuf.setSubType(0x600 + type);
	switch (type)
	{
	case CHATTYPE::CHATTYPE_NORMAL:
		chatBuf.appendString(handle, 0x10);
		break;
	case CHATTYPE::CHATTYPE_ADMIN:
		chatBuf.appendString(handle, 0x10);
		break;
	case CHATTYPE::CHATTYPE_ANNOUNCE:
		chatBuf.appendString(std::string("ANNOUNCEMENT"), 0x10);
		break;
	case CHATTYPE::CHATTYPE_NOTIFICATION:
		chatBuf.appendString(std::string("NOTIFICATION"), 0x10);
		break;
	case CHATTYPE::CHATTYPE_EVENT:
		chatBuf.appendString(std::string("EVENT"), 0x10);
		break;
	default:
		chatBuf.appendString(handle, 0x10);
		break;
	}
	chatBuf.appendString(message, 0x4E);
	SendToAll(&chatBuf);
}
void Server::SendOfflineChatMessage(CHATTYPE type, std::string& fromHandle, std::string& toHandle, std::string& message)
{
	managementserver.outbuf.clearBuffer();
	managementserver.outbuf.setSize(0x06);
	managementserver.outbuf.setOffset(0x06);
	managementserver.outbuf.setType(0x0004);
	managementserver.outbuf.setSubType(0x0000);
	managementserver.outbuf.append<uint8_t>(static_cast<uint8_t>(type));
	managementserver.outbuf.appendString(fromHandle, 0x10);
	managementserver.outbuf.appendString(toHandle, 0x10);
	managementserver.outbuf.appendString(message, 0x4E);
	managementserver.Send();
}
void Server::SendTeamChatMessage(std::string& fromHandle, uint32_t teamID, std::string& message)
{
	managementserver.outbuf.clearBuffer();
	managementserver.outbuf.setSize(0x06);
	managementserver.outbuf.setOffset(0x06);
	managementserver.outbuf.setType(0x0004);
	managementserver.outbuf.setSubType(0x0002);
	managementserver.outbuf.appendString(fromHandle, 0x10);
	managementserver.outbuf.appendString(message, 0x4E);
	managementserver.outbuf.append<uint32_t>(teamID);
	managementserver.Send();
}
void Server::SendInformMessage(std::string& toHandle, std::string& message, uint32_t colour)
{
	managementserver.outbuf.clearBuffer();
	managementserver.outbuf.setSize(0x06);
	managementserver.outbuf.setOffset(0x06);
	managementserver.outbuf.setType(0x0004);
	managementserver.outbuf.setSubType(0x0001);
	managementserver.outbuf.appendString(toHandle, 0x10);
	managementserver.outbuf.appendString(message, 0x4E);
	managementserver.outbuf.append<uint32_t>(colour);
	managementserver.Send();
}
void Server::SendAnnounceMessage(std::string& message, uint32_t colour)
{
	PACKET chatBuf;
	chatBuf.clearBuffer();
	chatBuf.setSize(0x06);
	chatBuf.setOffset(0x06);
	chatBuf.setType(0x600);
	chatBuf.setSubType(0x6F0);
	chatBuf.appendString(message, 0x4E);
	chatBuf.append<uint8_t>(static_cast<uint8_t>(GetRValue(colour)));
	chatBuf.append<uint8_t>(static_cast<uint8_t>(GetGValue(colour)));
	chatBuf.append<uint8_t>(static_cast<uint8_t>(GetBValue(colour)));
	SendToAll(&chatBuf);
}
void Server::SendToAll(PACKET* src)
{
	for (uint32_t i = 0; i < connections.size(); i++)
	{
		if (connections[i]->ClientSocket >= 0) connections[i]->Send(src);
	}
}
int32_t Server::loadPDat()
{
	std::string filename = "data\\P.DAT";
	std::ifstream inFile(filename, std::ios::in | std::ios::binary);
	if (!inFile.is_open())
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Loading parts file: %s.", toWide(filename).c_str());
		return 1;
	}
	size_t fileSize = 0;
	std::streampos begin;
	begin = inFile.tellg();
	inFile.seekg(0, std::ios::end);
	fileSize = (size_t)(inFile.tellg() - begin);
	if (fileSize < sizeof(P_DAT_ENTRY))
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Loading parts file: %s. Too small.", toWide(filename).c_str());
		return 1;
	}
	uint8_t* pDatBuf = (uint8_t*)malloc(fileSize);
	if(pDatBuf == nullptr)
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Allocating memory for parts file: %s.", toWide(filename).c_str());
		return 1;
	}
	inFile.seekg(0, std::ios::beg);
	inFile.read((char*)pDatBuf, fileSize);
	inFile.close();

	SBOL::BlowFish* SBOL_BF = new SBOL::BlowFish();
	if (SBOL_BF->PDATDecrypt((char*)pDatBuf, fileSize) < 0)
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Decrypting parts file: %s.", toWide(filename).c_str());
		return 1;
	}
	fileSize = *(size_t*)&pDatBuf[0x00];
	pDAT.count = *(uint32_t*)&pDatBuf[0x08];
	pDAT.pdat = (P_DAT_ENTRY*)malloc(fileSize);
	if(pDAT.pdat == nullptr)
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Allocating memory for parts file: %s.", toWide(filename).c_str());
		return 1;
	}
	logger->Log(Logger::LOGTYPE_NONE, L"Parts file %s loaded. Hash: %04X, size: %ubytes.", toWide(filename).c_str(), *(uint32_t*)&pDatBuf[0x04], fileSize);
	CopyMemory(pDAT.pdat, &pDatBuf[0x0C], fileSize - 4);
	free(pDatBuf);
	delete SBOL_BF;
	return 0;
}
int32_t Server::loadSDat()
{
	std::string filename = "data\\S.DAT";
	std::ifstream inFile(filename, std::ios::in | std::ios::binary);
	if (!inFile.is_open())
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Loading spec file: %s.", toWide(filename).c_str());
		return 1;
	}
	size_t fileSize = 0;
	std::streampos begin;
	begin = inFile.tellg();
	inFile.seekg(0, std::ios::end);
	fileSize = (size_t)(inFile.tellg() - begin);
	if (fileSize < sizeof(S_DAT_ENTRY))
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Loading spec file: %s. Too small.", toWide(filename).c_str());
		return 1;
	}
	uint8_t* sDatBuf = (uint8_t*)malloc(fileSize);
	if (sDatBuf == nullptr)
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Allocating memory for spec file: %s.", toWide(filename).c_str());
		return 1;
	}
	inFile.seekg(0, std::ios::beg);
	inFile.read((char*)sDatBuf, fileSize);
	inFile.close();

	SBOL::BlowFish* SBOL_BF = new SBOL::BlowFish();
	if (SBOL_BF->SDATDecrypt((char*)sDatBuf, fileSize) < 0)
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Decrypting spec file: %s.", toWide(filename).c_str());
		return 1;
	}
	fileSize = *(size_t*)&sDatBuf[0x00];
	sDAT.count = *(uint32_t*)&sDatBuf[0x08];
	sDAT.sdat = (S_DAT_ENTRY*)malloc(fileSize);
	if (sDAT.sdat == nullptr)
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Allocating memory for spec file: %s.", toWide(filename).c_str());
		return 1;
	}
	logger->Log(Logger::LOGTYPE_NONE, L"Spec file %s loaded. Hash: %04X, size: %ubytes.", toWide(filename).c_str(), *(uint32_t*)&sDatBuf[0x04], fileSize);
	CopyMemory(sDAT.sdat, &sDatBuf[0x0C], fileSize - 4);
	free(sDatBuf);
	delete SBOL_BF;
	return 0;
}
int32_t Server::loadPartShopData()
{
	std::string filename = "data\\PARTSHOP.DAT";
	std::ifstream inFile(filename, std::ios::in | std::ios::binary);
	if (!inFile.is_open())
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Loading shop file: %s.", toWide(filename).c_str());
		return 1;
	}
	size_t fileSize = 0;
	std::streampos begin;
	begin = inFile.tellg();
	inFile.seekg(0, std::ios::end);
	fileSize = (size_t)(inFile.tellg() - begin);
	if (fileSize < sizeof(PARTSHOPDATA_ENTRY) || fileSize % sizeof(PARTSHOPDATA_ENTRY))
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Loading shop file: %s. Unexpected size.", toWide(filename).c_str());
		return 1;
	}
	shopData.data = (PARTSHOPDATA_ENTRY*)malloc(fileSize);
	if (shopData.data == nullptr)
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Allocating memory for shop file: %s.", toWide(filename).c_str());
		return 1;
	}
	inFile.seekg(0, std::ios::beg);
	inFile.read((char*)shopData.data, fileSize);
	inFile.close();
	shopData.count = (uint16_t)(fileSize / sizeof(PARTSHOPDATA_ENTRY));
	logger->Log(Logger::LOGTYPE_NONE, L"Shop file %s loaded. Size: %ubytes.", toWide(filename).c_str(), fileSize);
	return 0;
}
int32_t Server::loadItemData()
{
	std::string filename = "data\\ITEM.DAT";
	std::ifstream inFile(filename, std::ios::in | std::ios::binary);
	if (!inFile.is_open())
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Loading item file: %s.", toWide(filename).c_str());
		return 1;
	}
	size_t fileSize = 0;
	std::streampos begin;
	begin = inFile.tellg();
	inFile.seekg(0, std::ios::end);
	fileSize = (size_t)(inFile.tellg() - begin);
	if (fileSize < sizeof(ITEMDATA_ENTRY) || (fileSize % sizeof(ITEMDATA_ENTRY)) != 0)
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Loading item file: %s. Unexpected size.", toWide(filename).c_str());
		return 1;
	}
	itemData.data = (ITEMDATA_ENTRY*)malloc(fileSize);
	if (itemData.data == nullptr)
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"Error Allocating memory for item file: %s.", toWide(filename).c_str());
		return 1;
	}
	inFile.seekg(0, std::ios::beg);
	inFile.read((char*)itemData.data, fileSize);
	inFile.close();
	itemData.count = (uint16_t)(fileSize / sizeof(ITEMDATA_ENTRY));
	logger->Log(Logger::LOGTYPE_NONE, L"Item file %s loaded. Size: %ubytes.", toWide(filename).c_str(), fileSize);
	return 0;
}

std::vector<std::string> Server::GetRivalFiles(std::string& path)
{
	std::vector<std::string> rivalfiles;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA ffd;
	std::string spec = path + "\\*.json";

	hFind = FindFirstFileA(spec.c_str(), &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return std::vector<std::string>();

	do {
		if (strcmp(ffd.cFileName, ".") != 0 &&
			strcmp(ffd.cFileName, "..") != 0 &&
			!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			rivalfiles.push_back(path + "\\" + ffd.cFileName);
		}
	} while (FindNextFileA(hFind, &ffd) != 0);

	return rivalfiles;
}
