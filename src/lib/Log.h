#pragma once

#include <string>

namespace lib
{
	// TODO use variadic arguments
	static void info_log(std::string const& message);

	static void info_log(const char* message);

	static void error_log(std::string const& message);

	static void error_log(const char* message);
}