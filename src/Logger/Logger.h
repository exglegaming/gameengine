#ifndef LOGGER_H
#define LOGGER_H

#include <vector>
#include <string>

enum LogType 
{
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

struct LogEntry 
{
	LogType type;
	std::string message;
};

class Logger 
{
	public:
		static std::vector<LogEntry> messages;
		static void log(const std::string& message);
		static void err(const std::string& message);
};

#endif