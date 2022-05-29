#pragma once

#include "BaseIOInterface.h"
#include "UdpData.h"

#include <vector>

class GmtiReader :
    public BaseIOInterface
{
public:
	GmtiReader(BaseIOInterface::QueueType& queue);
	virtual ~GmtiReader();

	void stop() noexcept override;

	uint64_t count() noexcept override;

	uint64_t bytes() noexcept override;

	uint32_t address() noexcept override;

	uint32_t port() noexcept override;

	void operator ()() override;

private:
	uint64_t _count;
	uint64_t _bytes;
	bool _run;
	BaseIOInterface::QueueType& _queue;
	int _packetSize;
	int _pos;
	uint8_t _packetSizeBuf[4]{};
	std::vector<uint8_t> _message;
};

