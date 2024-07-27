#define _WINSOCKAPI_
#include <Windows.h>
#include <mutex>
#include <vector>
#include "globals.h"
#include "Logger.h"
#include "course.h"
#include "client.h"
#include "packet.h"
#include "serverpacket.h"
#include "packets.h"
#include "managementpackets.h"

Client::Client()
{
	initialize();
}
Client::~Client()
{
	clearRivals();
}
void Client::initialize()
{
	test1 = 0;
	test2 = 0;
	packetResend = 0;
	startJunction = 0;
	startDistance = 0;
	ZeroMemory((uint8_t*)&position, sizeof(POSITION));
	positionCount = 0;
	ClientSocket = -1;
	isClient = false;
	todc = false;
	canSave = false;
	lastSave = time(NULL) + AUTOSAVE_TIME;
	notBeginner = false;
	inCourse = false;
	hasPlayers = false;
	snddata = 0;
	driverslicense = 0;
	privileges = 0;
	logPosition = false;
	status = PLAYERSTATUS::PS_SAFEMODE;
	rcvread = 0;
	packetsize = 0;
	packetoffset = 0;
	hassize = false;
	packetready = false;
	server = nullptr;
	timeoutCount = 0;
	sendWelcome = 0;
	currentCourse = 0;
	course = nullptr;
	ZeroMemory((uint8_t*)&battle, sizeof(BATTLE));
	courseID = 0;
	ping = 0;
	pokes = 0;
	ZeroMemory(&packetAllow[0], sizeof(packetAllow));
	ZeroMemory(&IP_Address[0], sizeof(IP_Address));
	ZeroMemory(&ipaddr[0], sizeof(ipaddr));
	handle = "";
	activeSign = 0;
	ZeroMemory(&sign[0], sizeof(sign));
	ZeroMemory((uint8_t*)&teamdata, sizeof(teamdata));
	ZeroMemory((uint8_t*)&teammembers, sizeof(teammembers));
	teamareaaccess = 0;
	ZeroMemory((uint8_t*)&careerdata, sizeof(careerdata));
	inbuf.clearBuffer();
	outbuf.clearBuffer();
	ZeroMemory(&rcvbuf[0], sizeof(rcvbuf));
	ZeroMemory(&sndbuf[0], sizeof(sndbuf));
	itembox.clear();
	itembox.shrink_to_fit();
	shopCars.clear();
	shopCars.shrink_to_fit();
	clearSendQueue();
	clearRivals();
	currentRival = nullptr;
	ZeroMemory(&unlockableCars[0], sizeof(unlockableCars));
}
void Client::initializeGarage()
{
	garagedata.activeCar = nullptr;
	garagedata.activeCarBay = 0;
	garagedata.garageCount = 0;
	garagedata.garageType.clear();
	garagedata.garageType.shrink_to_fit();
	garagedata.car.clear();
	garagedata.car.shrink_to_fit();
	garagedata.car.resize(GARAGE_LIMIT);
	for (auto& car : garagedata.car)
	{
		car.carID = 0xFFFFFFFF;
	}
}
void Client::initializeTeam()
{
	ZeroMemory(&teamdata, sizeof(TEAMDATA));
	teamdata.teamID = 0xFFFFFFFF;
}
int32_t Client::joinCourse()
{
	if (course != nullptr) course->removeClient(this);
	if (notBeginner == false) currentCourse = 8;
	int32_t courseNumber = currentCourse;
	for (uint32_t i = 0; i < COURSE_DIMENSIONS; i++)
	{
		if (server->courses[i][courseNumber]->getClientCount() < COURSE_PLAYER_LIMIT)
		{
			server->courses[i][courseNumber]->addClient(this);
			break;
		}
	}
	if (course == nullptr)
	{
		logger->Log(Logger::LOGTYPE_Client, L"Client can't join course.");
		Disconnect();
		return 0;
	}
	return 1;
}
int8_t Client::getCarCount()
{
	int8_t count = 0;
	for (uint32_t i = 0; i < (uint32_t)(garagedata.garageCount * 4); i++)
	{
		if (garagedata.car[i].carID != 0xFFFFFFFF) count++;
	}
	return count;
}
int32_t Client::getEmptyBay()
{
	for (uint32_t i = 0; i < (uint32_t)(garagedata.garageCount * 4); i++)
	{
		if (garagedata.car[i].carID == 0xFFFFFFFF) return i;
	}
	return -1;
}
bool Client::addCar(int32_t carID, uint32_t bay, COLOUR2 colour)
{
	return addCar(carID, bay, colour.R1, colour.G1, colour.B1, colour.R2, colour.G2, colour.B2);
}
bool Client::addCar(int32_t carID, uint32_t bay, float r1, float g1, float b1, float r2, float g2, float b2)
{
	if ((garagedata.garageCount * 4) <= GARAGE_LIMIT)
	{
		if (bay >= garagedata.car.size() || garagedata.car[bay].carID != 0xFFFFFFFF) return false;
		garagedata.car[bay].carID = carID;
		garagedata.car[bay].status = 0;
		garagedata.car[bay].KMs = 0.0f;
		garagedata.car[bay].carMods.colour1_R = r1;
		garagedata.car[bay].carMods.colour1_G = g1;
		garagedata.car[bay].carMods.colour1_B = b1;
		garagedata.car[bay].carMods.colour2_R = r2;
		garagedata.car[bay].carMods.colour2_G = g2;
		garagedata.car[bay].carMods.colour2_B = b2;
		garagedata.car[bay].engineCondition = 10099;
		garagedata.car[bay].carMods.wheels = carID;
		garagedata.car[bay].carMods.stickers = 0xFFFFFFFF;
		// Set Normal part types as owned.
		garagedata.car[bay].parts.unknown1[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.engine[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.muffler[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.transmission[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.differential[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.tyreBrakes[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.suspension[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.body[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.overFenders[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.frontBumper[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.rearBumper[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.bonnet[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.mirrors[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.sideSkirts[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.rearSpoiler[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.grill[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.car[bay].parts.lights[0] = PART_UNLOCKED | PART_OWNED;
		return true;
	}
	return false;
}
bool Client::removeCar(uint32_t bay)
{
	if (bay >= garagedata.car.size() || garagedata.car[bay].carID == 0xFFFFFFFF || bay == garagedata.activeCarBay) return false;
	ZeroMemory(&garagedata.car[bay], sizeof(CAR));
	garagedata.car[bay].carID = 0xFFFFFFFF;
	server->managementserver.RemoveCar(driverslicense, bay);
	return true;
}
int32_t Client::setActiveCar(uint32_t bay)
{
	if (garagedata.garageCount > 0)
	{
		garagedata.activeCarBay = bay % (garagedata.garageCount * 4);
		garagedata.activeCar = &garagedata.car[garagedata.activeCarBay];

		garagedata.activeCar->parts.unknown1[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.engine[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.muffler[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.transmission[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.differential[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.tyreBrakes[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.suspension[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.body[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.overFenders[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.frontBumper[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.rearBumper[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.bonnet[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.mirrors[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.sideSkirts[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.rearSpoiler[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.grill[0] = PART_UNLOCKED | PART_OWNED;
		garagedata.activeCar->parts.lights[0] = PART_UNLOCKED | PART_OWNED;
	}
	else
	{
		garagedata.activeCar = nullptr;
	}
	return (garagedata.activeCar) ? 0 : 1;
}
int32_t Client::getCarID(uint32_t bay)
{
	if (bay < garagedata.car.size() && garagedata.car[bay].carID != 0xFFFFFFFF) return (int32_t)garagedata.car[bay].carID;
	else return -1;
}
int32_t Client::getCarManufacturer(uint32_t carID)
{
	switch (carID)
	{
	case Server::CARLIST::AE86_L_3_1985:
	case Server::CARLIST::AE86_T_3_1985:
	case Server::CARLIST::AE86_L_2_1985:
	case Server::CARLIST::AE86_T_2_1985:
	case Server::CARLIST::MR2_GT_1997:
	case Server::CARLIST::MR2_G_1997:
	case Server::CARLIST::SUPRA_RZ_1993:
	case Server::CARLIST::SUPRA_RZ_1997:
	case Server::CARLIST::SUPRA_SZR_1997:
	case Server::CARLIST::ALTEZZA_R200_Z_1997:
	case Server::CARLIST::CHASER_TOURER_V_1996:
	case Server::CARLIST::CHASER_TOURER_V_1998:
	case Server::CARLIST::MARKII_TOURER_V_1998:
	case Server::CARLIST::CELICA_GTFOUR_1996:
	case Server::CARLIST::CELICA_SSIII_1996:
	case Server::CARLIST::CELICA_SSII_SSPACK_1999:
	case Server::CARLIST::MRS_1999:
	case Server::CARLIST::BB15Z_2000:
	case Server::CARLIST::SUPRA_30GT_T_1989:
	case Server::CARLIST::SUPRA_25GT_TT_1991:
	case Server::CARLIST::LANTIS_COUPE_TYPER_1993:
		return (int32_t)Server::CARMANUFACTURERS::MANUFACTURER_TOYOTA;
	case Server::CARLIST::SILVIA_K_1988:
	case Server::CARLIST::SILVIA_Q_1988:
	case Server::CARLIST::SILVIA_K_1991:
	case Server::CARLIST::SILVIA_K_1993:
	case Server::CARLIST::SILVIA_K_1996:
	case Server::CARLIST::SILVIA_SPECR_1999:
	case Server::CARLIST::SILVIA_SPECS_1999:
	case Server::CARLIST::S180SX_TYPEIII_1994:
	case Server::CARLIST::SKYLINE_GTR_VSPECII_1994:
	case Server::CARLIST::SKYLINE_GTR_1989:
	case Server::CARLIST::SKYLINE_GTST_TYPEM_1991:
	case Server::CARLIST::SKYLINE_GTR_VSPEC_1997:
	case Server::CARLIST::SKYLINE_25GT_TURBO_1998:
	case Server::CARLIST::SKYLINE_25GT_TURBO_2000:
	case Server::CARLIST::FAIRLADY_Z_S_TT_1998:
	case Server::CARLIST::FAIRLADY_Z_TBAR_1998:
	case Server::CARLIST::CEDRIC_BROUGHAM_VIP_1997:
	case Server::CARLIST::GLORIA_GRANTURISMO_ULTIMA_1997:
	case Server::CARLIST::FAIRLADY_Z_240ZG_1972:
	case Server::CARLIST::PULSAR_SERIE_VZN1_VER2_1998:
	case Server::CARLIST::CEDRIC_300VIP_1999:
	case Server::CARLIST::GLORIA_300_ULTIMA_1999:
	case Server::CARLIST::CEFIRO_CRUISING_1990:
	case Server::CARLIST::SKYLINE_2000GTR_1971:
		return (int32_t)Server::CARMANUFACTURERS::MANUFACTURER_NISSAN;
	case Server::CARLIST::LANCER_GSR_EVOIII_1995:
	case Server::CARLIST::LANCER_GSR_EVOIV_1996:
	case Server::CARLIST::GTO_TT_1993:
	case Server::CARLIST::FTO_GP_VERR_1997:
	case Server::CARLIST::STARION_GSRVR_1988:
	case Server::CARLIST::ECLIPSE_GST_1999:
	case Server::CARLIST::LANCER_GSR_EVOVII_2001:
		return (int32_t)Server::CARMANUFACTURERS::MANUFACTURER_MITSUBISHI;
	case Server::CARLIST::SAVANNA_RX7_INFIIII_1989:
	case Server::CARLIST::RX7_TYPERS_1999:
	case Server::CARLIST::RX7_TYPERZ_1998:
	case Server::CARLIST::MX5_ROADSTER_RS_1998:
	case Server::CARLIST::MX5_ROADSTER_RS_2000:
	case Server::CARLIST::AZ1_1992:
	case Server::CARLIST::SAVANNA_RX3_GT_1973:
	case Server::CARLIST::SAVANNA_RX7_GTX_1983:
		return (int32_t)Server::CARMANUFACTURERS::MANUFACTURER_MAZDA;
	case Server::CARLIST::IMPREZA_WRX_STI_VERIV_1997:
	case Server::CARLIST::IMPREZA_WRX_STI_VERV_1998:
	case Server::CARLIST::IMPREZA_WRX_STI_VERVI_1999:
	case Server::CARLIST::LEGACY_TOURING_WAGON_GTB_1998:
	case Server::CARLIST::IMPREZA_WRX_NB_2000:
	case Server::CARLIST::IMPREZA_WRX_STI_2000:
		return (int32_t)Server::CARMANUFACTURERS::MANUFACTURER_SUBARU;
	case Server::CARLIST::CAPPUCCINO_1991:
		return (int32_t)Server::CARMANUFACTURERS::MANUFACTURER_SUKUKI;
	default:
		return (int32_t)Server::CARMANUFACTURERS::MANUFACTURER_INVALID;
	}
}
int32_t Client::setHandle(std::string& in)
{
	handle = in.substr(0, 15);
	return 0;
}
int32_t Client::setUsername(std::string& in)
{
	username = in.substr(0, 20);
	return 0;
}
bool Client::enoughCP(int64_t price)
{
	if (careerdata.CP - price < 0) return false;
	else return true;
}
void Client::takeCP(int64_t price)
{
	if (careerdata.CP - price >= 0) careerdata.CP -= price;
	else careerdata.CP = 0;
}
void Client::giveCP(int64_t cp)
{
	if((careerdata.CP + cp) > careerdata.CP) careerdata.CP += cp;
	else careerdata.CP = _I64_MAX;
}
uint32_t Client::calculateOverhaul(uint32_t bay)
{
	if (bay > garagedata.car.size() || garagedata.car[bay].carID == 0xFFFFFFFF) return 0;
	S_DAT_ENTRY* sDAT = &server->sDAT.sdat[garagedata.car[bay].carID % server->sDAT.count];
	uint8_t carClass = sDAT->carClass;
	uint8_t shopTable = sDAT->shopTable;
	uint16_t engineType = (sDAT->engineType > 2) ? 2 : sDAT->turbo & 1;
	uint16_t engineDisplacement = sDAT->displacement;
	uint16_t engineLevel = 1;
	for (int32_t i = 1; i < sizeof(PARTS::engine) - 1; i++) if (garagedata.car[bay].parts.engine[i] & PART_OWNED) engineLevel = i;

	P_DAT_ENTRY* pDAT = &server->pDAT.pdat[shopTable % server->pDAT.count];
	return (uint32_t)(
		(
			(
				((float)(pDAT->engine[engineType][engineLevel].price[carClass]) / 10.0f) * 
				((float)engineDisplacement / 1000.0f) *
				(float)OVERHAULBASE_COST
			) / 2000.0f
		) *	(float)(100 - (min(garagedata.car[bay].engineCondition / 100, 100)))
	);
}
uint16_t Client::getShopPartPrice(uint32_t bay, uint8_t itemCategory, uint8_t itemType, uint32_t itemID)
{
	if (bay >= garagedata.car.size() || garagedata.car[bay].carID == 0xFFFFFFFF) return 0;
	S_DAT_ENTRY* sDAT = &server->sDAT.sdat[garagedata.car[bay].carID % server->sDAT.count];
	uint8_t carClass = sDAT->carClass;
	uint8_t shopTable = sDAT->shopTable;
	uint8_t engineType = (sDAT->engineType > 2) ? 2 : sDAT->turbo & 1;
	uint8_t transmissionType = (sDAT->gear5Ratio ? 1 : 0) + (sDAT->gear6Ratio ? 1 : 0);
	P_DAT_ENTRY* pDAT = &server->pDAT.pdat[shopTable % server->pDAT.count];

	switch (itemCategory)
	{
	case 1:
	{
		switch (itemType)
		{
		case 1: // Front Bumper
			return pDAT->frontBumper[itemID].price[carClass] * pDAT->frontBumper[0].price[carClass];
		case 2: // Bonnet
			return pDAT->bonnet[itemID].price[carClass] * pDAT->bonnet[0].price[carClass];
		case 3: // Mirrors
			return pDAT->mirror[itemID].price[carClass] * pDAT->mirror[0].price[carClass];
		case 4: // Side Skirts
			return pDAT->sideSkirt[itemID].price[carClass] * pDAT->sideSkirt[0].price[carClass];
		case 5: // Rear Bumper
			return pDAT->rearBumper[itemID].price[carClass] * pDAT->rearBumper[0].price[carClass];
		case 6: // Rear Spoiler
			return pDAT->rearSpoiler[itemID].price[carClass] * pDAT->rearSpoiler[0].price[carClass];
		default: // Over Fender
			return pDAT->overFender[itemID].price[carClass] * pDAT->overFender[0].price[carClass];
		}
	}
	break;
	case 2:
	{
		switch (itemType)
		{
		case 1: // Lights
			return pDAT->lights[itemID].price[carClass] * pDAT->lights[0].price[carClass];
		default: // Grill
			return pDAT->grill[itemID].price[carClass] * pDAT->grill[0].price[carClass];
		}
	}
	break;
	default:
	{
		switch (itemType)
		{
		case 1: // Muffler
			return pDAT->muffler[engineType][itemID].price[carClass] * pDAT->muffler[engineType][0].price[carClass];
		case 2: // Transmission
			return pDAT->transmission[transmissionType][itemID].price[carClass] * pDAT->transmission[transmissionType][0].price[carClass];
		case 3: // Differential
			return pDAT->differential[itemID].price[carClass] * pDAT->differential[0].price[carClass];
		case 4: // Tire / Brakes use calculated price table instead
		{
			if (itemID == 0 || itemID > 9)
			{
				logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to purchase invalid item from Tire/Brake shop ID %u",
					logger->toWide(handle).c_str(),
					driverslicense,
					logger->toWide((char*)&IP_Address).c_str(),
					itemID);
				return 0;
			}
			int32_t limit = server->shopData.count;
			PARTSHOPDATA_ENTRY* shopData = server->shopData.data;
			float price[4] = {
				shopData[shopData[sDAT->pLookup[0x00] % limit].redirect[itemID - 1].table].tireBrakeBase1,
				shopData[shopData[sDAT->pLookup[0x00] % limit].redirect[itemID - 1].table].tireBrakeBase2,
				shopData[shopData[sDAT->pLookup[0x01] % limit].redirect[itemID - 1].table].tireBrakeBase1,
				shopData[shopData[sDAT->pLookup[0x01] % limit].redirect[itemID - 1].table].tireBrakeBase2
			};
			return (
				(uint32_t)((((price[0] + price[1]) * server->shopBasePrices[itemID % (sizeof(server->shopBasePrices) / 4)]) + 0.5f) / 10.0f) +
				(uint32_t)((((price[2] + price[3]) * server->shopBasePrices[itemID % (sizeof(server->shopBasePrices) / 4)]) + 0.5f) / 10.0f)
				);
		}
		case 6: // Suspension
			return pDAT->suspension[itemID].price[carClass] * pDAT->suspension[0].price[carClass];
		case 7: //Body
			return pDAT->body[itemID].price[carClass] * pDAT->body[0].price[carClass];
		default: // Engine
			return pDAT->engine[engineType][itemID].price[carClass] * pDAT->engine[engineType][0].price[carClass];
		}
	}
	break;
	}
	return 0;
}
uint16_t Client::getShopPartPriceFromID(uint32_t carID, uint8_t itemCategory, uint8_t itemType, uint32_t itemID)
{
	S_DAT_ENTRY* sDAT = &server->sDAT.sdat[carID % server->sDAT.count];
	uint8_t carClass = sDAT->carClass;
	uint8_t shopTable = sDAT->shopTable;
	uint8_t engineType = (sDAT->engineType > 2) ? 2 : sDAT->turbo & 1;
	uint8_t transmissionType = (sDAT->gear5Ratio ? 1 : 0) + (sDAT->gear6Ratio ? 1 : 0);
	P_DAT_ENTRY* pDAT = &server->pDAT.pdat[shopTable % server->pDAT.count];

	switch (itemCategory)
	{
	case 1:
	{
		switch (itemType)
		{
		case 1: // Front Bumper
			return pDAT->frontBumper[itemID].price[carClass] * pDAT->frontBumper[0].price[carClass];
		case 2: // Bonnet
			return pDAT->bonnet[itemID].price[carClass] * pDAT->bonnet[0].price[carClass];
		case 3: // Mirrors
			return pDAT->mirror[itemID].price[carClass] * pDAT->mirror[0].price[carClass];
		case 4: // Side Skirts
			return pDAT->sideSkirt[itemID].price[carClass] * pDAT->sideSkirt[0].price[carClass];
		case 5: // Rear Bumper
			return pDAT->rearBumper[itemID].price[carClass] * pDAT->rearBumper[0].price[carClass];
		case 6: // Rear Spoiler
			return pDAT->rearSpoiler[itemID].price[carClass] * pDAT->rearSpoiler[0].price[carClass];
		default: // Over Fender
			return pDAT->overFender[itemID].price[carClass] * pDAT->overFender[0].price[carClass];
		}
	}
	break;
	case 2:
	{
		switch (itemType)
		{
		case 1: // Lights
			return pDAT->lights[itemID].price[carClass] * pDAT->lights[0].price[carClass];
		default: // Grill
			return pDAT->grill[itemID].price[carClass] * pDAT->grill[0].price[carClass];
		}
	}
	break;
	default:
	{
		switch (itemType)
		{
		case 1: // Muffler
			return pDAT->muffler[engineType][itemID].price[carClass] * pDAT->muffler[engineType][0].price[carClass];
		case 2: // Transmission
			return pDAT->transmission[transmissionType][itemID].price[carClass] * pDAT->transmission[transmissionType][0].price[carClass];
		case 3: // Differential
			return pDAT->differential[itemID].price[carClass] * pDAT->differential[0].price[carClass];
		case 4: // Tire / Brakes use calculated price table instead
		{
			if (itemID == 0 || itemID > 9)
			{
				logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to purchase invalid item from Tire/Brake shop ID %u",
					logger->toWide(handle).c_str(),
					driverslicense,
					logger->toWide((char*)&IP_Address).c_str(),
					itemID);
				return UINT16_MAX;
			}
			int32_t limit = server->shopData.count;
			PARTSHOPDATA_ENTRY* shopData = server->shopData.data;
			float price[4] = {
				shopData[shopData[sDAT->pLookup[0x00] % limit].redirect[itemID - 1].table].tireBrakeBase1,
				shopData[shopData[sDAT->pLookup[0x00] % limit].redirect[itemID - 1].table].tireBrakeBase2,
				shopData[shopData[sDAT->pLookup[0x01] % limit].redirect[itemID - 1].table].tireBrakeBase1,
				shopData[shopData[sDAT->pLookup[0x01] % limit].redirect[itemID - 1].table].tireBrakeBase2
			};
			return (
				(uint32_t)((((price[0] + price[1]) * server->shopBasePrices[itemID % (sizeof(server->shopBasePrices) / 4)]) + 0.5f) / 10.0f) +
				(uint32_t)((((price[2] + price[3]) * server->shopBasePrices[itemID % (sizeof(server->shopBasePrices) / 4)]) + 0.5f) / 10.0f)
				);
		}
		case 6: // Suspension
			return pDAT->suspension[itemID].price[carClass] * pDAT->suspension[0].price[carClass];
		case 7: //Body
			return pDAT->body[itemID].price[carClass] * pDAT->body[0].price[carClass];
		default: // Engine
			return pDAT->engine[engineType][itemID].price[carClass] * pDAT->engine[engineType][0].price[carClass];
		}
	}
	break;
	}
	return UINT16_MAX;
}
bool Client::equipPart(uint32_t bay, uint8_t itemCategory, uint8_t itemType, uint32_t itemID)
{
	if (bay >= garagedata.car.size() || garagedata.car[bay].carID == 0xFFFFFFFF) return false;
	S_DAT_ENTRY* sDAT = &server->sDAT.sdat[garagedata.car[bay].carID % server->sDAT.count];
	uint8_t carClass = sDAT->carClass;
	uint8_t shopTable = sDAT->shopTable;
	uint8_t engineType = sDAT->engineType;
	uint8_t transmissionType = (sDAT->gear5Ratio ? 1 : 0) + (sDAT->gear6Ratio ? 1 : 0);
	P_DAT_ENTRY* pDAT = &server->pDAT.pdat[shopTable % server->pDAT.count];

	switch (itemCategory)
	{
	case 1:
	{
		switch (itemType)
		{
		case 1: // Front Bumper
			if (garagedata.car[bay].parts.frontBumper[itemID] & PART_OWNED && itemID < sizeof(PARTS::frontBumper))
			{
				garagedata.car[bay].carMods.frontbumper = itemID;
				return true;
			}
			else return false;
		case 2: // Bonnet
			if (garagedata.car[bay].parts.bonnet[itemID] & PART_OWNED && itemID < sizeof(PARTS::bonnet))
			{
				garagedata.car[bay].carMods.bonnet = itemID;
				return true;
			}
			else return false;
		case 3: // Mirrors
			if (garagedata.car[bay].parts.mirrors[itemID] & PART_OWNED && itemID < sizeof(PARTS::mirrors))
			{
				garagedata.car[bay].carMods.mirrors = itemID;
				return true;
			}
			else return false;
		case 4: // Side Skirts
			if (garagedata.car[bay].parts.sideSkirts[itemID] & PART_OWNED && itemID < sizeof(PARTS::sideSkirts))
			{
				garagedata.car[bay].carMods.sideskirts = itemID;
				return true;
			}
			else return false;
		case 5: // Rear Bumper
			if (garagedata.car[bay].parts.rearBumper[itemID] & PART_OWNED && itemID < sizeof(PARTS::rearBumper))
			{
				garagedata.car[bay].carMods.rearbumper = itemID;
				return true;
			}
			else return false;
		case 6: // Rear Spoiler
			if (garagedata.car[bay].parts.rearSpoiler[itemID] & PART_OWNED && itemID < sizeof(PARTS::rearSpoiler))
			{
				garagedata.car[bay].carMods.rearspoiler = itemID;
				return true;
			}
			else return false;
		default: // Over Fender
			if (garagedata.car[bay].parts.overFenders[itemID] & PART_OWNED && itemID < sizeof(PARTS::overFenders))
			{
				garagedata.car[bay].carMods.overfenders = itemID;
				return true;
			}
			else return false;
		}
	}
	break;
	case 2:
	{
		switch (itemType)
		{
		case 1: // Lights
			if (garagedata.car[bay].parts.lights[itemID] & PART_OWNED && itemID < sizeof(PARTS::lights))
			{
				garagedata.car[bay].carMods.lights = itemID;
				return true;
			}
			else return false;
		default: // Grill
			if (garagedata.car[bay].parts.grill[itemID] & PART_OWNED && itemID < sizeof(PARTS::grill))
			{
				garagedata.car[bay].carMods.grill = itemID;
				return true;
			}
			else return false;
		}
	}
	break;
	default:
	{
		switch (itemType)
		{
		case 1: // Muffler
			if (garagedata.car[bay].parts.muffler[itemID] & PART_OWNED && itemID < sizeof(PARTS::muffler))
			{
				garagedata.car[bay].carMods.muffler = itemID;
				return true;
			}
			else return false;
		case 2: // Transmission
			if (garagedata.car[bay].parts.transmission[itemID] & PART_OWNED && itemID < sizeof(PARTS::transmission))
			{
				garagedata.car[bay].carMods.transmission = itemID;
				return true;
			}
			else return false;
		case 3: // Differential
			if (garagedata.car[bay].parts.differential[itemID] & PART_OWNED && itemID < sizeof(PARTS::differential))
			{
				garagedata.car[bay].carMods.differential = itemID;
				return true;
			}
			else return false;
		case 4: // Tire / Brakes use calculated price table instead
			if (garagedata.car[bay].parts.tyreBrakes[itemID] & PART_OWNED && itemID < sizeof(PARTS::tyreBrakes))
			{
				garagedata.car[bay].carMods.tyrebrakes = itemID;
				return true;
			}
			else return false;
		case 6: // Suspension
			if (garagedata.car[bay].parts.suspension[itemID] & PART_OWNED && itemID < sizeof(PARTS::suspension))
			{
				garagedata.car[bay].carMods.suspension = itemID;
				return true;
			}
			else return false;
		case 7: //Body
			if (garagedata.car[bay].parts.body[itemID] & PART_OWNED && itemID < sizeof(PARTS::body))
			{
				garagedata.car[bay].carMods.body = itemID;
				return true;
			}
			else return false;
		default: // Engine
			if (garagedata.car[bay].parts.engine[itemID] & PART_OWNED && itemID < sizeof(PARTS::engine))
			{
				garagedata.car[bay].carMods.engine = itemID;
				return true;
			}
			else return false;
		}
	}
	break;
	}
	return false;
}
bool Client::confirmClass(uint32_t bay, CARCLASS carClass)
{
	if (bay >= garagedata.car.size() || garagedata.car[bay].carID == 0xFFFFFFFF) return false;
	S_DAT_ENTRY* sDAT = &server->sDAT.sdat[garagedata.car[bay].carID % server->sDAT.count];
	if (carClass == sDAT->carClass) return true;
	return false;
}
uint8_t Client::getCarClass(int32_t bay)
{
	if (bay == -1) bay = getActiveCar();
	S_DAT_ENTRY* sDAT = &server->sDAT.sdat[garagedata.car[bay].carID % server->sDAT.count];
	return sDAT->carClass;
}
bool Client::purchasePart(uint32_t bay, uint8_t itemCategory, uint8_t itemType, uint32_t itemID, bool override)
{
	if (bay >= garagedata.car.size() || garagedata.car[bay].carID == 0xFFFFFFFF) return false;
	S_DAT_ENTRY* sDAT = &server->sDAT.sdat[garagedata.car[bay].carID % server->sDAT.count];
	uint8_t carClass = sDAT->carClass;
	uint8_t shopTable = sDAT->shopTable;
	uint8_t engineType = sDAT->engineType;
	uint8_t transmissionType = (sDAT->gear5Ratio ? 1 : 0) + (sDAT->gear6Ratio ? 1 : 0);
	P_DAT_ENTRY* pDAT = &server->pDAT.pdat[shopTable % server->pDAT.count];

	switch (itemCategory)
	{
	case 1:
	{
		switch (itemType)
		{
		case 1: // Front Bumper
			if ((garagedata.car[bay].parts.frontBumper[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::frontBumper))
			{
				garagedata.car[bay].parts.frontBumper[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		case 2: // Bonnet
			if ((garagedata.car[bay].parts.bonnet[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::bonnet))
			{
				garagedata.car[bay].parts.bonnet[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		case 3: // Mirrors
			if ((garagedata.car[bay].parts.mirrors[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::mirrors))
			{
				garagedata.car[bay].parts.mirrors[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		case 4: // Side Skirts
			if ((garagedata.car[bay].parts.sideSkirts[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::sideSkirts))
			{
				garagedata.car[bay].parts.sideSkirts[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		case 5: // Rear Bumper
			if ((garagedata.car[bay].parts.rearBumper[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::rearBumper))
			{
				garagedata.car[bay].parts.rearBumper[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		case 6: // Rear Spoiler
			if ((garagedata.car[bay].parts.rearSpoiler[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::rearSpoiler))
			{
				garagedata.car[bay].parts.rearSpoiler[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		default: // Over Fender
			if ((garagedata.car[bay].parts.overFenders[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::overFenders))
			{
				garagedata.car[bay].parts.overFenders[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		}
	}
	break;
	case 2:
	{
		switch (itemType)
		{
		case 1: // Lights
			if ((garagedata.car[bay].parts.lights[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::lights))
			{
				garagedata.car[bay].parts.lights[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		default: // Grill
			if ((garagedata.car[bay].parts.grill[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::grill))
			{
				garagedata.car[bay].parts.grill[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		}
	}
	break;
	default:
	{
		switch (itemType)
		{
		case 1: // Muffler
			if ((garagedata.car[bay].parts.muffler[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::muffler))
			{
				garagedata.car[bay].parts.muffler[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		case 2: // Transmission
			if ((garagedata.car[bay].parts.transmission[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::transmission))
			{
				garagedata.car[bay].parts.transmission[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		case 3: // Differential
			if ((garagedata.car[bay].parts.differential[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::differential))
			{
				garagedata.car[bay].parts.differential[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		case 4: // Tire / Brakes use calculated price table instead
			if ((garagedata.car[bay].parts.tyreBrakes[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::tyreBrakes))
			{
				garagedata.car[bay].parts.tyreBrakes[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		case 6: // Suspension
			if ((garagedata.car[bay].parts.suspension[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::suspension))
			{
				garagedata.car[bay].parts.suspension[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		case 7: //Body
			if ((garagedata.car[bay].parts.body[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::body))
			{
				garagedata.car[bay].parts.body[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		default: // Engine
			if ((garagedata.car[bay].parts.engine[itemID] & PART_UNLOCKED || override == true) && itemID < sizeof(PARTS::engine))
			{
				garagedata.car[bay].parts.engine[itemID] |= PART_UNLOCKED | PART_OWNED;
				return true;
			}
			else return false;
		}
	}
	break;
	}
	return false;
}
void Client::addItem(int16_t itemID)
{
	if(itemID >= 0)
		itembox.push_back(itemID);
}
bool Client::removeItem(uint16_t itemID)
{
	for (uint32_t i = 0; i < itembox.size(); i++)
	{
		if (itembox[i] == itemID)
		{
			itembox.erase(itembox.begin() + i);
			return true;
		}
	}
	return false;
}
bool Client::hasItem(uint16_t itemID)
{
	for (auto& item : itembox)
	{
		if (item == itemID) return true;
	}
	return false;
}
bool Client::isValidItem(uint16_t itemID)
{
	return server->isValidItem(itemID);
}
bool Client::isValidCar(uint32_t carID)
{
	return server->isValidCar(carID);
}
int16_t Client::getItemPrice(uint16_t itemID)
{
	if (isValidItem(itemID)) return server->itemData.data[itemID].saleValue;
	else return -1;
}
int16_t Client::getItemResalePrice(uint16_t itemID)
{
	if (isValidItem(itemID)) return server->itemData.data[itemID].resaleValue;
	else return -1;
}
int16_t Client::getShopCarPrice(uint32_t carID)
{
	if (isValidCar(carID) == false) return -1;
	else
	{
		for (uint32_t i = 0; i < server->itemData.count; i++)
		{
			if (server->itemData.data[i].itemType == ITEMTYPES::ITEMTYPE_CAR && server->itemData.data[i].carID == carID) return server->itemData.data[i].resaleValue;
		}
	}
	return -1;
}
int16_t Client::getShopCarResalePrice(uint32_t carID)
{
	if (isValidCar(carID) == false) return -1;
	else
	{
		for (uint32_t i = 0; i < server->itemData.count; i++)
		{
			if (server->itemData.data[i].itemType == ITEMTYPES::ITEMTYPE_CAR && server->itemData.data[i].carID == carID) return server->itemData.data[i].saleValue;
		}
	}
	return -1;
}
uint32_t Client::getCarSalePrice(uint32_t bay)
{
	if (bay >= garagedata.car.size() || garagedata.car[bay].carID == 0xFFFFFFFF) return 0;
	uint32_t carID = garagedata.car[bay].carID;
	uint32_t price = getShopCarResalePrice(carID);
	if(garagedata.car[bay].carMods.engine) price += (uint32_t)((float)(getShopPartPriceFromID(carID, 0, PARTTYPE_ENGINE, garagedata.car[bay].carMods.engine) / RESALEDIVIDER) / 100.0f * min(garagedata.car[bay].engineCondition / 100, 100));
	if(garagedata.car[bay].carMods.muffler) price += (getShopPartPriceFromID(carID, 0, PARTTYPE_MUFFLER, garagedata.car[bay].carMods.muffler) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.transmission) price += (getShopPartPriceFromID(carID, 0, PARTTYPE_TRANSMISSION, garagedata.car[bay].carMods.transmission) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.differential) price += (getShopPartPriceFromID(carID, 0, PARTTYPE_DIFFERENTIAL, garagedata.car[bay].carMods.differential) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.tyrebrakes) price += (getShopPartPriceFromID(carID, 0, PARTTYPE_TYREBRAKE, garagedata.car[bay].carMods.tyrebrakes) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.suspension) price += (getShopPartPriceFromID(carID, 0, PARTTYPE_SUSPENSION, garagedata.car[bay].carMods.suspension) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.body) price += (getShopPartPriceFromID(carID, 0, PARTTYPE_BODY, garagedata.car[bay].carMods.body) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.overfenders) price += (getShopPartPriceFromID(carID, 1, PARTTYPE_OVERFENDER, garagedata.car[bay].carMods.overfenders) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.frontbumper) price += (getShopPartPriceFromID(carID, 1, PARTTYPE_FRONTBUMPER, garagedata.car[bay].carMods.frontbumper) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.bonnet) price += (getShopPartPriceFromID(carID, 1, PARTTYPE_BONNET, garagedata.car[bay].carMods.bonnet) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.mirrors) price += (getShopPartPriceFromID(carID, 1, PARTTYPE_MIRRORS, garagedata.car[bay].carMods.mirrors) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.sideskirts) price += (getShopPartPriceFromID(carID, 1, PARTTYPE_SIDESKIRTS, garagedata.car[bay].carMods.sideskirts) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.rearbumper) price += (getShopPartPriceFromID(carID, 1, PARTTYPE_REARBUMPER, garagedata.car[bay].carMods.rearbumper) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.rearspoiler) price += (getShopPartPriceFromID(carID, 1, PARTTYPE_REARSPOILER, garagedata.car[bay].carMods.rearspoiler) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.grill) price += (getShopPartPriceFromID(carID, 2, PARTTYPE_GRILL, garagedata.car[bay].carMods.grill) / RESALEDIVIDER);
	if(garagedata.car[bay].carMods.lights) price += (getShopPartPriceFromID(carID, 2, PARTTYPE_LIGHTS, garagedata.car[bay].carMods.lights) / RESALEDIVIDER);
	price -= min(price, (uint32_t)(garagedata.car[bay].KMs / 100000.0f));
	price = max(100, price);

	return price;
}
ITEMDATA_ENTRY Client::getItem(uint16_t itemID)
{
	ITEMDATA_ENTRY item = { 0 };
	CopyMemory((uint8_t*)&item, (uint8_t*)&server->itemData.data[itemID], sizeof(ITEMDATA_ENTRY));
	return item;
}
int32_t Client::isInCarShop(uint32_t carID)
{
	for (uint32_t i = 0; i < shopCars.size(); i++) if (shopCars[i] == carID) return i;
	return -1;
}
void Client::processBattleWin()
{
	if (battle.isNPC == true)
	{
		if (currentRival)
		{
			addExp(currentRival->WinXP());
			giveCP(currentRival->WinCP(battle.KMs));
			addItem(currentRival->WinReward());
			if (careerdata.rivalWin < 0xFFFF)
				careerdata.rivalWin++;
			if(currentRival->GetCustom() == false)
				setRivalStatus(currentRival->GetTeamData().teamID, currentRival->GetTeamData().memberID, Rival::RIVALSTATUS::RS_WON);
		}
	}
	else
	{
		if(careerdata.playerWin < 0xFFFF)
			careerdata.playerWin++;

		float exp = battle.KMs / 50.0f;
		exp += battle.SP / 50000;
		exp += battle.challenger->careerdata.level * 10;
		exp += max(0, ((int32_t)battle.challenger->careerdata.playerWin - (int32_t)battle.challenger->careerdata.playerLose)) * 3;
		addExp((uint32_t)exp);
		careerdata.level = getLevel();

		logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has won battle. Which lasted for %0.2f Kms. %u Exp received with %u SP remaining.",
			logger->toWide(handle).c_str(),
			driverslicense,
			logger->toWide((char*)&IP_Address).c_str(),
			courseID,
			battle.KMs,
			(uint32_t)exp,
			battle.SP);
	}
}
void Client::processBattleLose()
{
	if (battle.isNPC == true)
	{
		if (currentRival)
		{
			addExp(currentRival->LoseXP());
			giveCP(currentRival->LoseCP(battle.KMs));
			addItem(currentRival->LoseReward());
			if (careerdata.rivalLose < 0xFFFF)
				careerdata.rivalLose++;
			if (currentRival->GetCustom() == false)
				setRivalStatus(currentRival->GetTeamData().teamID, currentRival->GetTeamData().memberID, Rival::RIVALSTATUS::RS_LOST);
		}
	}
	else
	{
		if (careerdata.playerLose < 0xFFFF)
			careerdata.playerLose++;

		float exp = battle.KMs / 100.0f;
		addExp((uint32_t)exp);
		careerdata.level = getLevel();

		logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has lost battle. Which lasted for %0.2f Kms. %u Exp received with %u SP remaining.",
			logger->toWide(handle).c_str(),
			driverslicense,
			logger->toWide((char*)&IP_Address).c_str(),
			courseID,
			battle.KMs,
			(uint32_t)exp,
			battle.SP);
	}
}
void Client::adjustEngineCondition(int32_t damage1, int32_t damage2, int32_t damage3, int32_t damage4)
{
	float boostAdjust = 1.0f + (float)((garagedata.activeCar->carSettings.boostPressure + 15) / 100.0f);
	float totalwear = (float)(damage1 + damage2 + damage3 + damage4);

	totalwear /= 10000.0f;
	totalwear *= battle.KMs;
	totalwear *= boostAdjust;

	garagedata.activeCar->engineCondition -= min(garagedata.activeCar->engineCondition, (uint32_t)totalwear);

	if (garagedata.activeCar->engineCondition < 4000) // Limit wear to 40%
		garagedata.activeCar->engineCondition = 4000;
}
void Client::addExp(uint32_t exp)
{
	if (careerdata.experiencePoints + exp > server->expToLevel[LEVEL_CAP]) exp = server->expToLevel[LEVEL_CAP] - (careerdata.experiencePoints);
	careerdata.recentExperiancePoints = exp;
	careerdata.experiencePoints += exp;
}
void Client::takeExp(uint32_t exp)
{
	if (careerdata.experiencePoints - exp & 0x80000000) exp = 0;
	careerdata.recentExperiancePoints = exp;
	careerdata.experiencePoints -= exp;
}
void Client::setExp(uint32_t exp)
{
	if (exp > server->expToLevel[LEVEL_CAP]) exp = server->expToLevel[LEVEL_CAP] - (careerdata.experiencePoints);
	careerdata.recentExperiancePoints = exp;
	careerdata.experiencePoints = exp;
}
uint8_t Client::getLevel()
{
	if (LEVEL_CAP > server->expToLevel.size())
	{
		logger->Log(Logger::LOGTYPE_ERROR, L"LEVEL_CAP (%u) is greater than values in expToLevel table size (%u)", LEVEL_CAP, server->expToLevel.size());
		return 1;
	}
	for (int32_t i = 0; i < LEVEL_CAP; i++)
	{
		if (careerdata.experiencePoints < server->expToLevel[i])
		{
			if (i > 0)
			{
				careerdata.experiencePercent = (uint8_t)(((float)(careerdata.experiencePoints - server->expToLevel[i - 1]) / (float)(server->expToLevel[i] - ((i > 1) ? server->expToLevel[i - 1] : 0))) * 100.0f);
				careerdata.level = i;
				return careerdata.level;
			}
		}
	}
	careerdata.experiencePercent = 100;
	careerdata.level = LEVEL_CAP;
	return LEVEL_CAP;
}
void Client::setLevel(uint8_t level)
{
	if (level > LEVEL_CAP) level = LEVEL_CAP;
	else if (level == 0) level++;
	careerdata.level = level;
	careerdata.experiencePoints = server->expToLevel[level - 1];
	careerdata.experiencePercent = (uint8_t)(((float)(careerdata.experiencePoints - server->expToLevel[level - 1]) / (float)(server->expToLevel[level] - ((level > 1) ? server->expToLevel[level - 1] : 0))) * 100.0f);
}
void Client::clearBattle()
{
	if(battle.challenger != nullptr)
	{ 
		battle.challenger->battle.status = BATTLESTATUS::BS_NOT_IN_BATTLE;
		battle.challenger->battle.timeout = 0;
		battle.challenger->battle.challenger = nullptr;
		battle.challenger->battle.initiator = false;
	}
	battle.status = BATTLESTATUS::BS_NOT_IN_BATTLE;
	battle.timeout = 0;
	battle.challenger = nullptr;
	battle.initiator = false;
	if (currentRival != nullptr)
		currentRival = nullptr;
}
Rival * Client::getRival(uint32_t RivalID)
{
	for (auto& rival : rivals)
	{
		if (rival.GetID() == RivalID)
		{
			return &rival;
		}
	}
	return nullptr;
}
void Client::getRivals()
{
	// DISABLE FOR LIVE
	//return;
	clearRivals();

	// TODO: Do this properly
	for (uint32_t i = 0; i < 3; i++)
	{
		uint32_t currentID = i;
		if (i == 0)
			currentID = 800;
		//Rival* newRival = new Rival(this, currentID);
		Rival newRival(this, currentID);
		if (!newRival.SetRivalID(currentID))
		{
			//delete newRival;
			continue;
		}
		newRival.SetID(i);
		//newRival.Random();
		newRival.SpaceTick(i, 12);
		rivals.push_back(newRival);
	}
}
void Client::clearRivals()
{
	//for (auto& rival : rivals)
	//{
	//	delete rival;
	//}
	rivals.clear();
	rivals.shrink_to_fit();
}
void Client::setRivalStatus(uint32_t TeamID, uint8_t MemberID, uint8_t Status)
{
	if (TeamID > (sizeof(careerdata.rivalStatus) / sizeof(RIVAL_STATUS)) || MemberID > 7 || (currentRival && currentRival->GetCustom()))
		return;

	if (Status == Rival::RIVALSTATUS::RS_WON)
		careerdata.rivalStatus[TeamID].wins++;

	if ((careerdata.rivalStatus[TeamID].rivalMember[MemberID] != Rival::RIVALSTATUS::RS_WON) ||
		(careerdata.rivalStatus[TeamID].rivalMember[MemberID] == Rival::RIVALSTATUS::RS_HIDDEN && Status == Rival::RIVALSTATUS::RS_SHOW))
		careerdata.rivalStatus[TeamID].rivalMember[MemberID] = Status;
}
int32_t Client::getSign(uint16_t id)
{
	if (id > sizeof(sign))
		return -1;
	return sign[id];
}
void Client::enableSign(uint16_t id)
{
	if (id > sizeof(sign))
		return;
	sign[id] = 1;
}
void Client::disableSign(uint16_t id)
{
	if (id > sizeof(sign))
		return;
	sign[id] = 0;
}
void Client::CreateTeam(std::string& teamname)
{
	if (inTeam() == false)
		server->managementserver.TeamCreate(driverslicense, teamname);
}
void Client::DeleteTeam()
{
	if (inTeam() && isTeamLeader())
		server->managementserver.TeamDelete(driverslicense);
}
void Client::RemoveTeamMember(uint32_t memberID)
{
	if (inTeam() && isTeamLeader())
		server->managementserver.TeamRemoveTeamMember(driverslicense, memberID);
}
void Client::UpdateTeamInviteOnly(uint8_t invitestatus)
{
	if (inTeam() && isTeamLeader())
		server->managementserver.TeamUpdateInviteOnly(driverslicense, invitestatus);
}
void Client::UpdateTeamComment(std::string& comment)
{
	if (inTeam() && isTeamLeader())
		server->managementserver.TeamUpdateComment(driverslicense, comment);
}
void Client::UpdateTeamMemberAreaAccess(uint32_t memberID, uint8_t access)
{
	if (inTeam() && isTeamLeader())
		server->managementserver.TeamSetAreaAccess(driverslicense, memberID, access);
}
void Client::GetTeamData()
{
	if(inTeam())
		server->managementserver.TeamGetData(driverslicense);
}
bool Client::inTeam()
{
	if (teamdata.teamID == 0xFFFFFFFF)
		return false;
	else
		return true;
}
bool Client::isTeamLeader()
{
	if (inTeam() == true)
	{
		if (strcmp(&teamdata.leaderName[0], &handle[0]) == 0)
			return true;
		else
			return false;
	}
	return false;
}
void Client::SendPing()
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0xA00);
	outbuf.setSubType(0xA80);
	outbuf.append<uint32_t>(timeGetTime());
	Send();
}
void Client::SendWelcome(uint8_t type, std::string& name)
{
	sendWelcome = 0;
	isClient = true;
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x100);
	outbuf.setSubType(0x180);
	outbuf.append<uint8_t>(type); // Type ????
	outbuf.appendString(name, 0x08); // Server Name ????
	Send();
}
void Client::SendAcceptConnection()
{
	// Byte at 0x07 is either beginner flag or server number. Initial connections are 0 but connections to beginner is 1
	uint32_t clientVer = inbuf.getVer() & 0xFFFFFF00;
	if (clientVer == 0x53427B00 /* new client */)
		//if (clientVer == 0x544D0100 /* Tofuman client */)
	{
		if (server->getStatus() == Server::SS_RUNNING && server->getConnectedToManagementServer())
		{
			sendWelcome = 0;
			if (setUsername(std::string((char*)&inbuf.buffer[0x08])))
			{
				SendAuthError(Server::AUTHLIST::AUTH_INVALID_PW);
				return;
			}
			else
			{
				SendWelcome(0, std::string("\0\1\2\4\5\6\7"));
				return;
			}
		}
		else
		{
			SendAuthError(Server::AUTHLIST::AUTH_BUSY);
			return;
		}
	}
	else
	{
		SendAuthError(Server::AUTHLIST::AUTH_OUTDATED);
		return;
	}
}
void Client::SendAuthError(std::string& cmd)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x100);
	outbuf.setSubType(0x181);
	outbuf.append<uint8_t>(7);
	outbuf.appendString(cmd, 0x78);
	Send();
}
void Client::SendAuthError(uint8_t cmd)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x7B);
	outbuf.setOffset(0x06);
	outbuf.setType(0x100);
	outbuf.setSubType(0x181);
	outbuf.append<uint8_t>(cmd);
	Send();
}
void Client::SendCareerRecord()
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0xC00);
	outbuf.setSubType(0xC80);
	outbuf.append<uint16_t>(careerdata.playerWin + careerdata.playerLose); // Player total
	outbuf.append<uint16_t>(careerdata.playerWin); // VS Player Win
	outbuf.append<uint16_t>(careerdata.playerLose); // VS Player Lose
	outbuf.append<uint16_t>(careerdata.rivalWin + careerdata.rivalLose); // Rival total
	outbuf.append<uint16_t>(careerdata.rivalWin); // VS Rival Win
	outbuf.append<uint16_t>(careerdata.rivalLose); // VS Rival Lose
	Send();
}
void Client::SendCourseJoin(uint8_t notify)
{
	// Clear active sign as it seems this isn't saved.
	activeSign = 0;

	// Send current client joining course to other players in course
	coursebuf.clearBuffer();
	coursebuf.setSize(0x06);
	coursebuf.setOffset(0x06);
	coursebuf.setType(0x400);
	coursebuf.setSubType(0x480);
	coursebuf.append<uint16_t>(courseID); // ID
	coursebuf.appendString(handle, 0x10); // Player Name
	coursebuf.append<uint32_t>(timeGetTime()); // Time maybe ????
	coursebuf.append<uint8_t>(careerdata.level); // Level
	// 0x6C byte array - Car Data
	coursebuf.append<uint32_t>(0, false); // ????
	coursebuf.append<uint32_t>(garagedata.activeCar->carID, false); // Car ID
	coursebuf.appendArray((uint8_t*)&garagedata.activeCar->carMods, sizeof(CARMODS) - sizeof(CARMODS::tuner));
	coursebuf.appendArray((uint8_t*)&garagedata.activeCar->carSettings, sizeof(CARSETTINGS)); // Car Settings
	coursebuf.append<uint16_t>(0x00, false); // ????
	// Positioning
	coursebuf.append<uint16_t>(position.location1 ? position.location1 : startJunction, false); // Junction
	coursebuf.append<uint16_t>(position.location2 ? position.location2 : startDistance, false); // Distance
	coursebuf.append<uint16_t>(position.location3 ? position.location3 : 0x32, false); // ???
	coursebuf.append<uint16_t>(0xFFFF, false); // Set 0xFFFF to auto place in area
	// Flags
	coursebuf.append<uint8_t>(1); // npc stuff must be below 0x20. 0x00 = Self, 0x01 = Player, 0x02 = NPC
	coursebuf.append<uint8_t>(0); // 1st bit arrow is flashin (if player), 7th bit arrow is green, 8th bit arrow is red
	coursebuf.append<uint16_t>(status, false); // 1st bit car is in safe mode, When 3rd bit set car is transparent
	coursebuf.append<uint8_t>(0); // ???? 
	coursebuf.append<uint8_t>(0); // ????
	// 0x6C byte array - Team Data
	coursebuf.appendArray((uint8_t*)&teamdata, sizeof(TEAMDATA));
	coursebuf.append<uint8_t>(notify); // If set with player notification of player entering is displayed.
	coursebuf.append<uint16_t>(0); // Count for something requires the shorts as many as these
	coursebuf.append<uint32_t>(activeSign); // Icon next to name. if player.
	SendToCourse(&coursebuf, true);

	// Get Players in course
	if (hasPlayers == false)
		hasPlayers = true;

	if (course != nullptr && course->getCourse() == currentCourse) course->sendCourseRacers(driverslicense);

	// TODO: Beginner course should have ROWING GUY and ROLLING KIDS teams for the moment no rivals in beginner
	if (currentCourse == COURSE_MAIN && notBeginner)
	{	// Rivals are only on main course
		getRivals();
		SendRivalJoin();
	}
}
void Client::SendRivalRecords()
{
	int32_t count = static_cast<int32_t>(inbuf.get<uint8_t>(0x04));
	if (inbuf.getSize() < 5 + (count * sizeof(int32_t)) || count > sizeof(careerdata.rivalStatus) / sizeof(RIVAL_STATUS))
	{
		logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has sent invalid 0xC01 packet.",
			logger->toWide(handle).c_str(),
			driverslicense,
			logger->toWide((char*)&IP_Address).c_str()
		);
		Disconnect();
		return;
	}
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0xC00);
	outbuf.setSubType(0xC81);

	outbuf.append<uint8_t>(count); // Rival count should be 0x64 as even removed rival ID's are processed

	uint8_t status_remaps[] = { 3, 0, 1, 2 };
	inbuf.addOffset(0x05);
	for (int32_t i = 0; i < count; i++)
	{
		uint32_t currentID = inbuf.get<uint32_t>();
		if (currentID >= sizeof(careerdata.rivalStatus) / sizeof(RIVAL_STATUS))
		{
			logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has sent invalid 0xC01 packet.",
				logger->toWide(handle).c_str(),
				driverslicense,
				logger->toWide((char*)&IP_Address).c_str()
			);
			Disconnect();
			return;
		}
		outbuf.append<uint32_t>(currentID); // Rival id. Must be loop index. Game is missing some teams.
		int32_t check = 0;
		for (int32_t j = 0; j < 8; j++) check += careerdata.rivalStatus[currentID].rivalMember[j] ? 1 : 0;
		outbuf.append<uint8_t>(check ? 0x08 : 0x00); // Team count
		// 0 - Seen / Show
		// 1 - Lost
		// 2 - Won
		// 3 - Not Seen / Hide
		if (check)
		{
			outbuf.append<uint8_t>(status_remaps[careerdata.rivalStatus[currentID].rivalMember[0x00] % sizeof(status_remaps)]); // Rival 1 // Boss
			outbuf.append<uint8_t>(status_remaps[careerdata.rivalStatus[currentID].rivalMember[0x01] % sizeof(status_remaps)]); // Rival 2
			outbuf.append<uint8_t>(status_remaps[careerdata.rivalStatus[currentID].rivalMember[0x02] % sizeof(status_remaps)]); // Rival 3
			outbuf.append<uint8_t>(status_remaps[careerdata.rivalStatus[currentID].rivalMember[0x03] % sizeof(status_remaps)]); // Rival 4
			outbuf.append<uint8_t>(status_remaps[careerdata.rivalStatus[currentID].rivalMember[0x04] % sizeof(status_remaps)]); // Rival 5
			outbuf.append<uint8_t>(status_remaps[careerdata.rivalStatus[currentID].rivalMember[0x05] % sizeof(status_remaps)]); // Rival 6
			outbuf.append<uint8_t>(status_remaps[careerdata.rivalStatus[currentID].rivalMember[0x06] % sizeof(status_remaps)]); // Rival 7
			outbuf.append<uint8_t>(status_remaps[careerdata.rivalStatus[currentID].rivalMember[0x07] % sizeof(status_remaps)]); // Rival 8 // Lowest
		}
		//outbuf.append<uint32_t>(careerdata.rivalStatus[currentID].wins); // Boss Defeats I don't think this should be the wins as there is a limit of a few hundred. Maybe unlocked members?
		outbuf.append<uint32_t>(check);
	}
	Send();
}
void Client::SendRivalJoin()
{
	// DISABLE FOR LIVE
	//return;
	if (rivals.size() == 0) return;

	for (auto& rival : rivals)
	{
		outbuf.clearBuffer();
		outbuf.setSize(0x06);
		outbuf.setOffset(0x06);
		outbuf.setType(0x400);
		outbuf.setSubType(0x480);
		outbuf.append<uint16_t>(rival.GetID()); //ID
		outbuf.appendString(std::string(rival.GetName()), 0x10); // Rival Name
		outbuf.append<uint32_t>(timeGetTime()); // Time maybe ????
		outbuf.append<uint8_t>(rival.GetLevel()); // Level
		outbuf.append<uint32_t>(0, false); // ????
		outbuf.append<int32_t>(rival.GetCar(), false); // Car ID
		outbuf.appendArray((uint8_t*)rival.GetCarModsPtr(), sizeof(CARMODS) - sizeof(CARMODS::tuner)); // Rivals don't have tuner names
		outbuf.appendArray((uint8_t*)rival.GetCarSettingsPtr(), sizeof(CARSETTINGS)); // Car Settings
		outbuf.append<uint16_t>(0); // ???
		outbuf.append<uint16_t>(rival.GetPosition().location1, false); // Junction
		outbuf.append<uint16_t>(rival.GetPosition().location2, false); // Distance
		outbuf.append<uint16_t>(rival.GetPosition().location3, false); // ???
		outbuf.append<uint16_t>(0xFFFF, false); // Set 0xFFFF to auto place in area
		outbuf.append<uint8_t>(2); // npc stuff must be below 0x20. 0x00 = Self, 0x01 = Player, 0x02 = NPC
		// TODO: Has rival been beaten or lost to?
		outbuf.append<uint8_t>(0); // 1st bit arrow is flashin (if player), 7th bit arrow is green, 8th bit arrow is red
		outbuf.append<uint16_t>(0, false); // 1st bit car is in safe mode, When 3rd bit set car is transparent
		outbuf.append<uint8_t>(0); // ???? 
		outbuf.append<uint8_t>(0); // ????
		// 0x6C byte array - Team Data
		outbuf.appendArray((uint8_t*)rival.GetTeamDataPtr(), sizeof(TEAMDATA));
		outbuf.append<uint8_t>(0); // If set with player notification of player entering is displayed.
		outbuf.append<uint16_t>(0); // Count for something requires the shorts as many as these
		outbuf.append<uint32_t>(0); // Icon next to name. if player.
		Send();
	}
}
void Client::SendPositionBrief()
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x700);
	outbuf.setSubType(0x780);
	outbuf.append<uint8_t>(currentCourse); // Course Number
	outbuf.append<uint16_t>(1); // count
	outbuf.append<uint16_t>(courseID); // ID
	outbuf.append<uint16_t>(position.location2); // 700 / 0x3BC limit - Junction
	outbuf.append<uint16_t>(position.location3); // Shifted right 5 - Position
	outbuf.append<uint16_t>(position.location1); // 0x7FFF limit
	outbuf.append<uint32_t>(position.time); // Epoch Time
	SendToProximity(position.posX1, position.posY1, &outbuf, true);
}
void Client::SendRivalPosition()
{
	// DISABLE FOR LIVE
	//return;
	if (currentCourse != 0) return;
	if (rivals.size() == 0) return;

	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x700);
	outbuf.setSubType(0x780);
	outbuf.append<uint8_t>(currentCourse); // Course Number
	outbuf.append<uint16_t>(static_cast<uint16_t>(rivals.size())); // count

	for (auto& rival : rivals)
	{
		rival.Tick();
		outbuf.append<uint16_t>(rival.GetID()); // ID
		outbuf.append<uint16_t>(rival.GetPosition().location2); // 700 / 0x3BC limit - Junction
		outbuf.append<uint16_t>(rival.GetPosition().location3); // Shifted right 5 - Position
		outbuf.append<uint16_t>(rival.GetPosition().location1); // 0x7FFF limit
		outbuf.append<uint32_t>(rival.GetPosition().time); // Epoch Time
	}

	// If in battle don't bother sending position but we still need to tick.
	if(battle.status == BATTLESTATUS::BS_NOT_IN_BATTLE)
		Send();
	//logger->Log(Logger::LOGTYPE_Client, L"0x%04X,0x%04X,0x%04X", position.location1, position.location2, position.location3);
}
void Client::SendRemoveRivals()
{
	for (auto& rival : rivals)
	{
		outbuf.clearBuffer();
		outbuf.setSize(0x06);
		outbuf.setOffset(0x06);
		outbuf.setType(0x400);
		outbuf.setSubType(0x481);
		outbuf.append<uint16_t>(rival.GetID()); // ID to remove
		Send();
	}
	clearRivals();
}
void Client::SendPosition()
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x700);
	outbuf.setSubType(0x700);
	outbuf.append<uint16_t>(courseID); // ID of player used in index of function table for 0x700 inbound packets.
	position.accuracy = ((positionCount++ % POSITION_ACCURACY) + 1) << 8 | POSITION_ACCURACY;
	position.accuracy = SWAP_SHORT(position.accuracy);
	outbuf.appendArray((uint8_t*)&position, sizeof(POSITION));
	SendToProximity(position.posX1, position.posY1, &outbuf, true);
}
void Client::SendPositionToOpponent()
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x700);
	outbuf.setSubType(0x700);
	outbuf.append<uint16_t>(courseID); // ID of player used in index of function table for 0x700 inbound packets.
	outbuf.append<uint16_t>(position.accuracy);
	outbuf.appendArray((uint8_t*)&position.location1, sizeof(POSITION) - sizeof(POSITION::accuracy));
	SendToOpponent();
}
void Client::SendChatMessage(CHATTYPE type, std::string& handle, std::string& message, uint32_t license, std::string& fromHandle)
{
	PACKET* chatBuf;
	if (handle.empty())
		handle = "NULL";

	if (fromHandle.empty())
		fromHandle = "NULL";

	if (license != 0)
		chatBuf = &outbuf;
	else
		chatBuf = &coursebuf;
	chatBuf->clearBuffer();
	chatBuf->setSize(0x06);
	chatBuf->setOffset(0x06);
	chatBuf->setType(0x600);
	chatBuf->setSubType(0x600 + type);
	switch (type)
	{
		case CHATTYPE_NORMAL:
			chatBuf->appendString(handle, 0x10);
			break;
		case CHATTYPE_ADMIN:
			chatBuf->appendString(handle, 0x10);
			break;
		case CHATTYPE_ANNOUNCE:
			chatBuf->appendString(handle, 0x10);
			break;
		case CHATTYPE_NOTIFICATION:
			chatBuf->appendString(handle, 0x10);
			break;
		case CHATTYPE_EVENT:
			chatBuf->appendString(handle, 0x10);
			break;
		case CHATTYPE_PRIVATE:
			chatBuf->appendString(handle, 0x10);
			chatBuf->appendString(fromHandle, 0x10);
			break;
		default:
			chatBuf->appendString(handle, 0x10);
			break;
	}
	chatBuf->appendString(message, 0x4E);
	if (license != 0)
	{
		if(course != nullptr) course->sendToClient(chatBuf, license);
		else if(license == driverslicense) Send();
	}
	else
		SendToCourse();
}
void Client::SendTeamChatMessage(std::string& fromHandle, std::string& message, uint32_t teamID)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x600);
	outbuf.setSubType(0x603);
	outbuf.appendString(fromHandle, 0x10);
	outbuf.appendString(message, 0x4E);
	outbuf.append<uint32_t>(teamID ? teamID : teamdata.teamID);
	Send();
}
void Client::SendAnnounceMessage(std::string& message, uint32_t colour, uint32_t license)
{
	PACKET* chatBuf;
	if (license != 0)
		chatBuf = &outbuf;
	else
		chatBuf = &coursebuf;
	chatBuf->clearBuffer();
	chatBuf->setSize(0x06);
	chatBuf->setOffset(0x06);
	chatBuf->setType(0x600);
	chatBuf->setSubType(0x6F0);
	chatBuf->appendString(message, 0x4E);
	chatBuf->append<uint8_t>(GetRValue(colour));
	chatBuf->append<uint8_t>(GetGValue(colour));
	chatBuf->append<uint8_t>(GetBValue(colour));
	if (license != 0)
	{
		if (course != nullptr) course->sendToClient(chatBuf, license);
		if (license != driverslicense) Send();
	}
	else
		SendToCourse();
}
void Client::SendPlayerStats()
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x1000);
	outbuf.setSubType(0x1080);
	outbuf.append<int64_t>(getCP());
	outbuf.append<uint8_t>(getLevel()); // Level
	outbuf.append<uint32_t>(careerdata.experiencePoints); // Exp Points?
	outbuf.append<uint8_t>(careerdata.experiencePercent); // Exp %
	Send();
}
void Client::SendItems()
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x1100);
	outbuf.setSubType(0x1180);
	outbuf.append<uint8_t>(static_cast<uint8_t>(min(itembox.size(), ITEMBOX_LIMIT)));
	for (uint32_t i = 0; i < min(itembox.size(), ITEMBOX_LIMIT); i++) outbuf.append<int16_t>(itembox.at(i));
	Send();
}
void Client::SendSigns()
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x1600);
	outbuf.setSubType(0x1680);
	uint16_t count = 0;
	uint8_t tempSign[54] = { 0 };
	for (int32_t i = 0; i < sizeof(sign); i++)
	{
		if (sign[i] != 0)
			tempSign[count++] = i + 1;
	}
	outbuf.append<uint16_t>(count);
	for (uint16_t i = 0; i < count; i++) outbuf.append<uint32_t>(tempSign[i] + 10);
	Send();
}
void Client::GetCarSettings(uint32_t bay)
{
	if (bay >= (uint32_t)(garagedata.garageCount * 4) || garagedata.car[bay].carID == 0xFFFFFFFF)
	{
		logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u has specified invalid bay number when requesting car data.",
			logger->toWide(handle).c_str(),
			driverslicense,
			logger->toWide((char*)&IP_Address).c_str()
		);
		Disconnect();
		return;
	}

	if (!garagedata.activeCar)
	{
		logger->Log(Logger::LOGTYPE_Client, L"Client has no active car.");
		Disconnect();
		return;
	}
	SendCarData(bay);
}
void Client::SendCarData(uint32_t bay)
{
	if (bay >= garagedata.car.size() || bay >= (uint32_t)(garagedata.garageCount * 4) || garagedata.car[bay].carID == 0xFFFFFFFF) return;
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x200);
	outbuf.setSubType(0x281);
	outbuf.append<uint32_t>(bay + 1); // Car bay
	outbuf.append<uint32_t>(garagedata.car[bay].carID); // Car ID
	outbuf.append<uint32_t>((uint32_t)(garagedata.car[bay].KMs / 100.0f)); // KMs
	outbuf.append<uint8_t>(0);
	outbuf.append<uint16_t>(static_cast<uint16_t>(garagedata.car[bay].engineCondition / 100)); // Engine Condition
	outbuf.append<uint8_t>(0); // Engine Status: 0 None, 1 Right, 2 Down
	outbuf.appendArray((uint8_t*)&garagedata.car[bay].carMods, sizeof(CARMODS));
	outbuf.append<uint16_t>(garagedata.car[bay].status); // Status. 1: for sale
	Send();
}
void Client::SendBayDetails()
{
	uint32_t limit = (uint32_t)(garagedata.garageCount * 4);
	for (uint32_t i = 0; i < limit; i++)
	{
		if (garagedata.car[i].carID != 0xFFFFFFFF)
		{
			outbuf.clearBuffer();
			outbuf.setSize(0x06);
			outbuf.setOffset(0x06);
			outbuf.setType(0x200);
			outbuf.setSubType(0x280);
			outbuf.append<uint8_t>(1); // byte moved to stack if not set process the 8 byte packet else structure continues
			outbuf.append<uint32_t>(i / 4); // int moved to stack. if below value is 0 then this value is shifted left by 4 then added to pointer address at 0x6d1318 + 04 (crashes if greater than 0) - Probaby 2nd and 3rd garages
			outbuf.append<uint8_t>((uint8_t)i % 4); // byte moved to stack another function is run 41fda0, Garage Bay number
			outbuf.append<uint32_t>(i + 1); // Associated Car Slot
			Send();
		}
	}
}
void Client::SwitchCar(uint32_t bay)
{
	if (bay > garagedata.car.size() || bay < 1)
	{
		Disconnect();
		return;
	}
	if (garagedata.car[bay - 1].carID == 0xFFFFFFFF)
	{
		Disconnect();
		return;
	}
	setActiveCar(bay - 1);
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x200);
	outbuf.setSubType(0x282);
	outbuf.append<uint32_t>(bay);
	Send();
}
void Client::PurchaseCar(uint32_t id, COLOUR2 colour, uint32_t cost)
{
	if (isInCarShop(id) == -1)
	{
		logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to purchase car not in shop. Car ID %u.",
			logger->toWide(handle).c_str(),
			driverslicense,
			logger->toWide((char*)&IP_Address).c_str(),
			id
		);
		Disconnect();
		return;
	}

	uint16_t finalPrice = getShopCarPrice(id);
	if (finalPrice == -1 || cost != finalPrice)
	{
		logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has provided different price for car than expected. Car ID %u, Cost %u, Real Cost %u.",
			logger->toWide(handle).c_str(),
			driverslicense,
			logger->toWide((char*)&IP_Address).c_str(),
			id,
			cost,
			finalPrice
		);
		Disconnect();
		return;
	}

	int32_t emptyBay = getEmptyBay();
	if (emptyBay == -1 || emptyBay > 3)
	{
		logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to purchase car with no room in garage. Car ID %u.",
			logger->toWide(handle).c_str(),
			driverslicense,
			logger->toWide((char*)&IP_Address).c_str(),
			id
		);
		Disconnect();
		return;
	}

	if (enoughCP(finalPrice))
	{
		if (addCar(id, emptyBay, colour) == true) takeCP(finalPrice);
		else
		{
			logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to purchase car from the car but there was an error adding car to garage. Car ID %u, Car count: %u.",
				logger->toWide(handle).c_str(),
				driverslicense,
				logger->toWide((char*)&IP_Address).c_str(),
				id,
				getCarCount()
			);
			Disconnect();
			return;
		}
	}
	else
	{
		logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) has tried to purchase car from the car shop without enough CP. Car ID %u, Client CP %u, Real Cost %u.",
			logger->toWide(handle).c_str(),
			driverslicense,
			logger->toWide((char*)&IP_Address).c_str(),
			id,
			getCP(),
			finalPrice
		);
		Disconnect();
		return;
	}

	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x200);
	outbuf.setSubType(0x283);
	outbuf.append<int32_t>(emptyBay + 1); // Available Slot
	outbuf.append<int64_t>(getCP());
	Send();
}
void Client::SendCarData()
{
	uint32_t limit = (uint32_t)(garagedata.garageCount * 4);
	for (uint32_t i = 0; i < limit; i++)
	{
		if (garagedata.car[i].carID != 0xFFFFFFFF)
		{
			outbuf.clearBuffer();
			outbuf.setSize(0x06);
			outbuf.setOffset(0x06);
			outbuf.setType(0x200);
			outbuf.setSubType(0x281);
			outbuf.append<uint32_t>(i + 1); // Car bay
			outbuf.append<uint32_t>(garagedata.car[i].carID); // Car ID
			outbuf.append<uint32_t>((uint32_t)(garagedata.car[i].KMs / 100.0f)); // KMs
			outbuf.append<uint8_t>(0);
			outbuf.append<uint16_t>(static_cast<uint16_t>(garagedata.car[i].engineCondition / 100)); // Engine Condition
			outbuf.append<uint8_t>(0); // Engine Status: 0 None, 1 Right, 2 Down
			outbuf.appendArray((uint8_t*)&garagedata.car[i].carMods, sizeof(CARMODS));
			outbuf.append<uint16_t>(garagedata.car[i].status); // Status. 1: for sale
			Send();
		}
	}
}
void Client::SendBattleChallengeNPC(uint16_t RivalID, uint32_t _time)
{
	battle.isNPC = true;
	battle.status = BATTLESTATUS::BS_IN_BATTLE;
	battle.SP = battle.lastSP = INITIALBATTLE_SP;
	battle.KMs = 0.0f;
	battle.initiator = true;
	battle.spCount = 0;
	battle.timeout = 0;
	currentRival = getRival(RivalID);

	if (currentRival != nullptr)
	{
		setRivalStatus(currentRival->GetTeamData().teamID, currentRival->GetTeamData().memberID, Rival::RIVALSTATUS::RS_SHOW);
		outbuf.clearBuffer();
		outbuf.setSize(0x06);
		outbuf.setOffset(0x06);
		outbuf.setType(0x500);
		outbuf.setSubType(0x584);
		outbuf.append<uint16_t>(courseID); // Client's course ID
		outbuf.append<uint16_t>(RivalID); // Rival's ID (the ID assigned as it joined the course)
		outbuf.append<uint32_t>(_time); // Time
		outbuf.appendArray((uint8_t*)currentRival->GetRivalDifficultyPtr(), sizeof(RIVALDIFFICULTY)); // Difficulty settings. Has values that specify how the NPC races and traffic
		outbuf.append<uint16_t>(currentRival->GetPosition().location2); // Distance
		outbuf.append<uint16_t>(currentRival->GetPosition().location3); // Unknown Position
		outbuf.append<uint16_t>(currentRival->GetPosition().location1); // Junction
		outbuf.append<uint32_t>(currentRival->GetPosition().time); // Time
		Send();
		return;
	}
	else
	{
		// Rival not found
		battle.status = BATTLESTATUS::BS_NOT_IN_BATTLE;
		SendBattleNPCAbort(0);
	}
}
void Client::SendBattleChallenge(uint16_t challengeID, uint16_t clientID, uint32_t _time)
{
	battle.status = BATTLESTATUS::BS_INIT_BATTLE;
	battle.SP = battle.lastSP = INITIALBATTLE_SP;
	battle.KMs = 0.0f;
	battle.initiator = true;
	battle.spCount = 0;
	battle.timeout = time(NULL) + BATTLE_TIMEOUT;

	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x500);
	outbuf.setSubType(0x580);
	outbuf.append<uint16_t>(challengeID);
	outbuf.append<uint16_t>(clientID);
	outbuf.append<uint32_t>(_time); // Time
	outbuf.append<uint16_t>(position.location2);
	outbuf.append<uint16_t>(position.location3);
	outbuf.append<uint16_t>(position.location1);
	outbuf.append<uint32_t>(position.time);
	Send();
}
void Client::SendBattleChallengeToOpponent(uint16_t challengeID, uint16_t clientID, uint32_t _time)
{
	if (battle.challenger == nullptr) return;
	battle.challenger->battle.status = BATTLESTATUS::BS_INIT_BATTLE;
	battle.challenger->battle.SP = battle.challenger->battle.lastSP = INITIALBATTLE_SP;
	battle.challenger->battle.KMs = 0.0f;
	battle.challenger->battle.spCount = 0;
	battle.challenger->battle.timeout = time(NULL) + BATTLE_TIMEOUT;
	battle.challenger->battle.challenger = this;

	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x500);
	outbuf.setSubType(0x580);
	outbuf.append<uint16_t>(challengeID);
	outbuf.append<uint16_t>(clientID);
	outbuf.append<uint32_t>(_time); // Time
	outbuf.append<uint16_t>(position.location2);
	outbuf.append<uint16_t>(position.location3);
	outbuf.append<uint16_t>(position.location1);
	outbuf.append<uint32_t>(position.time);
	SendToOpponent();
}
void Client::SendBattleAbort(uint8_t res)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x500);
	outbuf.setSubType(0x581);
	outbuf.append<uint8_t>(res);
	battle.status = BATTLESTATUS::BS_NOT_IN_BATTLE;
	Send();
}
void Client::SendBattleNPCAbort(uint8_t res)
{
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x500);
	outbuf.setSubType(0x585);
	outbuf.append<uint8_t>(res);
	battle.status = BATTLESTATUS::BS_NOT_IN_BATTLE;
	Send();
}
void Client::SendBattleStart()
{
	if (battle.challenger == nullptr) return;
	battle.status = BATTLESTATUS::BS_IN_BATTLE;
	battle.timeout = 0;
	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x500);
	outbuf.setSubType(0x582);
	Send();
}
void Client::SendBattleCheckStatus()
{
	if (battle.challenger == nullptr) return;

	if (battle.status == BATTLESTATUS::BS_WON || battle.status == BATTLESTATUS::BS_LOST)
	{
		if (abs(battle.challenger->battle.spCount - battle.spCount) > 10)
		{
			battle.challenger->logger->Log(Logger::LOGTYPE_Client, L"Client %s (%u / %s) with ID %u may have altered the battle SP. %u SP. SP Count %u",
				battle.challenger->logger->toWide(battle.challenger->handle).c_str(),
				battle.challenger->driverslicense,
				battle.challenger->logger->toWide((char*)&battle.challenger->IP_Address).c_str(),
				battle.challenger->courseID,
				battle.challenger->battle.SP,
				battle.challenger->battle.spCount
			);
		}

		outbuf.clearBuffer();
		outbuf.setSize(0x06);
		outbuf.setOffset(0x06);
		outbuf.setType(0x500);
		outbuf.setSubType(0x583);

		outbuf.append<uint8_t>(0);
		outbuf.append<uint8_t>(1);

		if (battle.status == BATTLESTATUS::BS_LOST)
		{
			processBattleLose();
			battle.challenger->processBattleWin();

			// Win Table
			outbuf.append<uint16_t>(battle.challenger->courseID);							// Client Course ID
			outbuf.append<uint8_t>(battle.challenger->careerdata.level);					// Level
			outbuf.append<uint32_t>(battle.challenger->careerdata.recentExperiancePoints);	// EXP Gained
			outbuf.append<uint8_t>(battle.challenger->careerdata.experiencePercent);		// EXP Percentage
			outbuf.append<uint32_t>(battle.challenger->careerdata.ranking);					// Ranking

			// Lose Table
			outbuf.append<uint16_t>(courseID);							// Client Course ID
			outbuf.append<uint8_t>(careerdata.level);					// Level
			outbuf.append<uint32_t>(careerdata.recentExperiancePoints);	// EXP Gained
			outbuf.append<uint8_t>(careerdata.experiencePercent);		// EXP Percentage
			outbuf.append<uint32_t>(careerdata.ranking);				// Ranking
		}
		else
		{
			processBattleWin();
			battle.challenger->processBattleLose();

			// Win Table
			outbuf.append<uint16_t>(courseID);							// Client Course ID
			outbuf.append<uint8_t>(careerdata.level);					// Level
			outbuf.append<uint32_t>(careerdata.recentExperiancePoints);	// EXP Gained
			outbuf.append<uint8_t>(careerdata.experiencePercent);		// EXP Percentage
			outbuf.append<uint32_t>(careerdata.ranking);				// Ranking

			// Lose Table
			outbuf.append<uint16_t>(battle.challenger->courseID);							// Client Course ID
			outbuf.append<uint8_t>(battle.challenger->careerdata.level);					// Level
			outbuf.append<uint32_t>(battle.challenger->careerdata.recentExperiancePoints);	// EXP Gained
			outbuf.append<uint8_t>(battle.challenger->careerdata.experiencePercent);		// EXP Percentage
			outbuf.append<uint32_t>(battle.challenger->careerdata.ranking);					// Ranking
		}

		Send();
		SendToOpponent();
		clearBattle();
	}
}
void Client::SendBattleNPCFinish()
{
	if (battle.isNPC == false || currentRival == nullptr) return;
	if (battle.status == Client::BATTLESTATUS::BS_WON) processBattleWin();
	else if (battle.status == Client::BATTLESTATUS::BS_LOST) processBattleLose();

	outbuf.clearBuffer();
	outbuf.setSize(0x06);
	outbuf.setOffset(0x06);
	outbuf.setType(0x500);
	outbuf.setSubType(0x586);
	outbuf.append<uint8_t>(getLevel());// (battle.status == BATTLESTATUS::WON) ? 0 : 1);
	outbuf.append<uint32_t>((battle.status == Client::BATTLESTATUS::BS_WON) ? currentRival->WinXP() : currentRival->LoseXP()); // XP Gained
	outbuf.append<uint8_t>(careerdata.experiencePercent); // XP Percentage
	outbuf.append<uint32_t>((battle.status == Client::BATTLESTATUS::BS_WON) ? currentRival->WinCP(battle.KMs) : currentRival->LoseCP(battle.KMs)); // CP
	outbuf.append<uint32_t>(0); // ???
	outbuf.append<int16_t>((battle.status == Client::BATTLESTATUS::BS_WON) ? currentRival->WinReward() : currentRival->LoseReward()); // Item
	outbuf.append<uint8_t>(0); // For Survival?
	Send();
	clearBattle();
}
void Client::Send(PACKET* src)
{
	if (src == nullptr)
		src = &outbuf;
	if (ClientSocket >= 0)
	{
		if (CLIENT_BUFFER_SIZE < ((int)src->getSize() + 15))
		{
			logger->Log(Logger::LOGTYPE_Client, L"Client Sent too large packet.");
			Disconnect();
		}
		else
		{
			uint16_t size = src->getSize();
			if (size == 0) return;
			addToSendQueue(src);
#ifdef PACKET_OUTPUT
			//if (src->getType() != 0x0A00 && src->getType() != 0x0700)
			if (src->getType() != 0x0700)
			{
				logger->Log(Logger::LOGTYPE_PACKET, L"Packet: Server -> Client (%u)", courseID);
				logger->Log(Logger::LOGTYPE_PACKET, logger->packet_to_text(&src->buffer[0x00], size).c_str());
			}
#endif
			//src->setSize(size);
		}
	}
}
void Client::SendToCourse(PACKET* src, bool exclude)
{
	if (src == nullptr)
		src = &coursebuf;
	if (course != nullptr && course->getCourse() == currentCourse)
	{
		course->sendToCourse(src, exclude ? driverslicense : -1);
	}
}
void Client::SendToProximity(float x, float y, PACKET* src, bool exclude)
{
	if (src == nullptr)
		src = &coursebuf;
	if (course != nullptr && course->getCourse() == currentCourse)
	{
		course->sendToProximity(src, x, y, exclude ? driverslicense : -1);
	}
}
void Client::SendToOpponent(PACKET* src)
{
	if (src == nullptr)
		src = &outbuf;
	if (battle.challenger != nullptr)
	{
		battle.challenger->Send(src);
	}
}
void Client::packetEnable(uint32_t packetType)
{
	if (packetType > sizeof(packetAllow)) return;
	else packetAllow[packetType] = true;
}
void Client::packetDisable(uint32_t packetType)
{
	if (packetType > sizeof(packetAllow)) return;
	else packetAllow[packetType] = false;
}
void Client::enableStandardPackets()
{
	packetEnable(0x00);
	packetDisable(0x01);
	packetEnable(0x02);
	packetEnable(0x03);
	packetEnable(0x04);
	packetEnable(0x05);
	packetEnable(0x06);
	packetEnable(0x07);
	packetDisable(0x09);
	packetEnable(0x0A);
	packetEnable(0x0C);
	packetEnable(0x10);
	packetEnable(0x11);
	packetEnable(0x12);
	packetEnable(0x15);
	packetEnable(0x16);
	packetDisable(0x17);
}
void Client::enableShopPackets()
{
	packetDisable(0x03);
	packetDisable(0x04);
	packetDisable(0x05);
	packetDisable(0x07);
	packetEnable(0x09);
	packetDisable(0x0C);
	packetEnable(0x11);
	packetEnable(0x12);
	packetDisable(0x15);
	packetDisable(0x16);
	packetEnable(0x17);
}
void Client::enableCoursePackets()
{
	packetEnable(0x03);
	packetEnable(0x04);
	packetEnable(0x05);
	packetEnable(0x07);
	packetDisable(0x09);
	packetEnable(0x0C);
	packetEnable(0x11);
	packetEnable(0x12);
	packetEnable(0x15);
	packetEnable(0x16);
	packetDisable(0x17);
}
void Client::enableAllPackets()
{
	packetEnable(0x00);
	packetEnable(0x01);
	packetEnable(0x02);
	packetEnable(0x03);
	packetEnable(0x04);
	packetEnable(0x05);
	packetEnable(0x06);
	packetEnable(0x07);
	packetEnable(0x08);
	packetEnable(0x09);
	packetEnable(0x0A);
	packetEnable(0x0C);
	packetEnable(0x10);
	packetEnable(0x11);
	packetEnable(0x12);
	packetEnable(0x15);
	packetEnable(0x16);
	packetEnable(0x17);
}
bool Client::CanSendPackets(uint32_t packetType)
{
	if (packetType > sizeof(packetAllow)) return false;
	return packetAllow[packetType] ? true : false;
}
void Client::Disconnect()
{
	todc = true;
}
void Client::ProcessPacket()
{
	if (!todc)
	{
		uint32_t packetType = (inbuf.getType() >> 8) & 0xFF;
		if (packetType > sizeof(MainPacketFunctions) / 4)
		{
			logger->Log(Logger::LOGTYPE_COMM, L"Invalid Packet Message: %04X from client %s (%u / %s) with ID %u",
				inbuf.getType(),
				logger->toWide(handle).c_str(),
				driverslicense,
				logger->toWide((char*)&IP_Address).c_str(),
				courseID
			);
			SendPing();
			//Disconnect();
			return;
		}
#ifdef PACKET_OUTPUT
		//if(packetType == 0x07)
		//if (packetType != 0x0A && packetType != 0x07 && packetType != 0x00 && packetType != 0x04)
		//if (packetType != 0x0A && packetType != 0x07 && packetType != 0x00)
		if (packetType != 0x07 && packetType != 0x00)
		//if (packetType != 0x0A && packetType != 0x00)
		//if (packetType == 0x07 || packetType == 0x04)
		//if(inbuf.getType() != 0x700)
		{
			logger->Log(Logger::LOGTYPE_PACKET, L"Packet: Client (%u) -> Server", courseID);
			logger->Log(Logger::LOGTYPE_PACKET, logger->packet_to_text(&inbuf.buffer[0x00], inbuf.getSize()).c_str());
		}
#endif
		// Deny all except authentication packets from clients not authenticated
		if (CanSendPackets(packetType) == false) return;

		// Clear packet resend
		packetResend = 0;

		MainPacketFunctions[packetType](this);
		timeoutCount = time(NULL) + CLIENT_TIMEOUT;
#ifdef PACKET_OUTPUT
		//if (packetType != 0x07) return;
		//if (packetType == 0x07 || packetType == 0x00 || packetType == 0x0A || packetType == 0x04 || packetType == 0x05) return;
		if (packetType == 0x07 || packetType == 0x00 || packetType == 0x04 || packetType == 0x05) return;
		//if (packetType == 0x07 || packetType == 0x00 || packetType == 0x04) return;
		//if (packetType != 0x07 || packetType != 0x04) return;
		else if ((inbuf.getType() & 0xFF00) == (outbuf.getType() & 0xFF00)) logger->Log(Logger::LOGTYPE_COMM, L"Packet Message: %04X -> %04X", inbuf.getType(), outbuf.getSubType());
		else logger->Log(Logger::LOGTYPE_COMM, L"Packet Message: %04X -> NO RESPONSE", inbuf.getType());
#endif
	}
}
void Client::ProcessManagementPacket()
{
	if (!todc)
	{
		uint32_t packetType = serverbuf.getType();
		if (packetType > sizeof(ManagementPacketFunctions) / 4)
		{
			logger->Log(Logger::LOGTYPE_COMM, L"Invalid Packet Message: %04X from management server", serverbuf.getType());
			Disconnect();
			return;
		}
#ifdef PACKET_OUTPUT
		logger->Log(Logger::LOGTYPE_PACKET, L"Packet: Management -> Client");
		logger->Log(Logger::LOGTYPE_PACKET, logger->packet_to_text(&serverbuf.buffer[0x00], serverbuf.getSize() + 2).c_str());
#endif
		ManagementPacketFunctions[packetType](this);
	}
}
void Client::addToSendQueue(PACKET* src)
{
	*(uint16_t*)&src->buffer[0x00] = SWAP_SHORT(src->getSize() - 2);
	SEND_QUEUE entry = { 0 };
	CopyMemory(&entry.sndbuf[0], &src->buffer[0x00], src->getSize());
	{
		std::lock_guard<std::mutex> locker(_muClient);
		sendQueue.push(entry);
	}
}
void Client::addToSendQueue(SEND_QUEUE* src)
{
	if (src != nullptr)
	{
		std::lock_guard<std::mutex> locker(_muClient);
		sendQueue.push(*src);
	}
}
SEND_QUEUE Client::getFromSendQueue()
{
	SEND_QUEUE current = { 0 };
	std::lock_guard<std::mutex> locker(_muClient);
	if (sendQueue.size())
	{
		current = sendQueue.front();
		sendQueue.pop();
		return current;
	}
	else
	{
		std::queue<SEND_QUEUE> q;
		sendQueue.swap(q);
	}
	return current;
}
uint32_t Client::messagesInSendQueue()
{
	std::lock_guard<std::mutex> locker(_muClient);
	if (sendQueue.size() > 10)
	{
		logger->Log(Logger::LOGTYPE_COMM, L"Send Queue exceeds 10 for client %s(%u / %s). %u in queue",
			logger->toWide(handle).c_str(),
			driverslicense,
			logger->toWide((char*)&IP_Address).c_str(),
			sendQueue.size()
		);
	}
	else if (sendQueue.size() > 50)
	{
		logger->Log(Logger::LOGTYPE_COMM, L"Send Queue exceeds 50 for client %s(%u / %s) so was disconnected",
			logger->toWide(handle).c_str(),
			driverslicense,
			logger->toWide((char*)&IP_Address).c_str()
		);
		Disconnect();
		return 0;
	}
	return sendQueue.size();
}

void Client::clearSendQueue()
{
	std::lock_guard<std::mutex> locker(_muClient);
	std::queue<SEND_QUEUE> q;
	sendQueue.swap(q);
}