#define _WINSOCKAPI_
#include <windows.h>
#include "serverpacket.h"

SERVERPACKET::SERVERPACKET()
{
	init();
}


SERVERPACKET::~SERVERPACKET()
{
}

void SERVERPACKET::init()
{
	pOffset = 0;
	pSize = 0;
	ZeroMemory(buffer, sizeof(buffer));
}

void SERVERPACKET::appendArray(uint8_t* in, uint32_t size)
{
	if ((pOffset + size) < CLIENT_BUFFER_SIZE)
	{
		CopyMemory(&buffer[pOffset], in, size);
		pOffset += size;
		setSize(getSize() + size);
	}
}

void SERVERPACKET::setArray(uint8_t* in, uint32_t size, uint32_t offset)
{
	if ((offset + size) < CLIENT_BUFFER_SIZE)
	{
		CopyMemory(&buffer[offset], in, size);
	}
}

void SERVERPACKET::getArray(uint8_t * in, uint32_t size, uint32_t offset)
{
	if ((offset + size) < CLIENT_BUFFER_SIZE)
	{
		CopyMemory(&in[0], &buffer[offset], size);
	}
}

void SERVERPACKET::getArray(uint8_t * in, uint32_t size)
{
	if ((pOffset + size) < CLIENT_BUFFER_SIZE)
	{
		CopyMemory(&in[0], &buffer[pOffset], size);
		pOffset += size;
	}
}

int32_t SERVERPACKET::appendString(std::string& cmd)
{
	int32_t size = cmd.length();
	if (pOffset + size < CLIENT_BUFFER_SIZE)
	{
		strcpy_s((char *)&buffer[pOffset], size + 1, &cmd[0]);
		pOffset += size;
		setSize(getSize() + size);
		return size + 1;
	}
	return 0;
}

int32_t SERVERPACKET::appendString(std::string& cmd, uint32_t addToSize)
{
	int32_t size = cmd.length();
	if (pOffset + size < CLIENT_BUFFER_SIZE)
	{
		strcpy_s((char *)&buffer[pOffset], size + 1, &cmd[0]);
		pOffset += addToSize;
		setSize(getSize() + addToSize);
		return addToSize;
	}
	return 0;
}

int32_t SERVERPACKET::setString(std::string& cmd, uint32_t offset)
{
	int32_t size = cmd.length();
	if (offset + size < CLIENT_BUFFER_SIZE)
	{
		strcpy_s((char *)&buffer[offset], size + 1, &cmd[0]);
		return size + 1;
	}
	return 0;
}

std::string SERVERPACKET::getString(uint32_t offset)
{
	std::string tempString;
	tempString.assign((char*)&buffer[offset]);
	return tempString;
}

std::string SERVERPACKET::getString(uint32_t offset, uint32_t size)
{
	std::string tempString;
	if (offset + size > CLIENT_BUFFER_SIZE)
		return "";
	tempString.assign((char*)&buffer[offset], size);
	if (tempString.find("\0") != std::string::npos)
		tempString.assign((char*)&buffer[offset]);
	return tempString;
}

std::string SERVERPACKET::getStringA(uint32_t size)
{
	std::string tempString;
	if (pOffset + size > CLIENT_BUFFER_SIZE)
		return "";
	tempString.assign((char*)&buffer[pOffset], size);
	if (tempString.find("\0") != std::string::npos)
		tempString.assign((char*)&buffer[pOffset]);
	pOffset += size;
	return tempString;
}

void SERVERPACKET::clearBuffer()
{
	pOffset = 0;
	pSize = 0;
	ZeroMemory(&buffer[0], sizeof(buffer));
}

uint16_t SERVERPACKET::getType()
{
	return *(uint16_t*)&buffer[0x02];
}

void SERVERPACKET::setType(uint16_t in)
{
	*(uint16_t*)&buffer[0x02] = in;
}

uint16_t SERVERPACKET::getSubType()
{
	return *(uint16_t*)&buffer[0x04];
}

void SERVERPACKET::setSubType(uint16_t in)
{
	*(uint16_t*)&buffer[0x04] = in;
}
