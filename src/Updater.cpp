#include "Updater.h"
#include <network_ipc.h>
#include <fstream>
#include <functional>
#include <unistd.h>
#include <sys/stat.h>

#define NUC

#ifdef NUC
#include "GrubEnv.h"
#endif

#ifdef IPC
#include "UbootEnv.h"
#endif

#define CMDLINE_DEFAULT_PATH		"/proc/cmdline"
#define UPDATEPART_SYMLINK_PATH		"/dev/updatePart"
#define MAX_SYMLINK_PATH_LEN		50


#define VERBOSE 1

Updater* Updater::s_Instance{nullptr};

pthread_mutex_t Updater::mymutex;
pthread_cond_t Updater::cv_end = PTHREAD_COND_INITIALIZER;

std::ifstream Updater::s_updateFile;

Updater::UpdateState Updater::s_updateState = READY;
std::mutex Updater::s_stateMutex;

BootloaderEnv *Updater::s_bootloaderEnv;


int end_status = EXIT_SUCCESS;



Updater::Updater()
{
	int ustate;

	// Create a new instance of bootloaderEnv based on the system it's running on.
	#ifdef NUC
	s_bootloaderEnv = new GrubEnv;
	#endif

	#ifdef IPC
	s_bootloaderEnv = new UbootEnv;
	#endif

	ustate = std::stoi(s_bootloaderEnv->getVal("ustate"));

	if (ustate == 1)
	{
		// First boot after updating
		
		// Do a selftest if passed then...
		if (selftest())
		{
			//std::cout << "Passed Selftest" << std::endl;
			//s_bootloaderEnv->setVal("ustate", 0);
			//s_bootloaderEnv->saveEnv();
			// Set the update symbolic link
			//setUpdatePartSymlink(//get the correct rootfs)
		}
		else
		{
			std::cout << "Failed Selftest!" << std::endl;
			// Perform a reboot or something
		}
	}
	else if (ustate == 3)
	{
		std::cout << "Rolled back the previous update. Consider updating again" << std::endl;
	}
	else 
	{
		std::cout << "Not first boot" << std::endl;
	}

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

	s_stateMutex.lock();
	if (s_updateState == UPDATING)
	{
		std::cout << "UPDATE STILL IN PROGRESS" << std::endl;
		s_stateMutex.unlock();
		return;
	}
	else if (s_updateState == READY)
	{
		std::cout << "READY TO UPDATE" << std::endl;
		s_updateState = UPDATING;
	}
	s_stateMutex.unlock();


    s_updateFile.open(updateFile, std::ifstream::in);
    
    if (!s_updateFile)
	{
        std::cout << "Error opening file" << std::endl;
		s_stateMutex.lock();
		s_updateState = READY;
		s_stateMutex.unlock();
		return;
	}
	
    swupdate_prepare_req(&s_updateRequest); // This fills the request structure with default values
	startStatus = swupdate_async_start(readData, getStatus, endUpdate, &s_updateRequest, sizeof(s_updateRequest));

    if (startStatus < 0)
	{
        std::cout << "And error occured while trying to start the update" << std::endl;
		
		s_stateMutex.lock();
		s_updateState = READY;
		s_stateMutex.unlock();

        s_updateFile.close();
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
    s_updateFile.read(buf, sizeof(buf));
	*pbuf = buf;
	*size = static_cast<int>(s_updateFile.gcount());
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
	if (status == SUCCESS)
	{
		// Set USTATE AS WELL Because it's set by SWupdate before this and thus not atomic
		// Update env vars

		//s_bootloaderEnv->setVal("ROOTFSPART", getUpdateTarget());
		//s_bootloaderEnv->setVal("FALLBACKPART", getBootedRootfs());
		//s_bootloaderEnv->saveEnv();
		//std::cout << "ustate=" << s_bootloaderEnv->getVal("ustate") << std::endl;
		//std::cout << "Edited the bootloader env vars" << std::endl;
		
		// SWupdate can set env vars. Reload to prevent losing those edits.
		s_bootloaderEnv->reloadEnv();
		std::string a = s_bootloaderEnv->getVal("ustate");
		std::cout << "Ustate=" << a << std::endl;
	}

	// Close the file after updating
	s_stateMutex.lock();
	s_updateState = READY;
	s_updateFile.close();
	s_stateMutex.unlock();
	return 0;
}

std::string Updater::getBootedRootfs()
{
	std::ifstream cmdline_file(CMDLINE_DEFAULT_PATH);
    std::string cmdline;

    // Read the contents of the cmdline file into a string
    std::getline(cmdline_file, cmdline);

    // Search for the line that contains the "root=" argument
    std::size_t root_pos = cmdline.find("root=");
    if (root_pos != std::string::npos) 
	{
        // Extract the value of the "root=" argument
        std::string root_arg = cmdline.substr(root_pos + 5);
        std::size_t space_pos = root_arg.find(" ");
        std::string root_value = root_arg.substr(0, space_pos);

        return root_value;//.back();
    }
	return "";
}

std::string Updater::getUpdateTarget() 
{
    char targetPath[MAX_SYMLINK_PATH_LEN];
    ssize_t len = readlink(UPDATEPART_SYMLINK_PATH, targetPath, sizeof(targetPath));
    if (len == -1) {
        // handle error
        return "";
    }
    targetPath[len] = '\0';	// readlink doesn't add the terminating null byte
    return std::string(targetPath);
}

void Updater::setUpdatePartSymlink(const std::string& updateRootfsPart)
{
	struct stat info;
	// Check if the symlink exists, remove it if it does
	if (lstat(UPDATEPART_SYMLINK_PATH, &info) == 0)
    {
        if (unlink(UPDATEPART_SYMLINK_PATH) == -1) {
            // handle error
            std::cout << "Can't unlink symlink" << std::endl;
        }
    }

    // Create new link
    int result = symlink(updateRootfsPart.c_str(), UPDATEPART_SYMLINK_PATH);
    if (result == -1) {
        std::cout << "ERROR can't create symlink" << std::endl;
    }
}

int Updater::selftest()
{
	return 1;
}