#include "UdpData.h"

#include <iterator>


UdpData::UdpData()
{

}

UdpData::UdpData(uint8_t* buf, uint32_t len)
{
	if (buf != nullptr)
	{
		_data.insert(_data.end(), &buf[0], &buf[len]);
	}
}

// Copy Ctor
UdpData::UdpData(const UdpData& rhs)
{
	std::copy(rhs._data.begin(), rhs._data.end(), std::back_inserter(_data));
}

// Move Ctor
UdpData::UdpData(UdpData&& src) noexcept
{
	*this = std::move(src);
}

// Copy Assignment
UdpData& UdpData::operator=(const UdpData& rhs)
{
	UdpData temp(rhs);
	swap(temp);

	return *this;
}

// Move Assignment
UdpData& UdpData::operator=(UdpData&& rhs) noexcept
{
	if (this != &rhs)
	{
		_data = std::move(rhs._data);
	}

	return *this;
}

UdpData::~UdpData(void)
{
}

void UdpData::swap(UdpData& src)
{
	_data.swap(src._data);
}

uint8_t* UdpData::data()
{
	return _data.data();
}

const uint8_t* UdpData::data() const
{
	return _data.data();
}

size_t UdpData::length() const
{
	return _data.size();
}

UdpData& UdpData::write(const uint8_t* buf, int count)
{
	_data.insert(_data.end(), &buf[0], &buf[count]);
	return *this;
}
