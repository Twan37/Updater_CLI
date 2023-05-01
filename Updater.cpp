#include "Updater.h"
#include <network_ipc.h>
#include <fstream>
#include <functional>

#define VERBOSE 1

int end_status = EXIT_SUCCESS;

void Updater::startUpdate(std::string updateFile)
{
    int startStatus;
    m_updateFile.open(updateFile);
    
    if (!m_updateFile)
        std::cout << "Error opening file" << std::endl;

    swupdate_prepare_req(&m_updateRequest); // This fills the request structure with default values
	startStatus = swupdate_async_start(
		std::bind(&Updater::readData, this, std::placeholders::_1, std::placeholders::_2),
		std::bind(&Updater::getStatus, this, std::placeholders::_1),
		std::bind(&Updater::endUpdate, this, std::placeholders::_1),
		&m_updateRequest, sizeof(m_updateRequest));


    if (startStatus < 0)
        std::cout << "And error occured while trying to start the update" << std::endl;
        m_updateFile.close();
}


/* 
 * This function is called to read a new chunk of the image
 */
int Updater::readData(char **pbuf, int *size)
{
    static char buf[256];
    m_updateFile.read(buf, sizeof(buf));
	*pbuf = buf;
	return m_updateFile.gcount();
}

/*
 * This is called by the library to inform
 * about the current status of the update
 */
int Updater::getStatus(ipc_message *msg)
{
	if (VERBOSE)
		std::cout << "Status: " << msg->data.status.current << 
            "message: " << (strlen(msg->data.status.desc) > 0 ? msg->data.status.desc : "") << std::endl;

	return 0;
}


/*
 * This function is called at the end of the update
 */
int Updater::endUpdate(RECOVERY_STATUS status)
{
    std::cout << "updated" << std::endl;
	//end_status = (status == SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
    /*
	fprintf(stdout, "SWUpdate %s\n",
		status == FAILURE ? "*failed* !" :
			"was successful !");

	if (status == SUCCESS && run_postupdate) 
    {
		fprintf(stdout, "Executing post-update actions.\n");
		ipc_message msg;
		msg.data.procmsg.len = 0;
		if (ipc_postupdate(&msg) != 0 || msg.type != ACK) 
        {
			fprintf(stderr, "Running post-update failed!\n");
			end_status = EXIT_FAILURE;
		}
	}
    */
	//pthread_mutex_lock(&mymutex);
	//pthread_cond_signal(&cv_end);
	//pthread_mutex_unlock(&mymutex);

	return 0;
}
