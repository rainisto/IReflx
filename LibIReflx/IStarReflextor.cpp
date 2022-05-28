#include "IStarReflextor.h"

#include "SourceReader.h"
#include "UdpSender.h"

#include <thread>

namespace ThetaStream
{
	class IStarReflextor::Impl
	{
	public:
		uint64_t _inCount{0};
		uint64_t _outCount{0};
		SourceReader* _sourceReader{};
		UdpSender* _destReader{};
		ThetaStream::CommandLineParser _cmdLine;
	};


	IStarReflextor::IStarReflextor()
	{
		_pimpl = std::make_unique<ThetaStream::IStarReflextor::Impl>();
	}

	IStarReflextor::IStarReflextor(const ThetaStream::CommandLineParser& cmdLine)
	{
		_pimpl = std::make_unique<ThetaStream::IStarReflextor::Impl>();
		_pimpl->_cmdLine = cmdLine;
	}

	IStarReflextor::~IStarReflextor()
	{
	}

	void IStarReflextor::init(const ThetaStream::CommandLineParser& cmdLine)
	{
		_pimpl->_cmdLine = cmdLine;
	}

	int IStarReflextor::run()
	{
		BaseIOInterface::QueueType queue;

		SourceReader reader(_pimpl->_cmdLine.sourceIp(), _pimpl->_cmdLine.sourcePort(), queue, _pimpl->_cmdLine.sourceInterfaceAddress());
		UdpSender sender(_pimpl->_cmdLine.destinationIp(), _pimpl->_cmdLine.destinationPort(), queue, _pimpl->_cmdLine.ttl(), _pimpl->_cmdLine.destinationInterfaceAddress());

		_pimpl->_sourceReader = &reader;
		_pimpl->_destReader = &sender;

		std::thread listenThread{ &SourceReader::operator(), &reader };
		std::thread senderThread{ &UdpSender::operator(), &sender };

		listenThread.join();
		senderThread.join();

		_pimpl->_inCount = reader.count();
		_pimpl->_outCount = sender.count();

		return 0;
	}

	void IStarReflextor::stop()
	{
		_pimpl->_sourceReader->stop();
		_pimpl->_destReader->stop();
	}

	uint64_t IStarReflextor::inCount() const
	{
		return _pimpl->_inCount;
	}

	uint64_t IStarReflextor::outCount() const
	{
		return _pimpl->_outCount;
	}

}