#ifndef CPP_GALAXY_CLIENT_H
#define CPP_GALAXY_CLIENT_H
#include <string>
#include <vector>
#include <map>
#include "schema/fileserver.pb.h"


namespace galaxy {
    namespace client {
        namespace impl {
            // Remote clients
            void RCreateDirIfNotExist(const galaxy_schema::FileAnalyzerResult& result, const int mode=0777);
            std::string RDirOrDie(const galaxy_schema::FileAnalyzerResult& result);
            void RRmDir(const galaxy_schema::FileAnalyzerResult& result, bool include_hidden=false);
            void RRmDirRecursive(const galaxy_schema::FileAnalyzerResult& result, bool include_hidden=false);
            std::map<std::string, std::string> RListDirsInDir(const galaxy_schema::FileAnalyzerResult& result);
            std::map<std::string, std::string> RListFilesInDir(const galaxy_schema::FileAnalyzerResult& result, bool include_hidden=false);
            std::map<std::string, std::string> RListDirsInDirRecursive(const galaxy_schema::FileAnalyzerResult& result);
            std::map<std::string, std::string> RListFilesInDirRecursive(const galaxy_schema::FileAnalyzerResult& result, bool include_hidden=false);
            void RCreateFileIfNotExist(const galaxy_schema::FileAnalyzerResult& result, const int mode=0777);
            std::string RFileOrDie(const galaxy_schema::FileAnalyzerResult& result);
            void RRmFile(const galaxy_schema::FileAnalyzerResult& result, bool is_hidden=false);
            void RRenameFile(const galaxy_schema::FileAnalyzerResult& old_result, const galaxy_schema::FileAnalyzerResult& new_result);
            std::string RRead(const galaxy_schema::FileAnalyzerResult& result);
            std::map<std::string, std::string> RReadMultiple(const std::vector<galaxy_schema::FileAnalyzerResult>& results);
            void RWrite(const galaxy_schema::FileAnalyzerResult& result, const std::string& data, const std::string& mode="w");
            void RWriteMultiple(const std::vector<std::pair<galaxy_schema::FileAnalyzerResult, std::string>>& path_data_map, const std::string& mode="w");
            std::string RGetAttr(const galaxy_schema::FileAnalyzerResult& result);
            std::string RCheckHealth(const std::string& cell);

            // Local clients
            void LCreateDirIfNotExist(const galaxy_schema::FileAnalyzerResult& result, const int mode=0777);
            std::string LDirOrDie(const galaxy_schema::FileAnalyzerResult& result);
            void LRmDir(const galaxy_schema::FileAnalyzerResult& result, bool include_hidden=false);
            void LRmDirRecursive(const galaxy_schema::FileAnalyzerResult& result, bool include_hidden=false);
            std::map<std::string, std::string> LListDirsInDir(const galaxy_schema::FileAnalyzerResult& result);
            std::map<std::string, std::string> LListFilesInDir(const galaxy_schema::FileAnalyzerResult& result, bool include_hidden=false);
            std::map<std::string, std::string> LListDirsInDirRecursive(const galaxy_schema::FileAnalyzerResult& result);
            std::map<std::string, std::string> LListFilesInDirRecursive(const galaxy_schema::FileAnalyzerResult& result, bool include_hidden=false);
            void LCreateFileIfNotExist(const galaxy_schema::FileAnalyzerResult& result, const int mode=0777);
            std::string LFileOrDie(const galaxy_schema::FileAnalyzerResult& result);
            void LRmFile(const galaxy_schema::FileAnalyzerResult& result, bool is_hidden=false);
            void LRenameFile(const galaxy_schema::FileAnalyzerResult& old_result, const galaxy_schema::FileAnalyzerResult& new_result);
            std::string LRead(const galaxy_schema::FileAnalyzerResult& result);
            std::map<std::string, std::string> LReadMultiple(const std::vector<galaxy_schema::FileAnalyzerResult>& results);
            void LWrite(const galaxy_schema::FileAnalyzerResult& result, const std::string& data, const std::string& mode="w");
            void LWriteMultiple(const std::vector<std::pair<galaxy_schema::FileAnalyzerResult, std::string>>& path_data_map, const std::string& mode="w");
            std::string LGetAttr(const galaxy_schema::FileAnalyzerResult& result);
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