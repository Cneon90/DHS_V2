#include "Accounts.h"

void Accounts::AddAccount(const char* login, const char* password, const char* rights)
    {
        account_item newAccount;

        size_t loginLen = strlen(login) + 1;
        newAccount.pcLogin = new char[loginLen];
        strncpy(newAccount.pcLogin, login, loginLen);

        size_t passwordLen = strlen(password) + 1;
        newAccount.pcPassword = new char[passwordLen];
        strncpy(newAccount.pcPassword, password, passwordLen);

        size_t rightsLen = strlen(rights) + 1;
        newAccount.pcRights = new char[rightsLen];
        strncpy(newAccount.pcRights, rights, rightsLen);

        AccountList.push_back(newAccount);
    }	
    
 
    void Accounts::RemoveAccountByIndex(uint16_t index)
    {
        if (index < AccountList.size())
        {
            AccountList.erase(AccountList.begin() + index);
        }
    }
    
    
    account_item Accounts::GetAccountByIndex(size_t index)
    {
        if (index < AccountList.size())
        {
            return AccountList[index];
        }
        else
        {
            account_item emptyAccount;
            emptyAccount.pcLogin = NULL;
            emptyAccount.pcPassword = NULL;
			emptyAccount.pcRights = NULL;
            return emptyAccount;  
        }
    }
    
    
    account_item Accounts::FindAccountByLogin(const char* login)
    {
        for (const auto& account : AccountList)
        {
            if (strcmp(account.pcLogin, login) == 0)
            {
                return account; // Return found account
            }
        }

        // If not found, return an empty account
        account_item emptyAccount;
        emptyAccount.pcLogin = NULL;
        emptyAccount.pcPassword = NULL;
        emptyAccount.pcRights = NULL;
        return emptyAccount;  
    }
    
    
    
bool isValid(const account_item& item) {
    return item.pcLogin != nullptr && item.pcPassword != nullptr && item.pcRights != nullptr;
}



bool Accounts::ReadFileAccount(char* _filename) 
{ 
FILE *pxFile = fopen(_filename, "rb");
if (pxFile == nullptr) {
    std::cerr << "Failed to open file for reading, trying to create it..." << std::endl;
    pxFile = fopen(_filename, "wb");
    if (pxFile == nullptr) {
        std::cerr << "Can't create accounts list template file!" << std::endl;
        return false;
    }
    fprintf(pxFile, "User\nPassword\n");
    fclose(pxFile);
    pxFile = fopen(_filename, "rb");
    if (pxFile == nullptr) {
        std::cerr << "Failed to open file after creation attempt." << std::endl;
        return false;
    }
}

char cBuf[BUF_SIZE];
account_item tempItem;
int recordCount = 0;

while (fgets(cBuf, BUF_SIZE - 1, pxFile) != nullptr) {
    size_t n = strlen(cBuf);
    if (n > 0 && (cBuf[n - 1] == '\n' || cBuf[n - 1] == '\r')) {
        cBuf[n - 1] = '\0';  // Удаляем символ новой строки
    }

    char* pcPointer = new char[n + 1];
    if (pcPointer == nullptr) {
        std::cerr << "Memory allocation failed." << std::endl;
        fclose(pxFile);
        return false;
    }
    strcpy(pcPointer, cBuf);

    if (recordCount % 3 == 0) {
        tempItem.pcLogin = pcPointer;
    } else if (recordCount % 3 == 1) {
        tempItem.pcPassword = pcPointer;
    } else {
        tempItem.pcRights = pcPointer;
        if (isValid(tempItem)) {
            AccountList.push_back(tempItem);
            //std::cout << "Added account. Total accounts: " << AccountList.size() << std::endl;
        } else {
            // Освобождаем память, если запись невалидная
            delete[] tempItem.pcLogin;
            delete[] tempItem.pcPassword;
            delete[] tempItem.pcRights;
        }
        tempItem = account_item(); // Сбрасываем временный объект для следующего использования
    }

    recordCount++;
}

if (ferror(pxFile)) {
    std::cerr << "Error reading file." << std::endl;
    fclose(pxFile);
    return false;
}

fclose(pxFile);
return true;
}


bool Accounts::SaveAccount(const char* _filename, const char* login, const char* password, const char* rights) {
    // Добавление в вектор
    account_item newItem;
    newItem.pcLogin = new char[strlen(login) + 1];
    newItem.pcPassword = new char[strlen(password) + 1];
    newItem.pcRights = new char[strlen(rights) + 1];
    strcpy(newItem.pcLogin, login);
    strcpy(newItem.pcPassword, password);
    strcpy(newItem.pcRights, rights);
    
    AccountList.push_back(newItem);

    // Открытие файла для добавления
    FILE* pxFile = fopen(_filename, "ab");
    if (pxFile == nullptr) {
        std::cerr << "Cannot open file for appending!" << std::endl;
        return false;
    }

    fprintf(pxFile, "%s\n%s\n%s\n", login, password, rights);
    fclose(pxFile);

    return true;
}

