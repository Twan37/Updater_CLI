#include "UbootEnv.h"
#include <libuboot.h>
#include <string>

#define UBOOT_DEFAULT_ENV_PATH  "/etc/fw_env.config"

UbootEnv::UbootEnv()
{

}

UbootEnv::~UbootEnv()
{

}

void UbootEnv::initEnv()
{
    if (libuboot_initialize(&m_UbootCTX, NULL) < 0)
    {
        std::cout << "ERROR: Environment not initialized" << std::endl;
        exit(1);
    }
    
    if (libuboot_read_config(m_UbootCTX, UBOOT_DEFAULT_ENV_PATH) < 0)
    {
        std::cout << "ERROR: Something is wrong when trying to read the " << UBOOT_DEFAULT_ENV_PATH << " configuration file" << std::endl; 
        exit(1);
    }

    if (libuboot_open(m_UbootCTX) < 0)
    {
        std::cout << "Cannot read env using default" << std::endl;
        if (libuboot_load_file(m_UbootCTX, UBOOT_DEFAULT_ENV_PATH) < 0)
        {
            std::cout << "ERROR: Cannot read default env from file" << std::endl;
            exit(1);
        }
    }

}


void UbootEnv::printEnv()
{
    initEnv();
    void *tmp = NULL;
    while ((tmp = libuboot_iterator(m_UbootCTX, tmp)) != NULL) 
    {
        std::cout << libuboot_getname(tmp) << "=" << libuboot_getvalue(tmp) << std::endl;
    }
    libuboot_close(m_UbootCTX);
    libuboot_exit(m_UbootCTX);
}

std::string UbootEnv::getVal(std::string keyName)
{
    initEnv();
    return std::string(libuboot_get_env(m_UbootCTX, keyName.c_str()));
    libuboot_close(m_UbootCTX);
    libuboot_exit(m_UbootCTX);
}


void UbootEnv::setVal(std::string keyName, std::string valStr)
{
    initEnv();
    libuboot_set_env(m_UbootCTX, keyName.c_str(), valStr.c_str());
    
    // Storing the env. This might fail without the necessary permissions.
    if (libuboot_env_store(m_UbootCTX))
    {
        std::cout << "Eror storing the env" << std::endl;
    }

    libuboot_close(m_UbootCTX);
    libuboot_exit(m_UbootCTX);
}

void UbootEnv::setVal(std::string keyName, int valInt)
{
    std::string valIntStr = std::to_string(valInt);
    setVal(keyName, valIntStr);
}