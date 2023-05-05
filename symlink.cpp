#include <iostream>
#include <filesystem>
#include <unistd.h>

#define UNLINK
 
int main()
{
    std::string symlink("/dev/updatePart");
    char targetPath[50];
    ssize_t len = readlink(symlink.c_str(), targetPath, sizeof(targetPath));
    targetPath[len] = '\0';
    std::cout << targetPath << std::endl;
}


std::string getSymbolicLinkTarget(const std::string& linkPath) {
    char targetPath[PATH_MAX];
    ssize_t len = readlink(linkPath.c_str(), targetPath, sizeof(targetPath));
    if (len == -1) {
        // handle error
        return "";
    }
    targetPath[len] = '\0';
    return std::string(targetPath);

        struct stat info;

    #ifndef UNLINK
    
    if (lstat("/dev/updatePart2", &info) == 0)
    // Remove existing link if it exists
    {
        std::cout << "The symbolic link already exists" << std::endl;
        if (unlink("/dev/updatePart2") == -1) {
            // handle error
            std::cout << "Can't unlink existing link" << std::endl;
            return -1;
        }
    }
    else
    {
        std::cout << "Symbolic link doesn't exist" << std::endl;
    }

    // Create new link
    int result = symlink("/dev/sda40", "/dev/updatePart2");
    if (result == -1) {
        std::cout << "ERROR can't create symlink" << std::endl;
    }
    #endif
    #ifdef UNLINK
    if (lstat("/dev/updatePart2", &info) == 0)
    // Remove existing link if it exists
    {
        std::cout << "The symbolic link already exists" << std::endl;
        if (unlink("/dev/updatePart2") == -1) {
            // handle error
            std::cout << "Can't unlink existing link" << std::endl;
            return -1;
        }
        else
        {
            std::cout << "UNLINKED symbolic link" << std::endl;
        }
    }
    else 
    {
        std::cout << "The symbolic link doesn't exist" << std::endl;
        std::cout << info.<< std::endl;
    }
    #endif

}