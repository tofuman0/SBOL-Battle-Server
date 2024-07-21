#define _WINSOCKAPI_
#include <windows.h>
#include "packet.h"

PACKET::PACKET()
{
	init();
}


PACKET::~PACKET()
{
}

void PACKET::init()
{
	pOffset = 0;
	pSize = 0;
	ZeroMemory(buffer, sizeof(buffer));
}

void PACKET::appendArray(uint8_t* in, uint32_t size)
{
	if ((pOffset + size) < Client_BUFFER_SIZE)
	{
		CopyMemory(&buffer[pOffset], in, size);
		pOffset += size;
		setSize(getSize() + size);
	}
}

void PACKET::setArray(uint8_t* in, uint32_t size, uint32_t offset)
{
	if ((offset + size) < Client_BUFFER_SIZE)
	{
		CopyMemory(&buffer[offset], in, size);
	}
}

void PACKET::getArray(uint8_t * in, uint32_t size, uint32_t offset)
{
	if ((offset + size) < Client_BUFFER_SIZE)
	{
		CopyMemory(&in[0], &buffer[offset], size);
	}
}

void PACKET::getArray(uint8_t * in, uint32_t size)
{
	if ((pOffset + size) < Client_BUFFER_SIZE)
	{
		CopyMemory(&in[0], &buffer[pOffset], size);
		pOffset += size;
	}
}

int32_t PACKET::appendString(std::string& cmd)
{
	int32_t size = cmd.length();
	if (pOffset + size < Client_BUFFER_SIZE)
	{
		strcpy_s((char *)&buffer[pOffset], size + 1, &cmd[0]);
		pOffset += size;
		setSize(getSize() + size);
		return size + 1;
	}
	return 0;
}

int32_t PACKET::appendString(std::string& cmd, uint32_t addToSize)
{
	int32_t size = cmd.length();
	if (pOffset + size < Client_BUFFER_SIZE)
	{
		strcpy_s((char *)&buffer[pOffset], size + 1, &cmd[0]);
		pOffset += addToSize;
		setSize(getSize() + addToSize);
		return addToSize;
	}
	return 0;
}

int32_t PACKET::setString(std::string& cmd, uint32_t offset)
{
	int32_t size = cmd.length();
	if (offset + size < Client_BUFFER_SIZE)
	{
		strcpy_s((char *)&buffer[offset], size + 1, &cmd[0]);
		return size + 1;
	}
	return 0;
}

std::string PACKET::getString(uint32_t offset)
{
	std::string tempString;
	tempString.assign((char*)&buffer[offset]);
	return tempString;
}

std::string PACKET::getString(uint32_t offset, uint32_t size)
{
	std::string tempString;
	if (offset + size > Client_BUFFER_SIZE)
		return "";
	tempString.assign((char*)&buffer[offset], size);
	if (tempString.find("\0") != std::string::npos)
		tempString.assign((char*)&buffer[offset]);
	return tempString;
}

std::string PACKET::getStringA(uint32_t size)
{
	std::string tempString;
	if (pOffset + size > Client_BUFFER_SIZE)
		return "";
	tempString.assign((char*)&buffer[pOffset], size);
	if (tempString.find("\0") != std::string::npos)
		tempString.assign((char*)&buffer[pOffset]);
	pOffset += size;
	return tempString;
}

void PACKET::clearBuffer()
{
	pOffset = 0;
	pSize = 0;
	ZeroMemory(&buffer[0], sizeof(buffer));
}

uint16_t PACKET::getType()
{
	return swapShort(*(uint16_t*)&buffer[0x02]);
}

void PACKET::setType(uint16_t in)
{
	*(uint16_t*)&buffer[0x02] = swapShort(in);
}

uint16_t PACKET::getSubType()
{
	return swapShort(*(uint16_t*)&buffer[0x04]);
}

void PACKET::setSubType(uint16_t in)
{
	*(uint16_t*)&buffer[0x04] = swapShort(in);
}

uint32_t PACKET::getVer()
{
	return swapLong(*(uint32_t*)&buffer[0x04]);
}

void PACKET::setVer(uint32_t in)
{
	*(uint32_t*)&buffer[0x04] = swapLong(in);
}

uint16_t PACKET::swapShort(uint16_t in)
{
	return SWAP_SHORT(in);
}

int16_t PACKET::swapShort(int16_t in)
{
	return SWAP_SHORT(in);
}

uint32_t PACKET::swapLong(uint32_t in)
{
	return SWAP_LONG(in);
}

int32_t PACKET::swapLong(int32_t in)
{
	return SWAP_LONG(in);
}

uint64_t PACKET::swapLongLong(uint64_t in)
{
	return SWAP_LONGLONG(in);
}

int64_t PACKET::swapLongLong(int64_t in)
{
	return SWAP_LONGLONG(in);
}

float PACKET::swapFloat(float in)
{
	uint32_t tmp = *(uint32_t*)&in;
	tmp = SWAP_LONG(tmp);
	return *(float*)&tmp;
}