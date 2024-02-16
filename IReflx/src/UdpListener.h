#pragma once

#include "BaseIOInterface.h"

#include <memory>

class UdpListener :
	public BaseIOInterface
{
public:
	UdpListener(const char* ipmulticast, uint32_t port, BaseIOInterface::QueueType& queue, const char* iface_addr);
	virtual ~UdpListener(void);

	void stop() noexcept override;

	uint64_t count() noexcept override;

	uint64_t bytes() noexcept override;

	uint32_t address() noexcept override;

	uint32_t port() noexcept override;

	void operator ()() override;

private:
	class Impl;
	std::unique_ptr<Impl> _pimpl;
};

