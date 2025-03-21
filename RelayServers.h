#ifndef RELAYSERVERS_H
#define RELAYSERVERS_H

//#include <pthread.h>
#include <thread>
#include <mutex>

#define ATTR_MAX_SIZE   					256		 
#define ERROR_OPEN_FILE_READ				"The file could not be opened for reading."
#define ERROR_OPEN_FILE_WRITE				"The file could not be opened for writing."

class TRelayServer {
private:
    int id;
    char name[50];
    char cUser[ATTR_MAX_SIZE];
    char cPass[ATTR_MAX_SIZE];
    char cHost[ATTR_MAX_SIZE];
    unsigned short usPort;
    unsigned int uiAddress;
    bool isActive;
public:
    // Конструктор
    TRelayServer() : id(0), usPort(0), uiAddress(0), isActive(true) {
        cUser[0] = '\0';
        cPass[0] = '\0';
        cHost[0] = '\0';
    }
    
    // Method to initialize the server
    void initialize(int id, const std::string& user, const std::string& pass,
                    const std::string& host, unsigned short port, unsigned int address, bool active = true) {
        this->id = id;
        usPort = port;
        uiAddress = address;
        isActive = active;

        strncpy(cUser, user.c_str(), sizeof(cUser) - 1);
        cUser[sizeof(cUser) - 1] = '\0';

        strncpy(cPass, pass.c_str(), sizeof(cPass) - 1);
        cPass[sizeof(cPass) - 1] = '\0';

        strncpy(cHost, host.c_str(), sizeof(cHost) - 1);
        cHost[sizeof(cHost) - 1] = '\0';
    }
        
    
    // Получение ID
    int getId() const { return id; }

    // Получение пользователя
    const char* getUser() const { return cUser; }

    // Получение пароля
    const char* getPass() const { return cPass; }

    // Получение хоста
    const char* getHost() const { return cHost; }

    // Получение порта
    unsigned short getPort() const { return usPort; }

    // Получение адреса
    unsigned int getAddress() const { return uiAddress; }

    // Получение имени
    const char* getName() const { return name; }

    // Запись в файл
    void writeToFile(std::ofstream& ofs) const {
        ofs.write(reinterpret_cast<const char*>(this), sizeof(*this));
    }
    
     //
    bool getIsActive() const { return isActive; }

    // 
    void setIsActive(bool active) { isActive = active; }

    // File read
    static TRelayServer readFromFile(std::ifstream& ifs) {
        TRelayServer obj; // Temp object
        obj.initialize(0, "", "", "", 0, 0);
        ifs.read(reinterpret_cast<char*>(&obj), sizeof(obj));
        return obj;
    }
};



class RSHandler {
public:
   static void writeToFile(const std::string& filename, const std::vector<TRelayServer>& data) {
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) {
            std::cerr << ERROR_OPEN_FILE_WRITE << std::endl;
            return;
        }

        for (const auto& entry : data) {
            entry.writeToFile(ofs);
        }

        ofs.close();
    }
    
    static bool recordExists(const std::string& filename, const TRelayServer& record) {
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) {
            std::cerr << "Error opening file for reading." << std::endl;
            return false; // File doesn't exist or can't be opened
        }

        TRelayServer temp;
        while (ifs.read(reinterpret_cast<char*>(&temp), sizeof(temp))) {
        	if (temp.getId() == record.getId() &&
                strcmp(temp.getUser(), record.getUser()) == 0 &&
                strcmp(temp.getPass(), record.getPass()) == 0 &&
                strcmp(temp.getHost(), record.getHost()) == 0 &&
                temp.getPort() == record.getPort() &&
                temp.getAddress() == record.getAddress() &&
                strcmp(temp.getName(), record.getName()) == 0) {						
//            if (temp.getId() == record.getId()) { // Check if ID matches
                ifs.close();
                return true; // Record already exists
            }
        }

        ifs.close();
        return false; // Record does not exist
    }

    static void addRecord(const std::string& filename, const TRelayServer& record) {
        if (recordExists(filename, record)) {
            std::cerr << "Record with ID " << record.getId() << " already exists." << std::endl;
            return; // Record already exists, do not add
        }

        std::ofstream ofs(filename, std::ios::binary | std::ios::app); // Append to the file
        if (!ofs) {
            std::cerr << ERROR_OPEN_FILE_WRITE << std::endl;
            return;
        }
        record.writeToFile(ofs); // Write the new record
        ofs.close();
    }

    static void readFromFile(const std::string& filename) {
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) {
            std::cerr << ERROR_OPEN_FILE_READ << std::endl;
            return;
        }

 		while (ifs.peek() != EOF) {
            TRelayServer data = TRelayServer::readFromFile(ifs);
            std::cout << "ID: " << data.getId() << "\n";
            std::cout << "User: " << data.getUser() << "\n";
            std::cout << "Password: " << data.getPass() << "\n";
            std::cout << "Host: " << data.getHost() << "\n";
            std::cout << "Port: " << data.getPort() << "\n";
            std::cout << "Address: " << data.getAddress() << "\n";
            std::cout << "Active: " << (data.getIsActive() ? "Yes" : "No") << "\n\n";
        }

        ifs.close();
    }
    
    
  

    static std::vector<TRelayServer> readAllFromFile(const std::string& filename) {
        std::vector<TRelayServer> records;
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) {
            std::cerr << ERROR_OPEN_FILE_READ << std::endl;
            
			// Create the file if it does not exist
			std::ofstream ofs(filename, std::ios::binary);
			if (!ofs) {
			std::cerr << "Error: Could not create file." << std::endl;
			return records; 
			}
			// Close the output file stream after creating the file
			ofs.close();
            
            return records; 
        }

        while (ifs.peek() != EOF) {
            TRelayServer data = TRelayServer::readFromFile(ifs);
            records.push_back(data);
        }

        ifs.close();
        return records; 
    }
    

    static void changeActivity(const std::string& filename, int id, bool active) {
	    std::vector<TRelayServer> records = RSHandler::readAllFromFile(filename);
	    for (const auto& record : records) {
	        std::cout << "ID: " << record.getId() << "\n";
	        std::cout << "User: " << record.getUser() << "\n"; 
	        std::cout << "Password: " << record.getPass() << "\n"; 
	        std::cout << "Host: " << record.getHost() << "\n"; 
	        std::cout << "Port: " << record.getPort() << "\n"; 
	        std::cout << "Address: " << record.getAddress() << "\n"; 
	        std::cout << "Active: " << (record.getIsActive() ? "Yes" : "No") << "\n\n";
	    }    
	}
	
	
	static bool deleteFile(const std::string& filename) {
		if (std::remove(filename.c_str()) != 0) {
	        std::cerr << "Error: Could not delete the file." << std::endl;
	        return false; // Ошибка при удалении файла
	    }
	    return true; // Файл успешно удален
	}

};

#endif
