#pragma once

#include "BaseIOInterface.h"



class UdpSender :
	public BaseIOInterface
{
public:
	UdpSender(const char* ipaddr, uint32_t port, BaseIOInterface::QueueType& queue, unsigned char ttl, const char* iface_addr);
	~UdpSender();

	void operator()();

	void stop() override;

	uint64_t count() noexcept override;

	uint64_t bytes() noexcept override;

	uint32_t address() noexcept override;

	uint32_t port() noexcept override;

private:
	void send(const UdpData& data);
	void initSocket(const char* ipaddr, uint32_t port, unsigned char ttl, const char* iface_addr);

private:
	class Impl;
	std::unique_ptr<Impl> _pimpl;
};

