#ifndef SESSION_H
#define SESSION_H

#include <mutex>

class Session {
private:
    std::mutex tasksMutex;
    char cUser[SERVER_CONN_ATTR_TEXT_MAX_SIZE];
    char cPass[SERVER_CONN_ATTR_TEXT_MAX_SIZE];
    char cHost[SERVER_CONN_ATTR_TEXT_MAX_SIZE];
    unsigned short usPort;
    unsigned int uiAddress;

public:
//    void setUser(const std::string& user, const std::string& pass) {
//        std::lock_guard<std::mutex> lock(tasksMutex);
//        strncpy(cUser, user.c_str(), SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1);
//        cUser[SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1] = '\0';
//        std::strncpy(cPass, pass.c_str(), SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1);
//        cPass[SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1] = '\0';
//    }

    void setUser(const std::string& user, const std::string& pass) {
        std::lock_guard<std::mutex> lock(tasksMutex);

        // Копируем строку user в массив cUser с ограничением по размеру
        size_t userLength = min(user.size(), static_cast<size_t>(SERVER_CONN_ATTR_TEXT_MAX_SIZE));
        std::copy(user.c_str(), user.c_str() + userLength, cUser);
        cUser[userLength] = '\0';  // Ставим \0 сразу после окончания строки

        // Копируем строку pass в массив cPass с ограничением по размеру
        size_t passLength = min(pass.size(), static_cast<size_t>(SERVER_CONN_ATTR_TEXT_MAX_SIZE));
        std::copy(pass.c_str(), pass.c_str() + passLength, cPass);
        cPass[passLength] = '\0';  // Ставим \0 сразу после окончания строки
    }

    void setSocket(const std::string& host, unsigned short port, unsigned int address) {
        std::lock_guard<std::mutex> lock(tasksMutex);
        std::strncpy(cHost, host.c_str(), SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1);
        cHost[SERVER_CONN_ATTR_TEXT_MAX_SIZE - 1] = '\0';
        usPort = port;
        uiAddress = address;
    }

    std::string getUser() {
        std::lock_guard<std::mutex> lock(tasksMutex);
        // Находим первый нулевой символ в cUser
        size_t length = strlen(cUser);  // Вычисляем длину строки без учета нулевого символа
        return std::string(cUser, length);  // Возвращаем строку, обрезанную до первого \0
    }

    std::string getPass() {
        std::lock_guard<std::mutex> lock(tasksMutex);
        // Находим первый нулевой символ в cUser
        size_t length = strlen(cPass);  // Вычисляем длину строки без учета нулевого символа
        return std::string(cPass, length);  // Возвращаем строку, обрезанную до первого \0
    }

    unsigned short getPort() {
        std::lock_guard<std::mutex> lock(tasksMutex);
        return usPort;
    }

    unsigned int getAddress() {
        std::lock_guard<std::mutex> lock(tasksMutex);
        return uiAddress;
    }
    
    std::string getHost() {
        std::lock_guard<std::mutex> lock(tasksMutex);
        return std::string(cHost);
    }
};


#endif
