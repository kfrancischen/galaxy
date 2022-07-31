#ifndef CPP_INTERNAL_GALAXY_FS_INTERNAL_H_
#define CPP_INTERNAL_GALAXY_FS_INTERNAL_H_

#include <string>
#include <vector>
#include "absl/time/time.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/container/flat_hash_map.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <sys/types.h>

namespace galaxy {

    namespace internal {
        std::string JoinPath(const std::string& root_path, const std::string& added_path);
        bool ExistDir(const std::string& path);
        bool ExistFile(const std::string& path);
        absl::StatusOr<std::string> GetFileAbsDir(const std::string& abs_path);
        absl::StatusOr<std::string> GetFileName(const std::string& abs_path);
        absl::StatusOr<std::string> GetFileLockName(const std::string& abs_path);
        absl::StatusOr<std::vector<std::string>> ListFilesInDir(const std::string& path, bool include_hidden);
        absl::StatusOr<std::vector<std::string>> ListDirsInDir(const std::string& path);
        absl::StatusOr<std::vector<std::string>> ListDirsInDirRecursive(const std::string& path);
        absl::StatusOr<std::vector<std::string>> ListFilesInDirRecursive(const std::string& path, bool include_hidden);
        bool IsEmpty(const std::string& path);
        int Mkdir(const std::string& path, mode_t mode);
        int MkdirRecursive(const std::string &path, mode_t mode, bool check_exist);
    }

    namespace impl {
        // void LockFile(const std::string& lock_name);
        // void UnLockFile(const std::string& lock_name);
        void Lock(const std::string& path);
        void Unlock(const std::string& path);
        absl::Status CreateDirIfNotExist(const std::string& path, mode_t mode);
        absl::Status DieDirIfNotExist(const std::string& path, std::string& out_path);

        absl::Status CreateFileIfNotExist(const std::string& path, mode_t mode);
        absl::Status DieFileIfNotExist(const std::string& path, std::string& out_path);
        absl::Status ListFilesInDir(const std::string& path, absl::flat_hash_map<std::string, struct stat>& sub_files, bool include_hidden=false);
        absl::Status ListDirsInDir(const std::string& path, absl::flat_hash_map<std::string, struct stat>& sub_dirs);
        absl::Status ListAllInDirRecursive(const std::string& path, absl::flat_hash_map<std::string, struct stat>& sub_dirs,
            absl::flat_hash_map<std::string, struct stat>& sub_files, bool include_hidden=false);

        absl::Status RmDir(const std::string& path, bool include_hidden=false);
        absl::Status RmDirRecursive(const std::string& path, bool include_hidden=false);
        absl::Status RmFile(const std::string& path, bool require_lock);
        absl::Status RenameFile(const std::string& old_path, const std::string& new_path);
        absl::Status Read(const std::string& path, std::string& data);
        absl::Status Write(const std::string& path, const std::string& data, const std::string& mode, bool require_lock);
        absl::Status GetAttr(const std::string& path, struct stat *statbuf);
        absl::Status GetDiskUsage(struct statvfs *statvfsbuf);
        absl::Status GetRamUsage(struct sysinfo *sysinfobuf);
    }
}

#endif  // CPP_INTERNAL_GALAXY_FS_INTERNAL_H_
