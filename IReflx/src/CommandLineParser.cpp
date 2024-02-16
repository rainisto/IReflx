#include <IReflx/CommandLineParser.h>

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

const char* usage = "Usage: IReflx <OPTIONS>";
const char* opts = "  -s\tSource socket address; otherwise, stdin.\n \
 -d\tDestination socket address; otherwise, stdout.\n \
 -t\tTime to Live. (default: 16)\n \
 -i\tSpecifies the network interface IP address for the source stream.\n \
 -o\tSpecifies the network interface IP address for the destination stream.\n \
 -?\tPrint this message.";

namespace
{
	string getip(const char* ip)
	{
		string sip(ip);
		string::size_type pos = sip.find(":");
		string ret = sip.substr(0, pos);
		return ret;
	}

	int getport(const char* ip)
	{
		string sip(ip);
		string::size_type pos = sip.find(":");
		pos++;
		string port = sip.substr(pos, sip.size() - pos);
		int ret = atoi(port.c_str());
		return ret;
	}
}

namespace ThetaStream
{
	class CommandLineParser::Impl
	{
	public:
		Impl() {}

	public:
		int sourcePort{ 50000 };
		int destinationPort{ 50000 };
		int ttl{ 16 };
		std::string sourceIP{ "-" };
		std::string destinationIP{ "-" };
		std::string ifaceAddr;
		std::string ofaceAddr;
	};
}

ThetaStream::CommandLineParser::CommandLineParser()
{
	_pimpl = std::make_unique<ThetaStream::CommandLineParser::Impl>();
}

ThetaStream::CommandLineParser::~CommandLineParser()
{
}

ThetaStream::CommandLineParser::CommandLineParser(const CommandLineParser& other)
{
	_pimpl = std::make_unique<ThetaStream::CommandLineParser::Impl>();
	_pimpl->destinationPort = other.destinationPort();
	_pimpl->ttl = other.ttl();
	_pimpl->sourceIP = other.sourceIp();
	_pimpl->destinationIP = other.destinationIp();
	_pimpl->ifaceAddr = other.sourceInterfaceAddress();
	_pimpl->ofaceAddr = other.destinationInterfaceAddress();
}

ThetaStream::CommandLineParser& ThetaStream::CommandLineParser::operator=(const ThetaStream::CommandLineParser& rhs)
{
	ThetaStream::CommandLineParser temp(rhs);
	swap(temp);
	return *this;
}

void ThetaStream::CommandLineParser::parse(int argc, char** argv, const char* appname)
{
	string source{"-"};
	string dest{"-"};
	char c{};

	while (--argc > 0 && (*++argv)[0] == '-')
	{
		c = *++argv[0];
		switch (c)
		{
		case 's':
			source = *argv + 1;
			break;
		case 'd':
			dest = *argv + 1;
			break;
		case 'i':
			_pimpl->ifaceAddr = *argv + 1;
			break;
		case 'o':
			_pimpl->ofaceAddr = *argv + 1;
		case 't':
			_pimpl->ttl = std::stoi(*argv + 1);
			break;
		case '?':
		{
			std::stringstream msg;
			msg << usage << endl;
			msg << endl << "Options: " << endl;
			msg << opts << endl;
			std::runtime_error exp(msg.str().c_str());
			throw exp;
		}
		default:
		{
			std::stringstream msg;
			msg << appname << ": illegal option " << c << endl;
			std::runtime_error exp(msg.str().c_str());
			throw exp;
		}
		}
	}

	if (source != "-")
	{
		_pimpl->sourceIP = getip(source.c_str());
		_pimpl->sourcePort = getport(source.c_str());
	}

	if (dest != "-")
	{
		_pimpl->destinationIP = getip(dest.c_str());
		_pimpl->destinationPort = getport(dest.c_str());
	}
}

const char* ThetaStream::CommandLineParser::sourceIp() const
{
	return _pimpl->sourceIP.c_str();
}

const char* ThetaStream::CommandLineParser::destinationIp() const
{
	return _pimpl->destinationIP.c_str();
}

const char* ThetaStream::CommandLineParser::sourceInterfaceAddress() const
{
	return _pimpl->ifaceAddr.c_str();
}

const char* ThetaStream::CommandLineParser::destinationInterfaceAddress() const
{
	return _pimpl->ofaceAddr.c_str();
}

int ThetaStream::CommandLineParser::sourcePort() const
{
	return _pimpl->sourcePort;
}

int ThetaStream::CommandLineParser::destinationPort() const
{
	return _pimpl->destinationPort;
}

int ThetaStream::CommandLineParser::ttl() const
{
	return _pimpl->ttl;
}

void ThetaStream::CommandLineParser::swap(ThetaStream::CommandLineParser& other)
{
	std::swap(_pimpl->sourcePort, other._pimpl->sourcePort);
	std::swap(_pimpl->destinationPort, other._pimpl->destinationPort);
	std::swap(_pimpl->ttl, other._pimpl->ttl);
	_pimpl->destinationIP.swap(other._pimpl->destinationIP);
	_pimpl->sourceIP.swap(other._pimpl->sourceIP);
	_pimpl->ifaceAddr.swap(other._pimpl->ifaceAddr);
	_pimpl->ofaceAddr.swap(other._pimpl->ofaceAddr);
}
