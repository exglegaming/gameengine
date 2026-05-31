#ifndef LOGGER_H
#define LOGGER_H

#include <vector>
#include <string>

enum log_type 
{
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

struct log_entry 
{
	log_type type;
	std::string message;
};

class logger 
{
	public:
		static std::vector<log_entry> messages;
		static void log(const std::string& message);
		static void err(const std::string& message);
};

#endif