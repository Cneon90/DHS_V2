#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <vector>
#include <cstdint>
#include <cstring>

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define MAX_ACCOUNTS_COUNT                 256
#define BUF_SIZE 						   256


struct account_item {
    char* pcLogin    = nullptr;
    char* pcPassword = nullptr;
    char* pcRights   = nullptr;
};

class Accounts
{
	private:
		std::vector<account_item> AccountList;		
	public:
		~Accounts() {
	        for (auto& account : AccountList) {
	            // Ensure that destructor is called for each account_item
	            account.~account_item();
	        }
	    }	
		bool ReadFileAccount(char* _filename); 	
		void AddAccount(const char* login, const char* password, const char* rights);
		bool SaveAccount(const char* _filename, const char* login, const char* password, const char* rights);	
	    void RemoveAccountByIndex(uint16_t index);
	    account_item GetAccountByIndex(size_t index);
	    account_item FindAccountByLogin(const char* login);
	    int counts() const { return static_cast<int>(AccountList.size()); }
};

#endif
