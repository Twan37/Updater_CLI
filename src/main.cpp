#include <iostream>
#include "Updater.h"
#include "GrubEnv.h"
#include "UbootEnv.h"
#include "BootloaderEnv.h"
#include "UpdateProgress.h"
#include <unistd.h>


int main(void)
{
    Updater::GetInstance()->startUpdate("/home/twan/Documents/empty_test.swu");
    //std::cout << Updater::GetInstance()->getBootedRootfs() << std::endl;
    //std::cout << Updater::GetInstance()->getUpdateTarget() << std::endl;
    //BootloaderEnv *myEnv;
    //myEnv = new GrubEnv;
    //std::cout << myEnv->getVal("ROOTFSPART") << std::endl;

    while(1)
    {
        //std::cout << "ustate="<< Updater::GetInstance()->readBootloader() << std::endl;
        sleep(1);
    }

    //Updater::GetInstance()->setUpdatePartSymlink("/dev/sda1");
}