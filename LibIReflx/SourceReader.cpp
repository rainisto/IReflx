#include "SourceReader.h"

#include "UdpListener.h"
#include "StdinReader.h"
#include "GmtiReader.h"

SourceReader::SourceReader(const char* ipmulticast, uint32_t port, BaseIOInterface::QueueType& queue, const char* iface_addr)
	: _pimpl(nullptr)
{
	if (ipmulticast != nullptr && strcmp(ipmulticast, "-") != 0)
	{
		if (_stricmp(ipmulticast, "gmti") == 0)
		{
			_pimpl = new GmtiReader(queue);
		}
		else
		{
			_pimpl = new UdpListener(ipmulticast, port, queue, iface_addr);
		}
	}
	else
	{
		_pimpl = new StdinReader(queue);
	}
}

SourceReader::~SourceReader()
{
	delete _pimpl;
}

void SourceReader::stop() noexcept
{
	if (_pimpl != nullptr)
	{
		_pimpl->stop();
	}
}

uint64_t SourceReader::count() noexcept
{
	if (_pimpl != nullptr)
	{
		return _pimpl->count();
	}
	return uint64_t();
}

uint64_t SourceReader::bytes() noexcept
{
	if (_pimpl != nullptr)
	{
		return _pimpl->bytes();
	}
	return uint64_t();
}

uint32_t SourceReader::address() noexcept
{
	if (_pimpl != nullptr)
	{
		return _pimpl->address();
	}
	return uint32_t();
}

uint32_t SourceReader::port() noexcept
{
	if (_pimpl != nullptr)
	{
		return _pimpl->port();
	}
	return uint32_t();
}

void SourceReader::operator()()
{
	if (_pimpl != nullptr)
	{
		_pimpl->operator()();
	}
}
