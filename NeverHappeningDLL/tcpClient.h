#pragma once
#include "stdafx.h"
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "10000"
#define DEFAULT_SERVER "SERVERIP"

class TcpClient {
public:
	int __cdecl start(std::string message);
};