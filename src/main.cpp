// IReflx.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <IReflx/IStarReflextor.h>
#include <iostream>

ThetaStream::IStarReflextor* pReflx;

#ifdef _WIN32
#include <Windows.h>

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal.
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	case CTRL_LOGOFF_EVENT:
		pReflx->stop();
		std::cerr << "Closing down, please wait..." << std::endl << std::endl;
		return TRUE;
	default:
		return FALSE;
	}
}
#endif

void PrintHeader()
{
	std::cerr << "ISTAR Reflextor Console Application" << std::endl;
	std::cerr << "Developed by James McAvoy, jimcavoy@thetastream.com, ThetaStream Consulting" << std::endl << std::endl;
	std::cerr << std::endl << "Enter Ctrl-C to exit" << std::endl << std::endl;
}

int main(int argc, char* argv[])
{
	int ret = 0;
	try
	{
		ThetaStream::CommandLineParser cmdline;
		cmdline.parse(argc, argv, "IReflx.exe");

		PrintHeader();
#ifdef _WIN32
		if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
		{
			DWORD err = GetLastError();
			std::cerr << "Failed to set console control handler.  Error Code = " << err << std::endl;
			return err;
		}
#endif
		ThetaStream::IStarReflextor reflx(cmdline);
		pReflx = &reflx;
		ret = reflx.run();

		if (ret == 0)
		{
			std::cerr << "UDP Packets Received: " << reflx.inCount() << std::endl;
			std::cerr << "UDP Packets Sent: " << reflx.outCount() << std::endl;
		}
	}
	catch (const std::exception& exp)
	{
		std::cerr << exp.what() << std::endl;
		return -1;
	}
	catch (...)
	{
		std::cerr << "Unknown exception caught." << std::endl;
		return -1;
	}
	return ret;
}
