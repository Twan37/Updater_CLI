#ifndef BOOTLOADERENV_H_
#define BOOTLOADERENV_H_

#include <iostream>
#include <string>
#include <map>
#include <string>

class BootloaderEnv
{
    public:
        virtual void printEnv() = 0;
        virtual std::string getVal(std::string keyName) = 0;
        virtual void setVal(std::string keyName, std::string valStr) = 0;
        virtual void setVal(std::string keyName, int valInt) = 0;  
};


#endif /* BOOTLOADERENV_H_*/