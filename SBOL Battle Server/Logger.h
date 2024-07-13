#pragma once
#include "globals.h"
#include <iostream>
#include <fstream>
#include <string>

#define PACKET_BUFFER_SIZE 64000 * 16

class LOGGER
{
public:
	LOGGER();
	LOGGER(char* path);
	~LOGGER();
	const enum LOGTYPE {
		LOGTYPE_NONE,
		LOGTYPE_SERVER,
		LOGTYPE_PACKET,
		LOGTYPE_ERROR,
		LOGTYPE_DEBUG,
		LOGTYPE_COMM,
		LOGTYPE_CLIENT,
		LOGTYPE_MANAGEMENT
	};
	void Log(LOGTYPE type, const wchar_t* in, ...);
	void setLogPath(char* in);
	void setLogPath(std::string& in);
	bool isLogPathSet();
	std::string getLogPath();
	std::wstring packet_to_text(uint8_t* buf, uint32_t len);
	std::wstring toWide(std::string in);
	std::string toNarrow(std::wstring in);
private:
	std::string logpath;
};

