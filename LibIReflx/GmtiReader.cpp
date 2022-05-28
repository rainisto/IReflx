#include "GmtiReader.h"

#include <io.h>
#include <fcntl.h>


GmtiReader::GmtiReader(QueueType& queue)
	:_count(0)
	, _bytes(0)
	, _run(true)
	, _queue(queue)
	, _packetSize(-1)
	, _pos(0)
{
	_setmode(_fileno(stdin), _O_BINARY);
}

GmtiReader::~GmtiReader()
{
}

void GmtiReader::stop() noexcept
{
	_run = false;
}

uint64_t GmtiReader::count() noexcept
{
	return _count;
}

uint64_t GmtiReader::bytes() noexcept
{
	const uint64_t ret = _bytes;
	_bytes = 0;
	return ret;
}

uint32_t GmtiReader::address() noexcept
{
	return 0;
}

uint32_t GmtiReader::port() noexcept
{
	return 0;
}

void GmtiReader::operator()()
{
	BYTE buffer[BUFSIZ];
	while (_run)
	{
		size_t r = fread(buffer, 1, sizeof(buffer), stdin);
		if (r > 0)
		{
			_bytes += r;
			for (int i = 0; i < r; i++, _pos++)
			{
				if (_pos == 0)
				{
					_message.clear();
				}

				_message.push_back(buffer[i]);

				// Packet Size
				switch (_pos)
				{
				case 2: _packetSizeBuf[3] = buffer[i]; break;
				case 3: _packetSizeBuf[2] = buffer[i]; break;
				case 4: _packetSizeBuf[1] = buffer[i]; break;
				case 5:
					_packetSizeBuf[0] = buffer[i];
					memcpy(&_packetSize, _packetSizeBuf, 4);
					break;
				}

				if (_pos == _packetSize - 1)
				{
					_queue.Put(UdpData(_message.data(), _packetSize));
					_pos = -1;
					_packetSize = -1;
					_count++;
				}
			}
		}
	}
}
