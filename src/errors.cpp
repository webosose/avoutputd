// @@@LICENSE
//
//      Copyright (c) 2016 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#include <string>
#include <sstream>

#include "logging.h"
#include "errors.h"

std::string string_format_valist(const std::string& fmt_str, va_list ap)
{
	size_t n = fmt_str.size() * 2;
	std::unique_ptr<char[]> formatted(new char[n]);
	va_list apCopy;
	va_copy(apCopy, ap);

	int final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
	if (final_n < 0 || final_n >= (int)n)
	{
		/* There was not enough space, retry */
		/* MS implements < 0 as not large enough */
		n = (size_t) (abs(final_n) + 1);

		formatted.reset(new char[n]);
		vsnprintf(&formatted[0], n, fmt_str.c_str(), apCopy);
	}
	va_end(apCopy);

	return std::string(formatted.get());
}

FatalException::FatalException(const char* file, int line, const char* format, ...)
{
	std::stringstream s;
	va_list args;
	va_start(args, format);
	s << file << ":" << line << ": " << string_format_valist(format, args);
	va_end(args);
	mMessage = s.str();
	LOG_ERROR("FATAL_ERROR", 0, "%s", mMessage.c_str());
}

const char* FatalException::what() const noexcept
{
	return mMessage.c_str();
}
