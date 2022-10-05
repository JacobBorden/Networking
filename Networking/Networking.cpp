// Networking.cpp : Defines the entry point for the application.
//

#include "Networking.h"

Socket::Socket::Socket()
{

}

Socket::Socket::Socket(char* _pHost, int _pPort)
{
	try {
		InitSocket();
		CreateSocket(_pHost, _pPort);
		ConnectSocket();
	}

	catch(int errorCode){
		std::cout << "Exception thrown. Error code " << errorCode;
	}
	

}

Socket::Socket::~Socket()
{

}

bool Socket::Socket::InitSocket()
{
	int errorCode = WSAStartup(versionRequested, &wsaData);
	if (errorCode)
		throw errorCode;
	return true;
}

bool Socket::Socket::CreateSocket(char* _pHost, int _pPort)
{

	int length = log10(_pPort) + 2;	//Figure out the number of  digits using log10. 
									//Add a digit for the ones place
									//add another for a NULL termination character
	char* portInfo = new char[length];
	_itoa_s(_pPort, portInfo, length, 10); //convert an int to a string
	portInfo[length] = '\n';
	ZeroMemory(&addressInfo, sizeof(addressInfo));
	//Make class functions to set these
	SetAIFamily(AF_INET6);
	SetAISocketType(SOCK_STREAM);
	SetAIProtocol(IPPROTO_TCP);
	int errorCode = getaddrinfo((PCSTR)_pHost, (PCSTR)portInfo, &addressInfo, &hostAddressInfo);
	if (errorCode)
	{
		WSACleanup();
		throw errorCode;
	}

	connectionSocket = socket(hostAddressInfo->ai_family, hostAddressInfo->ai_socktype, hostAddressInfo->ai_protocol);
	if (connectionSocket == INVALID_SOCKET)
	{
		errorCode = WSAGetLastError();
		freeaddrinfo(hostAddressInfo);
		WSACleanup();
		throw errorCode;
	}

	return true;
}

bool Socket::Socket::ConnectSocket()
{
	int errorCode = connect(connectionSocket, hostAddressInfo->ai_addr, hostAddressInfo->ai_addrlen);
	if (errorCode)
	{
		errorCode = WSAGetLastError();
		Disconnect();
		throw errorCode;
	}

	freeaddrinfo(hostAddressInfo);
	isConnected = true;
	std::cout << std::endl << "Successfully connected";
	return true;
}

void Socket::Socket::SetAISocketType(int _pSocktype)
{
	addressInfo.ai_socktype = _pSocktype;
}

void Socket::Socket::SetAIProtocol(int _pProtocol)
{
	addressInfo.ai_protocol = _pProtocol;
}

void Socket::Socket::SetAIFamily(int _pFamily)
{
	addressInfo.ai_family = _pFamily;
}

bool Socket::Socket::Send(char* _pSendBuffer)
{
	int errorCode = send(connectionSocket, _pSendBuffer, strlen(_pSendBuffer), 0);
	if (errorCode)
	{
		Disconnect();
		errorCode = WSAGetLastError();
		throw errorCode;
	}
	
	return true;
}

std::vector<char> Socket::Socket::Receive()
{
	int bytesReceived = 0;
	
	std::vector<char> receiveBuffer;

	do {
		int bufferStart = receiveBuffer.size();
		receiveBuffer.resize(bufferStart + 512);
		bytesReceived = recv(connectionSocket, &receiveBuffer[bufferStart], 512, 0);
		receiveBuffer.resize(bufferStart + bytesReceived + 1);
		if (bytesReceived < 0)
		{
			int errorCode = WSAGetLastError();
			Disconnect();
			throw errorCode;
		}

	} while (bytesReceived == 512);

	return receiveBuffer;
}

void Socket::Socket::Disconnect()
{
	closesocket(connectionSocket);
	freeaddrinfo(hostAddressInfo);
	WSACleanup();
	isConnected = false;
}

bool Socket::Socket::ShutDown()
{
	int errorCode = shutdown(connectionSocket, SD_BOTH);
	if (errorCode)
	{
		errorCode = WSAGetLastError();
		Disconnect();
		throw errorCode;
	}

	Disconnect();
	return true;
}