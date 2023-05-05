#ifndef UPDATER_H_
#define UPDATER_H_

#include <iostream>
#include <string>
#include <network_ipc.h>
#include <fstream>
#include <mutex>
#include "BootloaderEnv.h"

class Updater
{
    public:
        enum UpdateState 
        {
            READY,
            UPDATING
        };

        // Singletons are not clonable and assignable
        Updater(const Updater&) = delete;
        void operator=(const Updater &) = delete;

        void startUpdate(std::string updateFile);
        static Updater *GetInstance();
        static std::string getBootedRootfs();
        static std::string getUpdateTarget();
        void setUpdatePartSymlink(const std::string& updateRootfsPart);
        std::string readBootloader() {return s_bootloaderEnv->getVal("ustate");}

    private:
        Updater();
        ~Updater();

        int selftest();
        static BootloaderEnv *s_bootloaderEnv;
        static std::mutex s_stateMutex;

        static UpdateState s_updateState;

        static Updater *s_Instance;

        static int readData(char **pbuf, int *size);
        static int getStatus(ipc_message *msg);
        static int endUpdate(RECOVERY_STATUS status);

        static pthread_mutex_t mymutex;
        static pthread_cond_t cv_end;

        static std::ifstream s_updateFile;
        struct swupdate_request s_updateRequest;
};

#endif /* UPDATER_H_*/