#ifndef CPP_CORE_GALAXY_FS_H_
#define CPP_CORE_GALAXY_FS_H_

#include <string>
#include <memory>
#include "absl/status/status.h"
#include "absl/container/flat_hash_map.h"
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>

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

        void Lock(const std::string& path);
        void Unlock(const std::string& path);
        absl::Status CreateDirIfNotExist(const std::string& path, mode_t mode=0777);
        absl::Status DieDirIfNotExist(const std::string& path, std::string& out_path);

        absl::Status CreateFileIfNotExist(const std::string& path, mode_t mode=0777);
        absl::Status DieFileIfNotExist(const std::string& path, std::string& out_path);

        absl::Status ListDirsInDir(const std::string& path, absl::flat_hash_map<std::string, struct stat>& sub_dirs);
        absl::Status ListFilesInDir(const std::string& path, absl::flat_hash_map<std::string, struct stat>& sub_files, bool include_hidden=false);

        absl::Status ListAllInDirRecursive(const std::string& path, absl::flat_hash_map<std::string, struct stat>& sub_dirs,
            absl::flat_hash_map<std::string, struct stat>& sub_files, bool include_hidden=false);

        absl::Status RmDir(const std::string& path, bool include_hidden=false);
        absl::Status RmDirRecursive(const std::string& path, bool include_hidden=false);
        absl::Status RmFile(const std::string& path);
        absl::Status RenameFile(const std::string& old_path, const std::string& new_path);

        absl::Status Read(const std::string& path, std::string& data);
        absl::Status Write(const std::string& path, const std::string& data, const std::string& mode="w", bool require_lock=true);
        absl::Status GetAttr(const std::string& path, struct stat *statbuf);
        absl::Status GetDiskUsage(struct statvfs *statvfsbuf);
        absl::Status GetRamUsage(struct sysinfo *sysinfobuf);

    private:
        std::string root_;
    };
} //  namespace galaxy.
#endif   //  CPP_CORE_GALAXY_FS_H_
