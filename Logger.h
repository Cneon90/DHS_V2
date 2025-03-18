#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <mutex>

#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdarg>

class Logger {
public:
	enum class LogLevel {
	    log_DEBUG,
	    log_INFO,
	    log_WARNING,
	    log_ERROR
	};
   
    static void setLogLevel(LogLevel level) {
        logLevel = level;
    }

    
//    static void log(LogLevel level, const std::string& message) {
//        if (level >= logLevel) {
//            std::lock_guard<std::mutex> guard(logMutex);
//            std::string levelStr = logLevelToString(level);
//            logFile << levelStr << ": " << message << std::endl;
//        }
//    }

//	static void log(LogLevel level, const std::string& message) {
//	    if (level >= logLevel) {
//	        std::lock_guard<std::mutex> guard(logMutex);
//	        std::string levelStr = logLevelToString(level);
//	        std::string timeStr = getFormattedTime(); // Получаем отформатированное время
//	        logFile << timeStr << " " << levelStr << ": " << message << std::endl;
//	    }
//	}

//	static	void log(LogLevel level, const std::string& message) {
//        if (level >= logLevel) {
//            std::lock_guard<std::mutex> guard(logMutex);
//            std::string levelStr = logLevelToString(level);
//            std::string timeStr = getFormattedTime(); // Получаем отформатированное время
//            logFile << timeStr << " " << levelStr << ": " << message << std::endl;
//            
//            if (consoleOutputEnabled) {
//                std::cout << timeStr << " " << levelStr << ": " << message << std::endl;
//            }
//        }
//    }


	static void log(LogLevel level, const char* fmt, ...) {
	    if (level >= logLevel) {
	        std::lock_guard<std::mutex> guard(logMutex);
	
	        // Форматируем сообщение
	        char buffer[4048];
	        va_list args;
	        va_start(args, fmt);
	        std::vsnprintf(buffer, sizeof(buffer), fmt, args);
	        va_end(args);
	        std::string message = buffer;
	
	        std::string levelStr = logLevelToString(level);
	        std::string timeStr = getFormattedTime(); // Получаем отформатированное время
	        logFile << timeStr << " " << levelStr << ": " << message << std::endl;
	
	        if (consoleOutputEnabled) {
	            std::cout << timeStr << " " << levelStr << ": " << message << std::endl;
	        }
	    }
	}


    static bool initialize(const std::string& filename) {
        if (logFile.is_open()) {
            return false; 
        }
        logFile.open(filename, std::ios::out | std::ios::app);
        return logFile.is_open();
    }
    
        // Метод для получения и копирования форматированной даты в массив
    static void copyFormattedDateToArray(char* array, size_t arraySize) {
        std::string formattedDate = getFormattedDate();
        std::strncpy(array, formattedDate.c_str(), arraySize - 1);
        array[arraySize - 1] = '\0'; // Обеспечиваем завершение строки нулевым символом
    }
    
	// Функция для получения текущей даты в формате dd.mm.yy
	static std::string getFormattedDate() {
	    // Получаем текущее время в формате Unix-времени
	    std::time_t rawtime = std::time(nullptr);
	
	    // Преобразуем Unix-время в структуру tm
	    std::tm* timeinfo = std::localtime(&rawtime);
	
	    // Форматируем время в строку в формате dd.mm.yy
	    char formattedDate[20];
	    std::strftime(formattedDate, sizeof(formattedDate), "%d.%m.%y", timeinfo);
	
	    return std::string(formattedDate);
	}	
	
	// Функция для получения текущего времени в формате HH:MM:SS
	static std::string getFormattedTime() {
		// Получаем текущее время в формате Unix-времени
		std::time_t rawtime = std::time(nullptr);
		
		// Преобразуем Unix-время в структуру tm
		std::tm* timeinfo = std::localtime(&rawtime);
		
		// Форматируем время в строку в формате HH:MM:SS
		char formattedTime[20];
		std::strftime(formattedTime, sizeof(formattedTime), "%H:%M:%S", timeinfo);
		
		return std::string(formattedTime);
	}

	static void ConsoleOutOn() { 
		consoleOutputEnabled = true;	
	}
	
	static void ConsoleOutOff() {
		consoleOutputEnabled = false;
	}
	

private:
    static std::ofstream logFile;
    static LogLevel logLevel;
    static std::mutex logMutex;
    static bool consoleOutputEnabled;
   
    static std::string logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::log_DEBUG:   return "  DEBUG";
            case LogLevel::log_INFO:    return "   INFO";
            case LogLevel::log_WARNING: return "WARNING";
            case LogLevel::log_ERROR:   return "  ERROR";
            default:                    return "UNKNOWN";
        }
    }
};

#endif // LOGGER_H
