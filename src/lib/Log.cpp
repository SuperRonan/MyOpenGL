#include "Log.h"
#include <iostream>

namespace lib
{
	void info_log(std::string const& message)
	{
		info_log(message.c_str());
	}

	void info_log(const char* message)
	{
		std::cout << message << '\n';
	}

	void error_log(std::string const& message)
	{
		error_log(message.c_str());
	}

	void error_log(const char* message)
	{
		std::cerr << message << '\n';
	}

}