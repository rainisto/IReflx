#pragma once

#include <memory>

namespace ThetaStream
{
	class CommandLineParser
	{
	public:
		CommandLineParser();
		~CommandLineParser();
		CommandLineParser(const CommandLineParser& other);
		CommandLineParser& operator=(const CommandLineParser& rhs);

		void parse(int argc, char** argv, const char* appname);

		const char* sourceIp() const;
		const char* destinationIp() const;
		const char* sourceInterfaceAddress() const;
		const char* destinationInterfaceAddress() const;
		int sourcePort() const;
		int destinationPort() const;
		int ttl() const;

	private:
		void swap(CommandLineParser& other);

		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};

}
