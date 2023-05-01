#include "UpdateProgress.h"
#include <progress_ipc.h>
#include <iostream>


UpdateProgress::UpdateProgress()
{
    m_SWprogressFd = -1;
    m_SWprogressFd = progress_ipc_connect(1); // Try what this does!

}

UpdateProgress::~UpdateProgress()
{

}

void UpdateProgress::getProgress()
{
    struct progress_msg msg;
    progress_ipc_receive(&m_SWprogressFd, &msg); // This function blocks
    std::cout << msg.cur_image << " " << msg.cur_percent << "%" << std::endl;
}