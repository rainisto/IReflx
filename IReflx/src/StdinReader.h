#pragma once

#include "BaseIOInterface.h"
#include "BoundedBuffer.h"
#include "UdpData.h"

class StdinReader :
	 public BaseIOInterface
{
public:
	StdinReader(BaseIOInterface::QueueType& queue);
	virtual ~StdinReader();

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
};

