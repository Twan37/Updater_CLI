#include "UbootEnv.h"
#include <libuboot.h>
#include <string>

#define UBOOT_DEFAULT_CONFIG_PATH   "/etc/fw_env.config"
#define UBOOT_DEFAULT_ENV_PATH      "/etc/u-boot-initial-env"

UbootEnv::UbootEnv()
{
    readEnv()
}

UbootEnv::~UbootEnv()
{

}

void UbootEnv::initEnv()
{
    if (libuboot_initialize(&m_UbootCTX, NULL) < 0)
    {
        std::cout << "[ERROR] Unable to initialize U-Boot environment" << std::endl;
        exit(1);
    }
    
    if (libuboot_read_config(m_UbootCTX, UBOOT_DEFAULT_CONFIG_PATH) < 0)
    {
        std::cout << "[ERROR] Something went wrong while trying to read the " << UBOOT_DEFAULT_CONFIG_PATH << " configuration file" << std::endl;
        exit(1);
    }

    if (libuboot_open(m_UbootCTX) < 0)
    {
        std::cout << "[ERROR] Cannot read the environment by using the " << UBOOT_DEFAULT_CONFIG_PATH << " configuration file" << std::endl;
        if (libuboot_load_file(m_UbootCTX, UBOOT_DEFAULT_CONFIG_PATH) < 0)
        {
            std::cout << "[ERROR] Cannot read default env from the " << UBOOT_DEFAULT_ENV_PATH  << " file" << std::endl;
            exit(1);
        }
    }
}

void UbootEnv::readEnv()
{
    void *tmp = NULL;

    initEnv();
    while ((tmp = libuboot_iterator(m_UbootCTX, tmp)) != NULL) 
    {
        m_envVars[std::string(libuboot_getname(tmp))] = std::string(libuboot_getvalue(tmp));
    }
    closeEnv();
}


void UbootEnv::printEnv()
{
    reloadEnv();
    for (const auto& pair : m_envVars) 
    {
        std::cout << pair.first << "=" << pair.second << std::endl;
    }
}


std::string UbootEnv::getVal(const std::string& keyName)
{
    reloadEnv(); // If SWupdate sets new values it has to reload the env to be able to see them
    return m_envVars[keyName];
}


void UbootEnv::setVal(const std::string& keyName, const std::string& valStr)
{
    m_envVars[keyName] = valStr;
}

void UbootEnv::setVal(const std::string& keyName, int valInt)
{
    m_envVars[keyName] = std::to_string(valInt);
}

void UbootEnv::saveEnv()
{
    initEnv();
    for (const auto& pair : m_envVars) 
    {
        std::cout << pair.first << "=" << pair.second << std::endl;
        libuboot_set_env(m_UbootCTX, pair.first.c_str(), pair.second.c_str());
    }

    // Storing the env. This might fail without the necessary permissions.
    if (libuboot_env_store(m_UbootCTX))
    {
        std::cout << "[ERROR] Can't store the env. This might be due to not having the right permissions" << std::endl;
    }
    closeEnv();
}

void UbootEnv::reloadEnv()
{
    readEnv();  // In this case reloading is just reading again
}

void UbootEnv::closeEnv()
{
    libuboot_close(m_UbootCTX);
    libuboot_exit(m_UbootCTX);
}
