#define FD_SETSIZE				300
#define _WINSOCKAPI_

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <cassert>

int tempValue1 = 0;
int tempValue2 = 0;
int tempValue3 = 0;
int tempValue4 = 0;

#include "strings.h"
#include "globals.h"
#include "server.h"
#include "main.h"
#include "resource.h"

int main()
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Get current flag
	int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// Turn on leak-checking bit.
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

	// Turn off CRT block checking bit.
	tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

	// Set flag to the new value.
	_CrtSetDbgFlag(tmpFlag);

	_set_printf_count_output(1); // Enable %n as random crashing occurred while packet logging
	srand(timeGetTime()); // Seed RNG
	SERVER* server = new SERVER();
	char titleBuf[0x100];
	snprintf(titleBuf, sizeof(titleBuf), "SBOL Battle Server v%s. Clients: 0", server->logger.toNarrow(VERSION_STRING).c_str());
	SetConsoleTitleA(titleBuf);

	HMODULE mainMod = GetModuleHandle(0);
	assert(mainMod);
	HICON mainIcon = ::LoadIcon(mainMod, MAKEINTRESOURCE(MAINICON));
	assert(mainIcon);
	ChangeIcon(mainIcon);

	server->logger.Log(LOGGER::LOGTYPE_NONE, L"SBOL Server version %s by Tofuman", VERSION_STRING);
	server->logger.Log(LOGGER::LOGTYPE_NONE, L"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
	if (server->Start())
	{
		server->logger.Log(LOGGER::LOGTYPE_ERROR, L"Server failed to start");
		return 1;
	}

	while (server->isRunning())
	{
		if (server->managementserver.isConfigured() && !server->managementserver.isRunning() && server->managementserver.shouldRetry())
			server->managementserver.Restart();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	delete server;
	return 0;
}

void ChangeIcon(const HICON hNewIcon)
{
	HMODULE hMod = LoadLibraryA(("Kernel32.dll"));
	typedef DWORD(__stdcall *SCI)(HICON);
	SCI pfnSetConsoleIcon = reinterpret_cast<SCI>(GetProcAddress(hMod, "SetConsoleIcon"));
	pfnSetConsoleIcon(hNewIcon);
	FreeLibrary(hMod);
}
//TODO: Add Rival List status to client data & database
//TODO: Add Team Items to client data & database
//TODO: Support the team garages/extended garages and store in database
//TODO: retrieve the S.DAT and P.DAT from management server to use when client purchases something from shop

