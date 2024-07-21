#pragma once
#include "globals.h"
#include <iostream>
#include <fstream>
#include <string>

#define PACKET_BUFFER_SIZE 64000 * 16

class Logger
{
public:
	Logger();
	Logger(char* path);
	~Logger();
	const enum LOGTYPE {
		LOGTYPE_NONE,
		LOGTYPE_SERVER,
		LOGTYPE_PACKET,
		LOGTYPE_ERROR,
		LOGTYPE_DEBUG,
		LOGTYPE_COMM,
		LOGTYPE_Client,
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
	void CheckLogPath(std::string path);
	std::string logpath;
};

