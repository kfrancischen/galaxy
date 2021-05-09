#ifndef CPP_GALAXY_CLIENT_H
#define CPP_GALAXY_CLIENT_H
#include <string>
#include <vector>


namespace galaxy {
    namespace client {

        // Remote clients
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
        void Write(const std::string& path, const std::string& data, const std::string& mode="w");
        std::string GetAttr(const std::string& path);

        // Local clients
        void LCreateDirIfNotExist(const std::string& path, const int mode=0777);
        std::string LDirOrDie(const std::string& path);
        void LRmDir(const std::string& path);
        void LRmDirRecursive(const std::string& path);
        std::vector<std::string> LListDirsInDir(const std::string& path);
        std::vector<std::string> LListFilesInDir(const std::string& path);
        void LCreateFileIfNotExist(const std::string& path, const int mode=0777);
        std::string LFileOrDie(const std::string& path);
        void LRmFile(const std::string& path);
        void LRenameFile(const std::string& old_path, const std::string& new_path);
        std::string LRead(const std::string& path);
        void LWrite(const std::string& path, const std::string& data, const std::string& mode="w");
    }  // namespace client
} // namespace galaxy

#endif // CPP_GALAXY_CLIENT_H