#include "UdpListener.h"

#include <iostream>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

#ifdef _WIN32
#define sprintf sprintf_s
#else
#define sprintf sprintf
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define PVOID void*
#define SD_SEND SHUT_WR
#define SOCKADDR sockaddr
#define IN_ADDR in_addr
#endif


using namespace std;

#define DEFAULT_BUFLEN 4016

class UdpListener::Impl
{
public:
	Impl(BaseIOInterface::QueueType& q)
		:_queue(q)
	{

	}

public:
	uint64_t _count{};
	uint64_t _bytes{};
	SOCKET _listenSocket{ INVALID_SOCKET };
	bool _run{true};
	BaseIOInterface::QueueType& _queue;
	std::string _ipmcast{};
	uint32_t _address{};
	uint32_t _port{};
};

UdpListener::UdpListener(const char* ipmulticast, uint32_t port, BaseIOInterface::QueueType& queue, const char* iface_addr)
{
	_pimpl = std::make_unique <UdpListener::Impl>(queue);
	WORD winsock_version, err;
	WSADATA winsock_data;
	winsock_version = MAKEWORD(2, 2);
	err = WSAStartup(winsock_version, &winsock_data);
	if (err != 0)
	{
		std::exception exp("Failed to initialize WinSock");
		throw exp;
	}

	//----------------------
	// Create a SOCKET for listening for 
	// incoming connection requests
	_pimpl->_listenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_pimpl->_listenSocket == INVALID_SOCKET) {
		WSACleanup();
		char szErr[BUFSIZ];
		sprintf(szErr, "Error at socket(): %d", WSAGetLastError());
		std::exception exp(szErr);
		throw exp;
	}

	//----------------------
	// Reuse address
	BOOL optVal = TRUE;
	int optLen = sizeof(BOOL);

	if (setsockopt(_pimpl->_listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optVal, optLen) == SOCKET_ERROR)
	{
		WSACleanup();
		char szErr[BUFSIZ];
		sprintf(szErr, "Error setting socket option SO_REUSEADDR: %d", WSAGetLastError());
		std::exception exp(szErr);
		throw exp;
	}

	//----------------------
	// enable to receive broadcast messages
	optVal = TRUE;
	optLen = sizeof(BOOL);

	if (setsockopt(_pimpl->_listenSocket, SOL_SOCKET, SO_BROADCAST, (char*)&optVal, optLen) == SOCKET_ERROR)
	{
		WSACleanup();
		char szErr[BUFSIZ];
		sprintf(szErr, "Error setting socket option SO_BROADCAST: %d", WSAGetLastError());
		std::exception exp(szErr);
		throw exp;
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	sockaddr_in service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	if (strlen(iface_addr) > 0)
	{
		inet_pton(AF_INET, iface_addr, (PVOID)&service.sin_addr);
	}
	else
	{
		service.sin_addr.s_addr = INADDR_ANY;
	}
	service.sin_port = htons(port);

	//----------------------
	// Bind the socket.
	if (bind(_pimpl->_listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		char szErr[BUFSIZ];
		sprintf_s(szErr, "UdpListner bind() failed : %i", WSAGetLastError());
		closesocket(_pimpl->_listenSocket);
		std::exception exp(szErr);
		throw(exp);
	}

	//----------------------
	// Join multicast group
	struct ip_mreq stIpMreq;
	int ret = inet_pton(AF_INET, ipmulticast, (PVOID)&stIpMreq.imr_multiaddr);

	// Not a valid IPv4 dotted-decimal string. Therefore, do a DNS lookup and try again.
	if (ret == 0)
	{
		struct addrinfo* results = nullptr;
		struct addrinfo* ptr = nullptr;
		struct addrinfo hints;
		struct sockaddr_in* sockaddr_ipv4;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;

		DWORD dwRetval = getaddrinfo(_pimpl->_ipmcast.c_str(), nullptr, &hints, &results);

		if (dwRetval != 0)
		{
			char szErr[BUFSIZ];
			sprintf_s(szErr, "Unable to open input stream, %s.  Error code: %d\n", _pimpl->_ipmcast.c_str(), dwRetval);
			closesocket(_pimpl->_listenSocket);
			std::exception exp(szErr);
			throw(exp);
		}

		for (ptr = results; ptr != nullptr; ptr = ptr->ai_next)
		{
			if (ptr->ai_family == AF_INET)
			{
				sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
				_pimpl->_ipmcast = inet_ntoa(sockaddr_ipv4->sin_addr);
				stIpMreq.imr_multiaddr = sockaddr_ipv4->sin_addr;
				ret = 1;
				break;
			}
		}
	}

	if (ret == 1)
	{
		stIpMreq.imr_interface.s_addr = service.sin_addr.s_addr;
		_pimpl->_address = stIpMreq.imr_multiaddr.S_un.S_addr;
		ret = setsockopt(_pimpl->_listenSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char FAR*) & stIpMreq, sizeof(struct ip_mreq));

		if (ret == SOCKET_ERROR) {
			//cerr << ipmulticast << " is unicast IP address" << endl;
		}
	}
}

UdpListener::~UdpListener(void)
{
	if (_pimpl->_run)
		stop();
	WSACleanup();
}

void UdpListener::stop() noexcept
{
	if (_pimpl->_listenSocket != INVALID_SOCKET)
	{
		struct ip_mreq mreq;
		SecureZeroMemory(&mreq, sizeof(ip_mreq));
		mreq.imr_interface.s_addr = INADDR_ANY;
		int ret = inet_pton(AF_INET, _pimpl->_ipmcast.c_str(), (PVOID)&mreq.imr_multiaddr);

		ret = setsockopt(_pimpl->_listenSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
		if (ret != 0)
		{
			char szErr[BUFSIZ];
			sprintf(szErr, "UdpListner Error setting socket option IP_DROP_MEMBERSHIP: %d", WSAGetLastError());
			OutputDebugString(szErr);
		}
		shutdown(_pimpl->_listenSocket, SD_RECEIVE);
		closesocket(_pimpl->_listenSocket);
		_pimpl->_listenSocket = INVALID_SOCKET;
	}
	_pimpl->_run = false;
}

void UdpListener::operator()()
{
	int nRead = 0;
	BYTE buff[DEFAULT_BUFLEN];
	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	while (_pimpl->_run)
	{
		nRead = recvfrom(_pimpl->_listenSocket,
			(char*)buff, DEFAULT_BUFLEN, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

		switch (nRead)
		{
		case 0:
			stop();
			break;
		case SOCKET_ERROR:
		{
			const UINT32 errCode = WSAGetLastError();
			if (errCode != WSAESHUTDOWN && errCode != WSAEINTR)
			{
				cerr << "UdpListner Socket Error: " << errCode << endl;
			}
			stop();
		}
		break;
		default:
		{
			_pimpl->_count++;
			_pimpl->_queue.Put(UdpData(buff, nRead));
			_pimpl->_bytes += nRead;
		}
		} //switch
	}
}

uint64_t UdpListener::count() noexcept
{
	return _pimpl->_count;
}

uint64_t UdpListener::bytes() noexcept
{
	const uint64_t ret = _pimpl->_bytes;
	_pimpl->_bytes = 0;
	return ret;
}

uint32_t UdpListener::address() noexcept
{
	return _pimpl->_address;
}

uint32_t UdpListener::port() noexcept
{
	return _pimpl->_port;
}

