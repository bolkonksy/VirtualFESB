#include "IOManager.h"

#include <fstream>
#include <filesystem>

namespace fs = std::tr2::sys;

namespace Engine
{
    bool IOManager::readFileToBuffer(const std::string& filePath, std::vector<unsigned char>& buffer)
    {
        std::ifstream file(filePath, std::ios::binary);
        if (file.fail()) {
            perror(filePath.c_str());
            return false;
        }

        file.seekg(0, std::ios::end);
        unsigned int fileSize = (unsigned int)file.tellg();

        file.seekg(0, std::ios::beg);
        fileSize -= (unsigned int)file.tellg();

        buffer.resize(fileSize);

        file.read((char*)&(buffer[0]), fileSize);

        file.close();
        return true;
    }

    bool IOManager::readFileToBuffer(const std::string& filePath, std::string& buffer)
    {
        std::ifstream file(filePath, std::ios::binary);
        if (file.fail()) {
            perror(filePath.c_str());
            return false;
        }

        file.seekg(0, std::ios::end);
        unsigned int fileSize = (unsigned int)file.tellg();

        file.seekg(0, std::ios::beg);
        fileSize -= (unsigned int)file.tellg();

        buffer.resize(fileSize);

        file.read((char*)&(buffer[0]), fileSize);

        file.close();
        return true;
    }

    bool IOManager::listFiles(const char* path, std::vector<File>& dirList)
    {
        auto dirPath = fs::path(path);

        if (!fs::is_directory(dirPath))
            return false;

        for (auto it = fs::directory_iterator(dirPath); it != fs::directory_iterator(); it++) {
            dirList.emplace_back();
            dirList.back().path = it->path().string();
            if (is_directory(it->path()))
                dirList.back().directoryFlag = true;
            else
                dirList.back().directoryFlag = false;
        }
        
        return true;
    }

    bool IOManager::createDirectory(const char* path)
    {
        return fs::create_directory(fs::path(path));
    }
}