#ifndef UBOOTENV_H_
#define UBOOTENV_H_

#include <iostream>
#include <string>
#include "BootloaderEnv.h"
#include <map>
#include <string>

class UbootEnv: public BootloaderEnv
{
    public:
        UbootEnv();
        ~UbootEnv();

        void printEnv();
        std::string getVal(std::string keyName) override;
        void setVal(std::string keyName, std::string valStr) override;
        void setVal(std::string keyName, int valInt) override;

    private:
        struct uboot_ctx *m_UbootCTX = NULL;
        void initEnv();
        
};


#endif /* UBOOTENV_H_*/