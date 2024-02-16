#pragma once
#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iterator>

template<class T, size_t N>
class BoundedBuffer
{
public:
	BoundedBuffer()
		:_fill_ptr(0)
		, _use_ptr(0)
		, _count(0)
	{
		_buffer = std::array<T, N>{};
	}

	~BoundedBuffer() {}

	BoundedBuffer(const BoundedBuffer& orig)
		:_fill_ptr(orig._fill_ptr)
		, _use_ptr(orig._use_ptr)
		, _count(orig._count)
		, Mutex(orig.Mutex)
		, EmptyCV(orig.EmptyCV)
		, FillCV(orig.FillCV)
	{
		for (int i = 0; i < orig._buffer.size(); ++i)
		{
			_buffer[i] = orig._buffer[i];
		}
	}

	BoundedBuffer& operator=(const BoundedBuffer& rhs)
	{
		BoundedBuffer temp(rhs);
		swap(temp);
		return *this;
	}

	BoundedBuffer(BoundedBuffer&& orig)
		:_fill_ptr(orig._fill_ptr)
		, _use_ptr(orig._use_ptr)
		, _count(orig._count)
		, Mutex(std::move(orig.Mutex))
		, EmptyCV(std::move(orig.EmptyCV))
		, FillCV(std::move(orig.FillCV))
		, _buffer(std::move(orig._buffer))
	{
	}

	BoundedBuffer& operator=(BoundedBuffer&& rhs)
	{
		if (this != &rhs)
		{
			_fill_ptr = rhs._fill_ptr;
			_use_ptr = rhs._use_ptr;
			_count = rhs._count;
			Mutex = std::move(rhs.Mutex);
			EmptyCV = std::move(rhs.EmptyCV);
			FillCV = std::move(rhs.FillCV);
			_buffer = std::move(rhs._buffer);
		}
		return *this;
	}

	void Put(T& data)
	{
		std::unique_lock<std::mutex> lck(Mutex);

		while (Count() == MAX())
		{
			EmptyCV.wait(lck);
		}

		_buffer[_fill_ptr] = data;
		_fill_ptr = (_fill_ptr + 1) % MAX();
		_count++;
	}

	void Put(T&& data)
	{
		std::unique_lock<std::mutex> lck(Mutex);

		while (Count() == MAX())
		{
			EmptyCV.wait(lck);
		}

		_buffer[_fill_ptr] = std::move(data);
		_fill_ptr = (_fill_ptr + 1) % MAX();
		_count++;
		
		FillCV.notify_all();
	}


	bool Get(T& data, int timeout)
	{
		std::unique_lock<std::mutex> lck(Mutex);
		const bool hasData = FillCV.wait_for(lck, std::chrono::milliseconds{ timeout }, [this]() noexcept {return Count() > 0; });
		if (hasData)
		{
			data = _buffer[_use_ptr];
			_use_ptr = (_use_ptr + 1) % MAX();
			_count--;
			EmptyCV.notify_all();
		}
		return hasData;
	}


	bool Get(T&& data, int timeout)
	{
		std::unique_lock<std::mutex> lck(Mutex);
		const bool hasData = FillCV.wait_for(lck, std::chrono::milliseconds{ timeout }, [this]() noexcept {return Count() > 0; });
		if (hasData)
		{
			data = std::move(_buffer[_use_ptr]);
			_use_ptr = (_use_ptr + 1) % MAX();
			_count--;
			EmptyCV.notify_all();
		}
		return hasData;
	}

private:
	size_t Count() noexcept
	{
		return _count;
	}

	size_t MAX() noexcept
	{
		return _buffer.max_size();
	}

private:
	void swap(BoundedBuffer& src)
	{
		_buffer = std::array<T, N>{};
		_buffer.swap(src._buffer);
		std::swap(_fill_ptr, src._fill_ptr);
		std::swap(_use_ptr, src._use_ptr);
		std::swap(_count, src._count);
#ifdef _WIN32
		std::swap(Mutex, src.Mutex);
		std::swap(EmptyCV, src.EmptyCV);
		std::swap(FillCV, src.FillCV);
#else
		Mutex = src.Mutex;
		EmptyCV = src.EmptyCV;
		FillCV = src.FillCV;
#endif
	}

private:
	std::array<T, N> _buffer;
	size_t _fill_ptr;
	size_t _use_ptr;
	size_t _count;

private:
	std::mutex Mutex;
	std::condition_variable EmptyCV;
	std::condition_variable FillCV;
};