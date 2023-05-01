#include "Updater.h"
#include <network_ipc.h>
#include <fstream>
#include <functional>


#define VERBOSE 1

Updater* Updater::s_Instance{nullptr};

pthread_mutex_t Updater::mymutex;
pthread_cond_t Updater::cv_end = PTHREAD_COND_INITIALIZER;

std::ifstream Updater::m_updateFile;

Updater::UpdateState Updater::m_updateState = READY;
std::mutex Updater::m_stateMutex;


int end_status = EXIT_SUCCESS;



Updater::Updater()
{

}

Updater::~Updater()
{
	if (s_Instance != NULL)
	{
		delete s_Instance;
	}
}


Updater *Updater::GetInstance()
{
	if (s_Instance == nullptr)
	{
		s_Instance = new Updater;
	}
	return s_Instance;
}


void Updater::startUpdate(std::string updateFile)
{
    int startStatus;

	m_stateMutex.lock();
	if (m_updateState == UPDATING)
	{
		std::cout << "UPDATE STILL IN PROGRESS" << std::endl;
		m_stateMutex.unlock();
		return;
	}
	else if (m_updateState == READY)
	{
		std::cout << "READY TO UPDATE" << std::endl;
		m_updateState = UPDATING;
	}
	m_stateMutex.unlock();


    m_updateFile.open(updateFile, std::ifstream::in);
    
    if (!m_updateFile)
	{
        std::cout << "Error opening file" << std::endl;
		m_stateMutex.lock();
		m_updateState = READY;
		m_stateMutex.unlock();
		return;
	}
	
    swupdate_prepare_req(&m_updateRequest); // This fills the request structure with default values
	startStatus = swupdate_async_start(readData, getStatus, endUpdate, &m_updateRequest, sizeof(m_updateRequest));

    if (startStatus < 0)
	{
        std::cout << "And error occured while trying to start the update" << std::endl;
		
		m_stateMutex.lock();
		m_updateState = READY;
		m_stateMutex.unlock();

        m_updateFile.close();
		return;
	}

	std::cout << "Update started!" << std::endl;
}


/* 
 * This function is called to read a new chunk of the image
 */
int Updater::readData(char **pbuf, int *size)
{
    static char buf[256];
    m_updateFile.read(buf, sizeof(buf));
	*pbuf = buf;
	*size = m_updateFile.gcount();
	return *size;
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
	// Close the file after updating
	m_stateMutex.lock();
	m_updateState = READY;
	m_updateFile.close();
	m_stateMutex.unlock();
	return 0;
}
