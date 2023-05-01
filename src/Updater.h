#ifndef UPDATER_H_
#define UPDATER_H_

#include <iostream>
#include <string>
#include <network_ipc.h>
#include <fstream>
#include <mutex>

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

    private:
        Updater();
        ~Updater();

        static std::mutex m_stateMutex;

        static UpdateState m_updateState;

        static Updater *s_Instance;

        static int readData(char **pbuf, int *size);
        static int getStatus(ipc_message *msg);
        static int endUpdate(RECOVERY_STATUS status);

        static pthread_mutex_t mymutex;
        static pthread_cond_t cv_end;

        static std::ifstream m_updateFile;
        struct swupdate_request m_updateRequest;
};

#endif /* UPDATER_H_*/