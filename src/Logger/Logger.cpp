#include "logger.h"
#include <iostream>
#include <chrono>
#include <ctime>

std::vector<log_entry> logger::messages;

std::string CurrentDateTimeToString() 
{
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	tm time_info;
	#ifdef _WIN32
    	localtime_s(&time_info, &now);
	#else
    	localtime_r(&now, &time_info);
	#endif

	std::string output(30, '\0');
	std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", &time_info);
	return output;
}

/** 
 * @brief Logs messages to the console.
 * @param message Your message to display to the console.
*/ 
void logger::log(const std::string& message) 
{
	log_entry logEntry;
	logEntry.type = LOG_INFO;
	logEntry.message = " LOG: [" + CurrentDateTimeToString() + "]: " + message;
	std::cout << "\x1B[32m" << logEntry.message << "\033[0m" << std::endl;
	messages.push_back(logEntry);
}

void logger::err(const std::string& message) 
{
	log_entry logEntry;
	logEntry.type = LOG_ERROR;
	logEntry.message = " ERR: [" + CurrentDateTimeToString() + "]: " + message;
	std::cout << "\x1B[91m" << logEntry.message << "\033[0m" << std::endl;
	messages.push_back(logEntry);
}