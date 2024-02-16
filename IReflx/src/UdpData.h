#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

class UdpData
{
public:
	enum {
		DEFAULT_BUFLEN = 1500
	};

	UdpData();
	UdpData(uint8_t* buf, uint32_t len);
	UdpData(const UdpData& rhs);
	UdpData& operator=(const UdpData& rhs);
	UdpData(UdpData&& src) noexcept;
	UdpData& operator=(UdpData&& rhs) noexcept;
	~UdpData(void);

	void swap(UdpData& src);

	uint8_t* data();
	const uint8_t* data() const;
	size_t length() const;

	UdpData& write(const uint8_t* data, int len);

private:
	std::vector<uint8_t> _data;
};

