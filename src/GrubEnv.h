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
        std::string getVal(std::string keyName) override;
        void setVal(std::string keyName, std::string valStr) override;
        void setVal(std::string keyName, int valInt) override;

    private:
        void readEnv();
        void writeEnv();

        std::map<std::string, std::string> m_envVars;
};


#endif /* GRUBENV_H_*/