#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <iostream>
#include <cstring>
#include <unistd.h> 


#if _WIN32
	#include "port_windows.h"	
#elif __linux__
	#include "port_linux.h"	
#else 
	#error "UNKNOWN TARGET OS"
#endif


#include <map>

class ArgParser {
public:
    ArgParser(int argc, char* argv[]) 
        : m_argc(argc), m_argv(argv) {
        parseArguments();
    }

    const char* getAppRootDir() const {
        return m_appRootDir;
    }

    bool isDaemonMode() const {
        return m_daemonMode;
    }

    int getPort() const {
        return m_port;
    }

    const std::map<std::string, std::string>& getKeyValuePairs() const {
        return m_keyValuePairs;
    }

    void runAsDaemon() const {
        #if __linux__
        if (m_daemonMode) {
            pid_t pID = fork();
            if (pID < 0) {
                std::cerr << "Can't fork child process for Daemon mode" << std::endl;
                exit(1);
            }
            if (pID > 0) {
                std::cout << "Daemon process (PID = " << pID << ") started successfully!" << std::endl;
                exit(0); // ��������� ������������ �������
            }
            // � �������� �������� ����� ���������� ���������� ���� ������
        }
        #endif
    }

private:
    int m_argc;
    char** m_argv;
    char m_appRootDir[256]; // ������ ����� ���������
    bool m_daemonMode = false;
    int m_port = 0; // �� ��������� 0, ���� �� ������
    std::map<std::string, std::string> m_keyValuePairs;

    void parseArguments() {
        // �������� ������� ���������� (������ ��������)
//        if (m_argc >= 1) {
//            strcpy(m_appRootDir, m_argv[0]);
//            size_t len = strlen(m_appRootDir);
//            while (len && m_appRootDir[len - 1] != PORT_CATALOG_DEVIDER_SYMBOL) { // PORT_CATALOG_DEVIDER_SYMBOL �������� �� '/'
//                len--;
//            }
//            if (len) m_appRootDir[len - 1] = '\0'; // ������� ��������� ������
//        }

        // ������������ ��������� ��������� ������
        for (int i = 0; i < m_argc; ++i) {
            if (strcmp(m_argv[i], "-d") == 0) {
                m_daemonMode = true;
            } else if (strncmp(m_argv[i], "--port=", 7) == 0) {
                m_port = atoi(m_argv[i] + 7); // ��������� ����� �����
            } else if (strncmp(m_argv[i], "--", 2) == 0) {
                // ����-��������
                std::string key_value(m_argv[i] + 2); // ���������� "--"
                size_t pos = key_value.find('=');
                if (pos != std::string::npos) {
                    std::string key = key_value.substr(0, pos);
                    std::string value = key_value.substr(pos + 1);
                    m_keyValuePairs[key] = value;
                }
            }
        }
    }
};



#endif
