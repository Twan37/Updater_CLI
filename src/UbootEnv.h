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
        std::string getVal(const std::string& keyName) override;
        void setVal(const std::string& keyName, const std::string& valStr) override;
        void setVal(const std::string& keyName, int valInt) override;
        void saveEnv() override;
        void reloadEnv() override;

    private:
        void initEnv();
        struct uboot_ctx *m_UbootCTX = NULL;
};


#endif /* UBOOTENV_H_*/