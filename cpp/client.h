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
            void RRmDir(const std::string& path, bool include_hidden=false);
            void RRmDirRecursive(const std::string& path, bool include_hidden=false);
            std::map<std::string, std::string> RListDirsInDir(const std::string& path);
            std::map<std::string, std::string> RListFilesInDir(const std::string& path, bool include_hidden=false);
            std::map<std::string, std::string> RListDirsInDirRecursive(const std::string& path);
            std::map<std::string, std::string> RListFilesInDirRecursive(const std::string& path, bool include_hidden=false);
            void RCreateFileIfNotExist(const std::string& path, const int mode=0777);
            std::string RFileOrDie(const std::string& path);
            void RRmFile(const std::string& path, bool is_hidden=false);
            void RRenameFile(const std::string& old_path, const std::string& new_path);
            std::string RRead(const std::string& path);
            std::map<std::string, std::string> RReadMultiple(const std::vector<std::string>& paths);
            void RWrite(const std::string& path, const std::string& data, const std::string& mode="w");
            void RWriteMultiple(const std::map<std::string, std::string>& path_data_map, const std::string& mode="w");
            std::string RGetAttr(const std::string& path);
            std::string RCheckHealth(const std::string& cell);

            // Local clients
            void LCreateDirIfNotExist(const std::string& path, const int mode=0777);
            std::string LDirOrDie(const std::string& path);
            void LRmDir(const std::string& path, bool include_hidden=false);
            void LRmDirRecursive(const std::string& path, bool include_hidden=false);
            std::map<std::string, std::string> LListDirsInDir(const std::string& path);
            std::map<std::string, std::string> LListFilesInDir(const std::string& path, bool include_hidden=false);
            std::map<std::string, std::string> LListDirsInDirRecursive(const std::string& path);
            std::map<std::string, std::string> LListFilesInDirRecursive(const std::string& path, bool include_hidden=false);
            void LCreateFileIfNotExist(const std::string& path, const int mode=0777);
            std::string LFileOrDie(const std::string& path);
            void LRmFile(const std::string& path, bool is_hidden=false);
            void LRenameFile(const std::string& old_path, const std::string& new_path);
            std::string LRead(const std::string& path);
            std::map<std::string, std::string> LReadMultiple(const std::vector<std::string>& paths);
            void LWrite(const std::string& path, const std::string& data, const std::string& mode="w");
            void LWriteMultiple(const std::map<std::string, std::string>& path_data_map, const std::string& mode="w");
            std::string LGetAttr(const std::string& path);
        }
        void CreateDirIfNotExist(const std::string& path, const int mode=0777);
        std::string DirOrDie(const std::string& path);
        void RmDir(const std::string& path, bool include_hidden=false);
        void RmDirRecursive(const std::string& path, bool include_hidden=false);
        std::map<std::string, std::string> ListDirsInDir(const std::string& path);
        std::map<std::string, std::string> ListFilesInDir(const std::string& path, bool include_hidden=false);
        std::map<std::string, std::string> ListDirsInDirRecursive(const std::string& path);
        std::map<std::string, std::string> ListFilesInDirRecursive(const std::string& path, bool include_hidden=false);
        void CreateFileIfNotExist(const std::string& path, const int mode=0777);
        std::string FileOrDie(const std::string& path);
        void RmFile(const std::string& path, bool is_hidden=false);
        void RenameFile(const std::string& old_path, const std::string& new_path);
        std::string Read(const std::string& path);
        std::map<std::string, std::string> ReadMultiple(const std::vector<std::string>& paths);
        void Write(const std::string& path, const std::string& data, const std::string& mode="w");
        void WriteMultiple(const std::map<std::string, std::string>& path_data_map, const std::string& mode="w");
        std::string GetAttr(const std::string& path);
        std::vector<std::string> ListCells();
        std::string CheckHealth(const std::string& cell);
    }  // namespace client
} // namespace galaxy

#endif // CPP_GALAXY_CLIENT_H