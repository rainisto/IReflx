#pragma once

#include "BaseIOInterface.h"
#include "UdpData.h"

class SourceReader :
	public BaseIOInterface
{
public:
	SourceReader(const char* ipmulticast, uint32_t port, BaseIOInterface::QueueType& queue, const char* iface_addr);
	virtual ~SourceReader();

	void stop() noexcept override;

	uint64_t count() noexcept override;

	uint64_t bytes() noexcept override;

	uint32_t address() noexcept override;

	uint32_t port() noexcept override;

	void operator ()() override;

private:
	BaseIOInterface* _pimpl;
};

