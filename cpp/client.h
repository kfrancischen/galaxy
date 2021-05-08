#ifndef CPP_GALAXY_CLIENT_H
#define CPP_GALAXY_CLIENT_H
#include <string>
#include <vector>
#include "cpp/internal/galaxy_client_internal.h"
#include "glog/logging.h"

namespace galaxy {
    namespace client {
        void CreateDirIfNotExist(const std::string& path, const int mode=0777);
        std::string DirOrDie(const std::string& path);
        void RmDir(const std::string& path);
        void RmDirRecursive(const std::string& path);
        std::vector<std::string> ListDirsInDir(const std::string& path);
        std::vector<std::string> ListFilesInDir(const std::string& path);
        void CreateFileIfNotExist(const std::string& path, const int mode=0777);
        std::string FileOrDie(const std::string& path);
        void RmFile(const std::string& path);
        void RenameFile(const std::string& old_path, const std::string& new_path);
        std::string Read(const std::string& path);
        void Write(const std::string& path, const std::string& data);
        std::string GetAttr(const std::string& path);
    }  // namespace client
} // namespace galaxy

#endif // CPP_GALAXY_CLIENT_H