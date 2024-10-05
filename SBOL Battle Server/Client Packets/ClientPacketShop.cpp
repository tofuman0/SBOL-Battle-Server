#include <Windows.h>
#include "..\server.h"

void ClientPacketShop(Client* client)
{
	uint16_t pType = client->inbuf.getType();
	// Prevent shop packets to be processed when not in shops
	if (pType > 0x900 && client->inCourse) return;

	switch (pType)
	{
	case 0x900: // Entered shop : Car Bay / Slot at 0x04? 0x04 Parts
	{
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;

		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u has specified invalid bay number in part shop.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x900);
		client->outbuf.setSubType(0x980);
		client->outbuf.append<uint32_t>(bay + 1); // Active Car
		for (int32_t i = 0; i < sizeof(client->garagedata.car[bay].parts); i++) ((((uint8_t*)&client->garagedata.car[bay].parts)[i] & PART_OWNED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0));
	}
	break;
	case 0x901: // Purchased item from shop : Car bay/slot at 0x04 Category at 0x08 (byte), item type 0x09 (byte), item id 0x0A (byte), cost at 0x0B (int)
	{
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		uint8_t itemCategory = client->inbuf.get<uint8_t>(0x08);
		uint8_t itemType = client->inbuf.get<uint8_t>(0x09);
		uint8_t itemID = client->inbuf.get<uint8_t>(0x0A);
		uint32_t cost = client->inbuf.get<uint32_t>(0x0B);
		uint32_t finalPrice = client->getShopPartPrice(bay, itemCategory, itemType, itemID);

		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u has specified invalid bay number in part shop.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		if (finalPrice == 0 || cost != finalPrice)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has provided different price for part than expected. Category %u, ID %u, Type %u, Cost %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemCategory,
				itemID,
				itemType,
				cost,
				finalPrice
			);
			client->Disconnect();
			return;
		}

		if (client->enoughCP(finalPrice))
		{
			if (client->purchasePart(bay, itemCategory, itemType, itemID) == false)
			{
				client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to purchase part from the part shop invalid data. Category %u, ID %u, Type %u, Client CP %u, Real Cost %u.",
					client->logger->toWide(client->handle).c_str(),
					client->driverslicense,
					client->logger->toWide((char*)&client->IP_Address).c_str(),
					itemCategory,
					itemID,
					itemType,
					client->getCP(),
					finalPrice
				);
				client->Disconnect();
				return;
			}
			client->takeCP(finalPrice);
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to purchase part from the part shop without enough CP. Category %u, ID %u, Type %u, Client CP %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemCategory,
				itemID,
				itemType,
				client->getCP(),
				finalPrice
			);
			client->Disconnect();
			return;
		}

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x900);
		client->outbuf.setSubType(0x981);
		client->outbuf.append<uint32_t>(bay + 1);
		client->outbuf.append<int64_t>(client->getCP());
	}
	break;
	case 0x902: // equipped item from shop : Car bay / slot at 0x04 Category at 0x08 (byte), item type 0x09 (byte), item id 0x0A (byte)
	{
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		uint8_t itemCategory = client->inbuf.get<uint8_t>(0x08);
		uint8_t itemType = client->inbuf.get<uint8_t>(0x09);
		uint8_t itemID = client->inbuf.get<uint8_t>(0x0A);

		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u has specified invalid bay number in part shop.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		if (client->equipPart(bay, itemCategory, itemType, itemID) == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u failed to equip part. Category %u, Type %u, ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemCategory,
				itemType,
				itemID
			);
			client->Disconnect();
			return;
		}

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x900);
		client->outbuf.setSubType(0x982);
		client->outbuf.append<uint32_t>(bay + 1);
		client->outbuf.append<int64_t>(client->getCP());
	}
	break;
	case 0x903: // Purchasing wheels
	{
		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;
		uint16_t wheelID = client->inbuf.get<uint16_t>(0x08);
		uint8_t wheelType = client->inbuf.get<uint8_t>(0x0A);
		uint32_t cost = client->inbuf.get<uint32_t>(0x23);

		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has specified invalid bay number in paint shop.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		S_DAT_ENTRY* sDAT = &client->server->sDAT.sdat[client->garagedata.car[bay].carID % client->server->sDAT.count];
		int32_t limit = client->server->shopData.count;
		PARTSHOPDATA_ENTRY* shopData = client->server->shopData.data;
		float price1 = shopData[sDAT->pLookup[0x00] % limit].wheelPrice;
		float price2 = shopData[sDAT->pLookup[0x01] % limit].wheelPrice;
		uint32_t finalPrice = (uint32_t)((price1 * 10.0f) + 0.5f) + (uint32_t)((price2 * 10.0f) + 0.5f);
		if (wheelID == client->garagedata.car[bay].carID) finalPrice = 0;
		else if (wheelID < 200 || wheelID >= 300)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to to purchase invalid wheel from shop. ID %u, Type %u, Client Cost %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				wheelID,
				wheelType,
				cost,
				finalPrice
			);
			client->Disconnect();
			return;
		}

		if (cost != finalPrice)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has altered shop price from wheel shop. ID %u, Type %u, Client Cost %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				wheelID,
				wheelType,
				cost,
				finalPrice
			);
			client->Disconnect();
			return;
		}

		if (client->enoughCP(finalPrice) && client->garagedata.car[bay].carID != 0xFFFFFFFF)
		{
#ifdef _DEBUG
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client bought from wheel shop. ID %u, Type %u, Cost %u. Guess: %u", wheelID, wheelType, cost, finalPrice);
#endif
			client->takeCP(finalPrice);
			client->garagedata.car[bay].carMods.wheels = wheelID;
			client->garagedata.car[bay].carMods.colourwheels = wheelType;
			client->garagedata.car[bay].carMods.wheelcolour.R1 = client->inbuf.get<float>(0x0B);
			client->garagedata.car[bay].carMods.wheelcolour.G1 = client->inbuf.get<float>(0x0F);
			client->garagedata.car[bay].carMods.wheelcolour.B1 = client->inbuf.get<float>(0x13);
			client->garagedata.car[bay].carMods.wheelcolour.R2 = client->inbuf.get<float>(0x17);
			client->garagedata.car[bay].carMods.wheelcolour.G2 = client->inbuf.get<float>(0x1B);
			client->garagedata.car[bay].carMods.wheelcolour.B2 = client->inbuf.get<float>(0x1F);
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x900);
			client->outbuf.setSubType(0x981); // Success???
			client->outbuf.append<uint32_t>(bay + 1);
			client->outbuf.append<int64_t>(client->getCP());
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to purchase wheels from the wheel shop without enough CP. ID %u, Type %u, Client CP %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				wheelID,
				wheelType,
				client->getCP(),
				finalPrice
			);
			client->Disconnect();
			return;
		}


	}
	break;
	case 0x904: // Request shop contents
	{
		int32_t count;

		uint32_t bay = client->inbuf.get<uint32_t>(0x04) - 1;

		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u has specified invalid bay number in part shop.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
		}

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x900);
		client->outbuf.setSubType(0x983);
		client->outbuf.append<uint32_t>(bay + 1);
		client->outbuf.append<uint8_t>(17); // repeat as many as this

		// Engine
		client->outbuf.append<uint8_t>(0); // Category
		client->outbuf.append<uint8_t>(0); // Item Type
		count = sizeof(PARTS::engine) - 1;
		for (int32_t i = 1; i <= count; i++)	(client->garagedata.car[bay].parts.engine[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0);  // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Muffler
		client->outbuf.append<uint8_t>(0); // Category
		client->outbuf.append<uint8_t>(1); // Item Type
		count = sizeof(PARTS::muffler) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.muffler[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Transmission
		client->outbuf.append<uint8_t>(0); // Category
		client->outbuf.append<uint8_t>(2); // Item Type
		count = sizeof(PARTS::transmission) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.transmission[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Differential
		client->outbuf.append<uint8_t>(0); // Category
		client->outbuf.append<uint8_t>(3); // Item Type
		count = sizeof(PARTS::differential) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.differential[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Tyre/Brakes
		client->outbuf.append<uint8_t>(0); // Category
		client->outbuf.append<uint8_t>(4); // Item Type
		count = sizeof(PARTS::tyreBrakes) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.tyreBrakes[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Category 0 Type 5 unused
		client->outbuf.append<uint8_t>(0); // Category
		client->outbuf.append<uint8_t>(5); // Item Type
		for (int32_t i = 0; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Suspension
		client->outbuf.append<uint8_t>(0); // Category
		client->outbuf.append<uint8_t>(6); // Item Type
		count = sizeof(PARTS::suspension) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.suspension[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Body
		client->outbuf.append<uint8_t>(0); // Category
		client->outbuf.append<uint8_t>(7); // Item Type
		count = sizeof(PARTS::body) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.body[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Overfender
		client->outbuf.append<uint8_t>(1); // Category
		client->outbuf.append<uint8_t>(0); // Item Type
		count = sizeof(PARTS::overFenders) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.overFenders[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Front Bumper
		client->outbuf.append<uint8_t>(1); // Category
		client->outbuf.append<uint8_t>(1); // Item Type
		count = sizeof(PARTS::frontBumper) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.frontBumper[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Bonnet
		client->outbuf.append<uint8_t>(1); // Category
		client->outbuf.append<uint8_t>(2); // Item Type
		count = sizeof(PARTS::bonnet) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.bonnet[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Mirrors
		client->outbuf.append<uint8_t>(1); // Category
		client->outbuf.append<uint8_t>(3); // Item Type
		count = sizeof(PARTS::mirrors) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.mirrors[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Sideskirts
		client->outbuf.append<uint8_t>(1); // Category
		client->outbuf.append<uint8_t>(4); // Item Type
		count = sizeof(PARTS::sideSkirts) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.sideSkirts[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Rear Bumper
		client->outbuf.append<uint8_t>(1); // Category
		client->outbuf.append<uint8_t>(5); // Item Type
		count = sizeof(PARTS::rearBumper) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.rearBumper[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Rear Spoiler
		client->outbuf.append<uint8_t>(1); // Category
		client->outbuf.append<uint8_t>(6); // Item Type
		count = sizeof(PARTS::rearSpoiler) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.rearSpoiler[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Grill
		client->outbuf.append<uint8_t>(2); // Category
		client->outbuf.append<uint8_t>(0); // Item Type
		count = sizeof(PARTS::grill) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.grill[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags

		// Lights
		client->outbuf.append<uint8_t>(2); // Category
		client->outbuf.append<uint8_t>(1); // Item Type
		count = sizeof(PARTS::lights) - 1;
		for (int32_t i = 1; i <= count; i++) (client->garagedata.car[bay].parts.lights[i] & PART_UNLOCKED) ? client->outbuf.append<uint8_t>(1) : client->outbuf.append<uint8_t>(0); // Set if part is unlocked
		for (int32_t i = count; i < 10; i++) client->outbuf.append<uint8_t>(0); // set unused flags
	}
	break;
	default:
		return;
	}
	client->Send();
}
