#include <iostream>
#include "Updater.h"
#include "GrubEnv.h"
#include "UbootEnv.h"
#include "BootloaderEnv.h"
#include "UpdateProgress.h"


int main(void)
{
    Updater::GetInstance()->startUpdate("/home/twan/Documents/empty_test.swu");
    UpdateProgress mypr;
    while(1)
    {
        mypr.getProgress();
    }
    return 0;
}