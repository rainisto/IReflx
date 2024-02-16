#pragma once

#include <IReflx/CommandLineParser.h>

namespace ThetaStream
{
	class IStarReflextor
	{
	public:
		IStarReflextor();
		IStarReflextor(const ThetaStream::CommandLineParser& cmdLine);
		~IStarReflextor();

		void init(const ThetaStream::CommandLineParser& cmdLine);

		int run();

		void stop();

		uint64_t inCount() const;
		uint64_t outCount() const;

	private:
		class Impl;
		std::unique_ptr<Impl> _pimpl;
	};
}