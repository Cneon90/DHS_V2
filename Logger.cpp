#include "Logger.h"

std::ofstream Logger::logFile;
Logger::LogLevel Logger::logLevel = Logger::LogLevel::log_DEBUG; 
std::mutex Logger::logMutex;

bool Logger::consoleOutputEnabled = true;



