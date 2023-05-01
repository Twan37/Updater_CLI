#include <iostream>
#include "Updater.h"
#include "GrubEnv.h"
#include "UbootEnv.h"
#include "BootloaderEnv.h"


int main(void)
{
    //Updater::GetInstance()->startUpdate("empty_test.swu");
    BootloaderEnv *myenv;
    myenv = new GrubEnv();
    myenv->printEnv();
    return 0;
}