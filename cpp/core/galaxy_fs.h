#ifndef CPP_CORE_GALAXY_FS_H_
#define CPP_CORE_GALAXY_FS_H_

#include <string>
#include <memory>
#include "absl/status/status.h"

namespace galaxy
{

    class GalaxyFs
    {
    public:
        GalaxyFs(const std::string& root);
        ~GalaxyFs();
        GalaxyFs(const GalaxyFs&) = delete;

        static std::unique_ptr<GalaxyFs> Instance();

        void SetRootDir(const std::string& path);

        absl::Status CreateDirIfNotExist(const std::string& path, mode_t mode=0777);
        absl::Status DieDirIfNotExist(const std::string& path, std::string& out_path);

        absl::Status CreateFileIfNotExist(const std::string& path, mode_t mode=0777);
        absl::Status DieFileIfNotExist(const std::string& path, std::string& out_path);

        absl::Status ListDirsInDir(const std::string& path, std::vector<std::string>& sub_dirs);
        absl::Status ListFilesInDir(const std::string& path, std::vector<std::string>& sub_files);

        absl::Status ListAllInDirRecursive(const std::string& path, std::vector<std::string>& sub_dirs,
            std::vector<std::string>& sub_files);

        absl::Status RmDir(const std::string& path);
        absl::Status RmDirRecursive(const std::string& path);
        absl::Status RmFile(const std::string& path);
        absl::Status RenameFile(const std::string& old_path, const std::string& new_path);

        absl::Status Read(const std::string& path, std::string& data);
        absl::Status Write(const std::string& path, const std::string& data, const std::string& mode);
        absl::Status GetAttr(const std::string& path, struct stat *statbuf);

    private:
        std::string root_;
    };
} //  namespace galaxy.
#endif   //  CPP_CORE_GALAXY_FS_H_
