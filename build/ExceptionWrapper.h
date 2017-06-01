#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

#define throw_exception(exc) throw ExceptionWrapper(exc, __FILE__, __LINE__);

class ExceptionWrapper : public std::runtime_error {

	std::string msg;

public:
	ExceptionWrapper(const std::runtime_error err, const char* file, int line);
	~ExceptionWrapper();
	const char *what() const;
};

#pragma once
