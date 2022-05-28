#include "UdpSender.h"

#include <io.h>
#include <iostream>
#include <fcntl.h>

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
const int BUFLEN = 1500;

class UdpSender::Impl
{
public:
	Impl(BaseIOInterface::QueueType& q)
		:_queue(q)
	{}

public:
	SOCKET _socket{INVALID_SOCKET};
	bool _run{true};
	sockaddr_in _recvAddr{};
	QueueType& _queue;
	uint64_t _count{};
	uint64_t _bytes{};
	uint32_t _address{};
	uint32_t _port{};
};

UdpSender::UdpSender(const char* ipaddr, uint32_t port, UdpSender::QueueType& q, unsigned char ttl, const char* iface_addr)
{
	_pimpl = std::make_unique<UdpSender::Impl>(q);

	if (strcmp(ipaddr, "-") == 0) // send the data onto a socket
	{
		_setmode(_fileno(stdout), _O_BINARY);
	}
	else // write the data to stdout
	{
		initSocket(ipaddr, port, ttl, iface_addr);
	}
}

UdpSender::~UdpSender(void)
{

}

void UdpSender::initSocket(const char* ipaddr, uint32_t port, unsigned char ttl, const char* iface_addr)
{
	WORD winsock_version, err;
	WSADATA winsock_data;
	winsock_version = MAKEWORD(2, 2);
	IN_ADDR inaddr;

	err = WSAStartup(winsock_version, &winsock_data);
	if (err != 0)
	{
		std::exception exp("Failed to initialize WinSock");
		throw exp;
	}

	//----------------------
	// Create a SOCKET for connecting to server
	_pimpl->_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_pimpl->_socket == INVALID_SOCKET) {
		WSACleanup();
		char szErr[BUFSIZ];
		sprintf(szErr, "Error at socket(): %d", WSAGetLastError());
		std::exception exp(szErr);
		throw exp;
	}

	// Set time to live
	unsigned char optVal = ttl;
	int optLen = sizeof(optVal);

	if (setsockopt(_pimpl->_socket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&optVal, optLen) == SOCKET_ERROR)
	{
		WSACleanup();
		char szErr[BUFSIZ];
		sprintf(szErr, "Error setting socket option IP_MULTICAST_TTL: %d", WSAGetLastError());
		std::exception exp(szErr);
		throw exp;
	}

	if (strlen(iface_addr) > 0)
	{
		inet_pton(AF_INET, iface_addr, (PVOID)&inaddr);
		if (setsockopt(_pimpl->_socket, IPPROTO_IP, IP_MULTICAST_IF, (char*)&inaddr.S_un.S_addr, sizeof(inaddr.S_un.S_addr)) == SOCKET_ERROR)
		{
			WSACleanup();
			char szErr[BUFSIZ];
			sprintf(szErr, "UdpSender Error setting socket option IP_MULTICAST_IF: %d", WSAGetLastError());
			std::exception exp(szErr);
			throw exp;
		}
	}

	if (inet_pton(AF_INET, ipaddr, (PVOID)&inaddr) != 1)
	{
		WSACleanup();
		char szErr[BUFSIZ];
		sprintf(szErr, "Error when calling inet_pton: %d", WSAGetLastError());
		std::exception exp(szErr);
		throw exp;
	}
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being send to
	_pimpl->_recvAddr.sin_family = AF_INET;
	_pimpl->_recvAddr.sin_addr.s_addr = inaddr.S_un.S_addr;
	_pimpl->_recvAddr.sin_port = htons(port);

	_pimpl->_address = _pimpl->_recvAddr.sin_addr.S_un.S_addr;
}

void UdpSender::stop()
{
	if (_pimpl->_socket != INVALID_SOCKET)
	{
		shutdown(_pimpl->_socket, SD_SEND);
		closesocket(_pimpl->_socket);
	}
	_pimpl->_run = false;
}

void UdpSender::operator()()
{
	while (_pimpl->_run)
	{
		UdpData d;
		const bool hasData = _pimpl->_queue.Get(std::forward<UdpData>(d), 100);
		if (hasData)
		{
			send(d);
		}
	}
}

void UdpSender::send(const UdpData& data)
{
	// Write the data to the standard console out
	if (_pimpl->_socket == INVALID_SOCKET)
	{
		size_t w = 0;
		for (size_t nleft = data.length(); nleft > 0;)
		{
			w = fwrite(data.data(), 1, nleft, stdout);
			if (w == 0)
			{
				std::cerr << "error: unable to write " << nleft << " bytes to stdout" << std::endl;
			}
			else
			{
				_pimpl->_count++;
				_pimpl->_bytes += data.length();
			}
			nleft -= w;
			fflush(stdout);
		}
	}
	else // Send the data onto a socket
	{
		const int status = sendto(_pimpl->_socket,
			(char*)data.data(),
			(int)data.length(),
			0,
			(SOCKADDR*)&_pimpl->_recvAddr,
			sizeof(_pimpl->_recvAddr));

		if (status == SOCKET_ERROR)
		{
			const UINT32 errCode = WSAGetLastError();
			cerr << "UDP Sending Error: " << errCode << endl;
		}
		else
		{
			_pimpl->_count++;
			_pimpl->_bytes += data.length();
		}
	}
}

uint64_t UdpSender::count() noexcept
{
	return _pimpl->_count;
}

uint64_t UdpSender::bytes() noexcept
{
	const uint64_t ret = _pimpl->_bytes;
	_pimpl->_bytes = 0;
	return ret;
}

uint32_t UdpSender::address() noexcept
{
	return _pimpl->_address;
}

uint32_t UdpSender::port() noexcept
{
	return _pimpl->_port;
}
