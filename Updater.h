#ifndef UPDATER_H_
#define UPDATER_H_

#include <iostream>
#include <string>
#include <network_ipc.h>
#include <fstream>

class Updater
{
    public:
        Updater();
        ~Updater();


        void startUpdate(std::string updateFile);

    private:

        static int readData(char **pbuf, int *size);
        static int getStatus(ipc_message *msg);
        static int endUpdate(RECOVERY_STATUS status);

        std::ifstream m_updateFile;
        struct swupdate_request m_updateRequest;
};


#endif /* UPDATER_H_*/