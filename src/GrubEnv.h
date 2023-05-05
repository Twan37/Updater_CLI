#ifndef GRUBENV_H_
#define GRUBENV_H_

#include <iostream>
#include <string>
#include "BootloaderEnv.h"
#include <map>
#include <string>

class GrubEnv: public BootloaderEnv
{
    public:
        GrubEnv();
        ~GrubEnv();

        void printEnv();
        std::string getVal(const std::string& keyName) override;
        void setVal(const std::string& keyName, const std::string& valStr) override;
        void setVal(const std::string& keyName, int valInt) override;
        void saveEnv() override;
        void reloadEnv() override;

    private:
        void readEnv();
        std::map<std::string, std::string> m_envVars;
};


#endif /* GRUBENV_H_*/