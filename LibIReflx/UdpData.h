#pragma once

#include <vector>

typedef unsigned char BYTE;

class UdpData
{
public:
	enum {
		DEFAULT_BUFLEN = 1500
	};

	UdpData();
	UdpData(BYTE* buf, uint32_t len);
	UdpData(const UdpData& rhs);
	UdpData& operator=(const UdpData& rhs);
	UdpData(UdpData&& src) noexcept;
	UdpData& operator=(UdpData&& rhs) noexcept;
	~UdpData(void);

	void swap(UdpData& src);

	BYTE* data();
	const BYTE* data() const;
	size_t length() const;

	UdpData& write(const BYTE* data, int len);

private:
	std::vector<BYTE> _data;
};

