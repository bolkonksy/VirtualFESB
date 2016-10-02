#ifndef _IOMANAGER_H
#define _IOMANAGER_H

#include <vector>
#include <string>

namespace Engine
{
    struct File {
        std::string path;
        bool directoryFlag;
    };

    class IOManager {
    public:
        static bool readFileToBuffer(const std::string& filePath, std::vector<unsigned char>& buffer);
        static bool readFileToBuffer(const std::string& filePath, std::string& buffer);

        static bool listFiles(const char* path, std::vector<File>& dirList);
        static bool createDirectory(const char* path);
    };
}

#endif