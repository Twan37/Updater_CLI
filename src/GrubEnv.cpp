#include "GrubEnv.h"
#include <fstream>
#include <map>
#include <string>

#define GRUBENV_DEFAULT_PATH    "/boot/grub/grubenv"
#define GRUBENV_PATH            GRUBENV_DEFAULT_PATH
#define GRUBENV_SIZE            1024
#define GRUBENV_HEADER          "# GRUB Environment Block\n"


GrubEnv::GrubEnv()
{
    
}

GrubEnv::~GrubEnv()
{

}


void GrubEnv::printEnv()
{
    readEnv();
    for (const auto& pair : m_envVars) 
    {
        std::cout << pair.first << "=" << pair.second << std::endl;
    }
}

std::string GrubEnv::getVal(std::string keyName)
{
    readEnv();
    return m_envVars[keyName];
}

void GrubEnv::setVal(std::string keyName, std::string valStr)
{
    readEnv();
    m_envVars[keyName] = valStr;
    writeEnv();
}

void GrubEnv::setVal(std::string keyName, int valInt)
{
    readEnv();
    m_envVars[keyName] = std::to_string(valInt);
    writeEnv();
}


void GrubEnv::readEnv()
{
    std::ifstream grubEnvFile(GRUBENV_PATH);

    if (!grubEnvFile.is_open()) 
    {
        std::cout << "Error: failed to open " << GRUBENV_PATH << std::endl;
        return;
    }
    
    std::string line;
    while (getline(grubEnvFile, line)) 
    {
        if (line.size() > 0 && line[0] != '#') 
        {
            size_t pos = line.find('=');
            if (pos != std::string::npos) 
            {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                m_envVars.insert({key, value});
            }
        }
    }
    grubEnvFile.close();
}

void GrubEnv::writeEnv()
{
    std::string envOutput = GRUBENV_HEADER;
    for (const auto& pair : m_envVars) 
    {
        envOutput += pair.first + "=" + pair.second + "\n";
    }
    envOutput.resize(GRUBENV_SIZE, '#');
    //std::cout << envOutput << std::endl;

    std::ofstream grubEnvFile(GRUBENV_DEFAULT_PATH);

    if (grubEnvFile.is_open())
    {
        grubEnvFile << envOutput.substr(0, GRUBENV_SIZE);   // Make the sure the length never exceeds GRUBENV_SIZE
        grubEnvFile.close();
        std::cout << "Successfully wrote to GrubEnv file" << std::endl;
    }
    else
    {
        std::cout << "Unable to open the GrubEnv file for writing" << std::endl;
    }

    // First writing to a copy and then renaming to original might even be a safer way of doing things
}