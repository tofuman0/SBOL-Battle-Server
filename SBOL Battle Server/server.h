#pragma once
#include <nlohmann\json.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include <thread>
#include "globals.h"
#include "Logger.h"
#include "client.h"
#include "course.h"
#include "managementserver.h"
#include <BlowfishSBOL\BlowFishSBOL.h>

using json = nlohmann::json;

extern int tempValue1;
extern int tempValue2;
extern int tempValue3;
extern int tempValue4;

class SERVER
{
public:
	SERVER();
	~SERVER();
	const enum SERVERSTATUS {
		SS_RUNNING,
		SS_MAINTENANCE,
		SS_STOP,
		SS_END
	};
	const enum CARMANUFACTURERS {
		MANUFACTURER_INVALID = -1,
		MANUFACTURER_TOYOTA,
		MANUFACTURER_NISSAN,
		MANUFACTURER_MITSUBISHI,
		MANUFACTURER_MAZDA,
		MANUFACTURER_SUBARU,
		MANUFACTURER_SUKUKI,
		MANUFACTURER_HONDA
	};
	const enum CARLIST {
		AE86_L_3_1985,						//0
		AE86_T_3_1985,						//1
		AE86_L_2_1985,						//2
		AE86_T_2_1985,						//3
		MR2_GT_1997,						//4
		MR2_G_1997,							//5
		SUPRA_RZ_1993,						//6
		SUPRA_RZ_1997 = 8,					//8
		SUPRA_SZR_1997,						//9
		ALTEZZA_R200_Z_1997,				//10
		CHASER_TOURER_V_1996 = 12,			//12
		CHASER_TOURER_V_1998,				//13
		MARKII_TOURER_V_1998 = 15,			//15
		SILVIA_K_1988,						//16
		SILVIA_Q_1988,						//17
		SILVIA_K_1991,						//18
		SILVIA_K_1993 = 21,					//21
		SILVIA_K_1996 = 23,					//23
		SILVIA_SPECR_1999 = 25,				//25
		SILVIA_SPECS_1999,					//26
		S180SX_TYPEIII_1994 = 28,			//28
		SKYLINE_GTR_VSPECII_1994,			//29
		SKYLINE_GTR_1989,					//30
		SKYLINE_GTST_TYPEM_1991,			//31
		SKYLINE_GTR_VSPEC_1997 = 33,		//33
		//SKYLINE_GTR_R34 = 35,				//35
		SKYLINE_25GT_TURBO_1998 = 36,		//36
		SKYLINE_25GT_TURBO_2000,			//37
		FAIRLADY_Z_S_TT_1998,				//38
		FAIRLADY_Z_TBAR_1998,				//39
		CEDRIC_BROUGHAM_VIP_1997,			//40
		GLORIA_GRANTURISMO_ULTIMA_1997,		//41
		LANCER_GSR_EVOIII_1995 = 51,		//51
		LANCER_GSR_EVOIV_1996,				//52
		SAVANNA_RX7_INFIIII_1989 = 56,		//56
		RX7_TYPERS_1999 = 58,				//58
		RX7_TYPERZ_1998,					//59
		IMPREZA_WRX_STI_VERIV_1997 = 61,	//61
		IMPREZA_WRX_STI_VERV_1998 = 63,		//63
		IMPREZA_WRX_STI_VERVI_1999 = 65,	//65
		FAIRLADY_Z_240ZG_1972 = 75,			//75
		MX5_ROADSTER_RS_1998 = 80,			//80
		MX5_ROADSTER_RS_2000,				//81
		CELICA_GTFOUR_1996 = 83,			//83
		CELICA_SSIII_1996,					//84
		GTO_TT_1993 = 87,					//87
		FTO_GP_VERR_1997,					//88
		PULSAR_SERIE_VZN1_VER2_1998,		//89
		LEGACY_TOURING_WAGON_GTB_1998 = 91,	//91
		CELICA_SSII_SSPACK_1999,			//92
		MRS_1999,							//93
		CEDRIC_300VIP_1999,					//94
		GLORIA_300_ULTIMA_1999,				//95
		AZ1_1992 = 108,						//108
		CAPPUCCINO_1991,					//109
		CEFIRO_CRUISING_1990 = 112,			//112
		STARION_GSRVR_1988 = 114,			//114
		SAVANNA_RX3_GT_1973,				//115
		SKYLINE_2000GTR_1971,				//116
		SAVANNA_RX7_GTX_1983 = 119,			//119
		ECLIPSE_GST_1999 = 124,				//124
		BB15Z_2000 = 127,					//127
		SUPRA_30GT_T_1989 = 129,			//129
		SUPRA_25GT_TT_1991,					//130
		LANCER_GSR_EVOVII_2001,				//131
		IMPREZA_WRX_NB_2000,				//132
		IMPREZA_WRX_STI_2000,				//133
		LANTIS_COUPE_TYPER_1993,			//134
		//SKYLINE_GTR_R34RKK = 141			// R34RKK
	};
	const enum CARCLASS {
		CARCLASS_A,
		CARCLASS_B,
		CARCLASS_C
	};
	const enum CHATTYPE {
		CHATTYPE_ANNOUNCE,
		CHATTYPE_NOTIFICATION,
		CHATTYPE_PRIVATE,
		CHATTYPE_EVENT = 0x03,
		CHATTYPE_NORMAL,
		CHATTYPE_ADMIN = 0xE0,
	};
	const enum AUTHLIST {
		AUTH_INVALID_UN,
		AUTH_INVALID_PW,
		AUTH_BUSY,
		AUTH_OUTDATED,
		AUTH_LOGGED_IN,
		AUTH_EXPIRED,
		AUTH_DISABLED,
		AUTH_MAINTENANCE
	};
	const enum DISCONNECTSTRINGS {
		DC_LOGOUT,
		DC_MAINTENTANCE,
		DC_KICKED,
		DC_BANNED
	};
	std::vector<Client*> connections;
	std::vector<COURSE*> courses[COURSE_DIMENSIONS];
	std::vector<uint32_t> expToLevel;
	std::vector<RIVALDATA> rivalData;
	P_DAT pDAT;
	S_DAT sDAT;
	PARTSHOPDATA shopData;
	ITEMDATA itemData;
	uint8_t* hDAT;
	Logger logger;
	MANAGEMENTSERVER managementserver;
	std::vector<uint32_t> startingCars;
	uint32_t shopBasePrices[11] = {
		//100, 102, 104, 106, 108, 110, 112, 115, 117, 120, 120
		100, 102, 118, 134, 150, 166, 182, 150, 198, 214, 214
	};
#pragma region Server Functions
	char* HEXString(uint8_t* in, char* out, uint32_t length);
	char* HEXString(uint8_t* in, uint32_t length);
	int32_t Start();
	int32_t Stop();
	void initialize();
	int32_t LoadConfig(const wchar_t* filename);
	bool isRunning() { return running; }
	static void serverThread(void* parg);
	void CheckClientPackets(Client* client, uint32_t rcvcopy, uint8_t* tmprcv);
	void initialize_connection(Client* connect);
	void removeClient(Client* connect);
	void organizeClients();
	std::string getMANAGEMENTSERVERAddress() { return managementServerAddress; }
	uint16_t getMANAGEMENTSERVERPort() { return managementServerPort; }
	void setConnectedToMANAGEMENTSERVER();
	void clearConnectedToMANAGEMENTSERVER();
	bool getConnectedToMANAGEMENTSERVER() { return connectedToManagement; }
	void addAuthenticatedClient(uint32_t license);
	void removeAuthenticatedClient(uint32_t license);
	bool clientIsAuthenticated(uint32_t license);
	void disconnectLoggedInUser(uint32_t license);
	void disconnectAllUsers(uint32_t exclude);
	Client* findUser(std::string& in);
	Client* findUser(uint32_t in);
	void saveClientData(Client* client);
	std::vector<std::string> split(std::string& in, std::string& delimit);
	uint32_t getNextClientID();
	bool isValidCar(uint16_t carID);
	bool isValidItem(uint16_t itemID);
	uint32_t playerCount();
	SERVERSTATUS getStatus() { return status; };
	void LoadRivalFile();
	RIVALDATA* GetRivalData(int32_t RivalID, Client* client = nullptr);
	void setStatus(SERVERSTATUS _status) { status = _status; };
	void SendChatMessage(CHATTYPE type, std::string& handle, std::string& message);
	void SendOfflineChatMessage(CHATTYPE type, std::string& fromHandle, std::string& toHandle, std::string& message);
	void SendTeamChatMessage(std::string& fromHandle, uint32_t teamID, std::string& message);
	void SendInformMessage(std::string& toHandle, std::string& message, uint32_t colour);
	void SendAnnounceMessage(std::string& message, uint32_t colour = 0xFFFFFF);
	void SendToAll(PACKET* src);
#pragma endregion
#pragma region Communication Functions
	int32_t tcp_sock_open(struct in_addr ip, uint16_t port);
	int32_t tcp_set_nonblocking(int32_t fd);
	int32_t tcp_listen(int32_t sockfd);
	int32_t tcp_accept(int32_t sockfd, struct sockaddr *client_addr, int32_t *addr_len);
	bool freeConnection();
	int32_t ServerSocket;
	int32_t TempSocket;
	uint32_t getServerNumConnections() { return serverNumConnections; }
	bool InitServerSocket();
	bool AcceptConnection();
	bool Send(Client* client);
	bool Recv(Client* client);
	bool ProcessRecv(Client* client, int32_t len);
	bool RecvToProcess(Client* client);
	bool ClientChecks(Client* client);
#pragma endregion
private:
	uint32_t currentClientID;
	std::vector<uint32_t> authenticatedClients;
	std::vector<uint32_t> serverConnections;
	uint32_t serverNumConnections;
	bool running;
	bool connectedToManagement;
	SERVERSTATUS status;
	std::wstring toWide(std::string in);
	std::string toNarrow(std::wstring in);
	char hexStr[(MAX_MESG_LEN + 1) * 2];
	const wchar_t* CONFIG_FILENAME = L"config.json";
	uint16_t serverPort;
	std::string serverName;
	uint32_t serverClientLimit;
	std::string serverWelcomeMessage;
	uint16_t managementServerPort;
	std::string managementServerAddress;
	int32_t VerifyConfig();
	int32_t loadPDat();
	int32_t loadSDat();
	int32_t loadPartShopData();
	int32_t loadItemData();
	std::vector<std::string> GetRivalFiles(std::string& path = std::string(".\\data\\rivals"));
};

