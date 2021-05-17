#ifndef CPP_GALAXY_CLIENT_H
#define CPP_GALAXY_CLIENT_H
#include <string>
#include <vector>
#include <map>


namespace galaxy {
    namespace client {
        namespace impl {
            // Remote clients
            void RCreateDirIfNotExist(const std::string& path, const int mode=0777);
            std::string RDirOrDie(const std::string& path);
            void RRmDir(const std::string& path);
            void RRmDirRecursive(const std::string& path);
            std::vector<std::string> RListDirsInDir(const std::string& path);
            std::vector<std::string> RListFilesInDir(const std::string& path);
            std::vector<std::string> RListDirsInDirRecursive(const std::string& path);
            std::vector<std::string> RListFilesInDirRecursive(const std::string& path);
            void RCreateFileIfNotExist(const std::string& path, const int mode=0777);
            std::string RFileOrDie(const std::string& path);
            void RRmFile(const std::string& path);
            void RRenameFile(const std::string& old_path, const std::string& new_path);
            std::string RRead(const std::string& path);
            std::map<std::string, std::string> RReadMultiple(const std::vector<std::string>& paths);
            void RWrite(const std::string& path, const std::string& data, const std::string& mode="w");
            std::string RGetAttr(const std::string& path);

            // Local clients
            void LCreateDirIfNotExist(const std::string& path, const int mode=0777);
            std::string LDirOrDie(const std::string& path);
            void LRmDir(const std::string& path);
            void LRmDirRecursive(const std::string& path);
            std::vector<std::string> LListDirsInDir(const std::string& path);
            std::vector<std::string> LListFilesInDir(const std::string& path);
            std::vector<std::string> LListDirsInDirRecursive(const std::string& path);
            std::vector<std::string> LListFilesInDirRecursive(const std::string& path);
            void LCreateFileIfNotExist(const std::string& path, const int mode=0777);
            std::string LFileOrDie(const std::string& path);
            void LRmFile(const std::string& path);
            void LRenameFile(const std::string& old_path, const std::string& new_path);
            std::string LRead(const std::string& path);
            std::map<std::string, std::string> LReadMultiple(const std::vector<std::string>& paths);
            void LWrite(const std::string& path, const std::string& data, const std::string& mode="w");
            std::string LGetAttr(const std::string& path);
        }
        void CreateDirIfNotExist(const std::string& path, const int mode=0777);
        std::string DirOrDie(const std::string& path);
        void RmDir(const std::string& path);
        void RmDirRecursive(const std::string& path);
        std::vector<std::string> ListDirsInDir(const std::string& path);
        std::vector<std::string> ListFilesInDir(const std::string& path);
        std::vector<std::string> ListDirsInDirRecursive(const std::string& path);
        std::vector<std::string> ListFilesInDirRecursive(const std::string& path);
        void CreateFileIfNotExist(const std::string& path, const int mode=0777);
        std::string FileOrDie(const std::string& path);
        void RmFile(const std::string& path);
        void RenameFile(const std::string& old_path, const std::string& new_path);
        std::string Read(const std::string& path);
        std::map<std::string, std::string> ReadMultiple(const std::vector<std::string>& paths);
        void Write(const std::string& path, const std::string& data, const std::string& mode="w");
        std::string GetAttr(const std::string& path);
    }  // namespace client
} // namespace galaxy

#endif // CPP_GALAXY_CLIENT_H