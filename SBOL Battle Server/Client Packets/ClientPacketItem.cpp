#include <Windows.h>
#include "..\server.h"

void ClientPacketItem(Client* client)
{
	uint16_t pType = client->inbuf.getType();

	switch (pType)
	{
	case 0x1100:
	{	// Get item list
		client->SendItems();
		return;
	}
	break;
	case 0x1101:
	{	// Item use - Item ID at 0x04
		uint16_t itemID = client->inbuf.get<uint16_t>(0x04);

		if (client->isValidItem(itemID) == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to use invalid item. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}

		if (client->removeItem(itemID) == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to use item they don't have. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}

		ITEMDATA_ENTRY item = client->getItem(itemID);
		uint32_t cp = 0;

		if (item.itemType != Client::ITEMTYPES::ITEMTYPE_ITEM)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to use item that isn't an ITEM type. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}

		if (item.type == Client::ITEMUSETYPES::USETYPE_CP)
		{
			cp = item.resaleValue;
			client->giveCP(cp);
		}
		// TODO process other item use types which provide boosts

		switch (itemID)
		{
		case 0x845: // Enter Beginner Course
			client->notBeginner = false;
			client->server->saveClientData(client);
			client->enableAllPackets();
			break;
		case 0x8DD: // Hand Signs
			client->enableSign(0);
			client->enableSign(1);
			client->enableSign(2);
			break;
		case 0x8DE: // Number Signs
			client->enableSign(3);
			client->enableSign(4);
			client->enableSign(5);
			client->enableSign(6);
			client->enableSign(7);
			client->enableSign(8);
			client->enableSign(9);
			client->enableSign(10);
			client->enableSign(11);
			client->enableSign(12);
			break;
		case 0x8DF: // Custom Signs
			client->enableSign(13);
			client->enableSign(14);
			client->enableSign(15);
			break;
		default:
			break;
		}

		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1100);
		client->outbuf.setSubType(0x1181);
		client->outbuf.append<uint8_t>(0);
		client->outbuf.append<uint16_t>(itemID);
		client->outbuf.append<uint32_t>(cp); // CP to add
	}
	break;
	case 0x1103:
	{	// Sell Item Ticket
		// Item ID @ 0x04
		// Value @ 0x06
		uint16_t itemID = client->inbuf.get<uint16_t>(0x04);
		uint32_t itemValue = client->inbuf.get<uint32_t>(0x06);

		if (client->isValidItem(itemID) == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to sell an invalid item. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}

		ITEMDATA_ENTRY item = client->getItem(itemID);
		int16_t finalPrice = client->getItemResalePrice(itemID);

		if (finalPrice == -1 && item.itemType == Client::ITEMTYPES::ITEMTYPE_PART) finalPrice = client->getShopPartPriceFromID(item.carID, item.category, item.type, item.typeValue);

		if (itemValue != finalPrice || finalPrice == -1)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried purchase an item with an modified cost. ID %u, Client Cost %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID,
				itemValue,
				finalPrice
			);
			client->Disconnect();
			return;
		}

		if (client->hasItem(itemID))
		{
			client->removeItem(itemID);
			client->giveCP(finalPrice);
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x1100);
			client->outbuf.setSubType(0x1183);
			client->outbuf.append<uint16_t>(itemID);
			client->outbuf.append<uint32_t>(finalPrice);
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to sell an item they don't have. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}
	}
	break;
	case 0x1104:
	{	// Item Purchase: Item ID at 0x04, cost at 0x06
		uint16_t itemID = client->inbuf.get<uint16_t>(0x04);
		uint32_t itemCost = client->inbuf.get<uint32_t>(0x06);

		if (client->isValidItem(itemID) == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to purchase an invalid item. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}
		int16_t finalPrice = client->getItemPrice(itemID);

		if (itemCost != finalPrice || finalPrice == -1)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried purchase an item with an modified cost. ID %u, Client Cost %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID,
				itemCost,
				finalPrice
			);
			client->Disconnect();
			return;
		}

		if (client->enoughCP(finalPrice) && client->itembox.size() < ITEMBOX_LIMIT)
		{
			client->takeCP(itemCost);
			client->addItem(itemID);
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x1100);
			client->outbuf.setSubType(0x1184);
			client->outbuf.append<uint8_t>(1);
			client->outbuf.append<uint32_t>(itemCost);
			client->outbuf.append<uint8_t>(static_cast<uint8_t>(min(client->itembox.size(), ITEMBOX_LIMIT)));
			for (uint32_t i = 0; i < min(client->itembox.size(), ITEMBOX_LIMIT); i++) client->outbuf.append<uint16_t>(client->itembox.at(i));
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried purchase an item without enough CP. ID %u, Client CP %u, Real Cost %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID,
				client->getCP(),
				finalPrice
			);
			client->Disconnect();
			return;
		}
	}
	break;
	case 0x1106:
	{	// Battle reward item list
		// TODO: To use reward to construct packet
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1100);
		client->outbuf.setSubType(0x1186);
		client->outbuf.append<uint8_t>(1);
		client->outbuf.append<uint16_t>(1);
	}
	break;
	case 0x1107:
	{	// Trash Item: ID at 0x04
		uint16_t itemID = client->inbuf.get<uint16_t>(0x04);
		if (client->removeItem(itemID) == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to trash an item they don't have. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}
		client->outbuf.clearBuffer();
		client->outbuf.setSize(0x06);
		client->outbuf.setOffset(0x06);
		client->outbuf.setType(0x1100);
		client->outbuf.setSubType(0x1187);
		client->outbuf.append<uint8_t>(1);
		client->outbuf.append<uint8_t>(static_cast<uint8_t>(min(client->itembox.size(), ITEMBOX_LIMIT)));
		for (uint32_t i = 0; i < min(client->itembox.size(), ITEMBOX_LIMIT); i++) client->outbuf.append<uint16_t>(client->itembox.at(i));
	}
	break;
	case 0x1108:
	{	// Used Car Item:
		// Item ID at 0x04
		// Car Colour R1 at 0x06
		// Car Colour G1 at 0x0A
		// Car Colour B1 at 0x0E
		// Car Colour R2 at 0x12
		// Car Colour G2 at 0x16
		// Car Colour B2 at 0x1A
		uint16_t itemID = client->inbuf.get<uint16_t>(0x04);
		float r1 = client->inbuf.get<float>(0x06);
		float g1 = client->inbuf.get<float>(0x0A);
		float b1 = client->inbuf.get<float>(0x0E);
		float r2 = client->inbuf.get<float>(0x12);
		float g2 = client->inbuf.get<float>(0x16);
		float b2 = client->inbuf.get<float>(0x1A);

		ITEMDATA_ENTRY item = client->getItem(itemID);

		if (client->isValidItem(itemID) == false || item.itemType != Client::ITEMTYPES::ITEMTYPE_CAR)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to trade in an invalid item to the car ticket shop. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}

		if (client->hasItem(itemID))
		{
			uint8_t res = 1;
			if (client->isValidCar(item.carID) == true)
			{
				int32_t emptyBay = client->getEmptyBay();
				if (emptyBay != -1)
				{
					if (client->addCar(item.carID, emptyBay, r1, g1, b1, r2, g2, b2) == true)
					{
						client->removeItem(itemID);
						res = 0;
					}
				}
			}
			else client->removeItem(itemID);
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x1100);
			client->outbuf.setSubType(0x1188);
			client->outbuf.append<uint8_t>(res);
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to trade in an item to the car ticket shop that they don't have. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}
	}
	break;
	case 0x1109:
	{	// Used Part Item:
		// ID at 0x04
		// Bay at 0x06
		uint16_t itemID = client->inbuf.get<uint16_t>(0x04);
		uint32_t bay = client->inbuf.get<uint32_t>(0x06) - 1;
		uint16_t thisItem = 0;
		bool result = false;

		if (bay >= (uint32_t)(client->garagedata.garageCount * 4) || client->garagedata.car[bay].carID == 0xFFFFFFFF)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) with ID %u has specified invalid bay number in parts ticket.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str()
			);
			client->Disconnect();
			return;
		}

		if (client->isValidItem(itemID) == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to purchase an invalid item. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}

		if (client->removeItem(itemID) == false)
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to exchange an item they don't have. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}

		ITEMDATA_ENTRY item = client->getItem(itemID);

		switch (item.itemType)
		{
		case Client::ITEMTYPES::ITEMTYPE_PART:
		{
			if (item.carManufacturer == 0xFF)
			{
				if (client->confirmClass(bay, (Client::CARCLASS)item.carClass) == true) result = client->purchasePart(bay, item.category, item.type, item.typeValue, true);
				else result = false;
			}
			else if (item.carManufacturer != 0xFF && item.carID == 0xFFFF)
			{
				if (client->confirmClass(bay, (Client::CARCLASS)item.carClass) == true && item.carManufacturer == client->getCarManufacturer(client->garagedata.car[bay].carID)) result = client->purchasePart(bay, item.category, item.type, item.typeValue, true);
				else result = false;
			}
			else
			{
				if (client->confirmClass(bay, (Client::CARCLASS)item.carClass) == true && item.carID == client->getCarID(bay)) result = client->purchasePart(bay, item.category, item.type, item.typeValue, true);
				else result = false;
			}
		}
		break;
		default:
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to exchange an item that isn't exchangable. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}

		if (result == true)
		{
			client->outbuf.clearBuffer();
			client->outbuf.setSize(0x06);
			client->outbuf.setOffset(0x06);
			client->outbuf.setType(0x1100);
			client->outbuf.setSubType(0x1189);
			client->outbuf.append<uint8_t>(0);
			client->outbuf.append<uint32_t>(bay + 1);
		}
		else
		{
			client->logger->Log(Logger::LOGTYPE_CLIENT, L"Client %s (%u / %s) has tried to exchange an item but didn't meet the requirements. ID %u.",
				client->logger->toWide(client->handle).c_str(),
				client->driverslicense,
				client->logger->toWide((char*)&client->IP_Address).c_str(),
				itemID
			);
			client->Disconnect();
			return;
		}
	}
	break;
	default:
		return;
	}
	client->Send();
}
