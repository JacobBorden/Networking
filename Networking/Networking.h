// Networking.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

namespace Socket {


	class Socket{
	public:
		Socket();
		Socket(char* _pHost, int _pPort);
		~Socket();
		bool InitSocket();
		bool CreateSocket(char* _pHost, int _pPort);
		bool ConnectSocket();
		void SetAISocketType(int _pSocktype);
		void SetAIProtocol(int _pProtocol);
		void SetAIFamily(int _pFamily);
		bool Send(char* _pSendBuffer);
		std::vector<char> Receive();
		void Disconnect();
		bool ShutDown();

	private:
		
		WORD versionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		bool isConnected = FALSE;
		addrinfo addressInfo;
		addrinfo* hostAddressInfo;
		SOCKET connectionSocket;
	};
}
