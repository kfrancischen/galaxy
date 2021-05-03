#ifndef CPP_INTERNAL_GALAXY_FS_INTERNAL_H_
#define CPP_INTERNAL_GALAXY_FS_INTERNAL_H_

#include <string>
#include <vector>
#include "absl/time/time.h"

namespace galaxy {

    namespace internal {

        constexpr char kSeperator = '/';
        constexpr char kLockNameTemplate[] = ".$0.lock";

        std::string JoinPath(const std::string& root_path, const std::string& added_path);
        bool ExistDir(const std::string& path);
        bool ExistFile(const std::string& path);
        std::string GetFileAbsDir(const std::string& abs_path);
        std::string GetFileName(const std::string& abs_path);
        std::string GetFileLockName(const std::string& abs_path);
        std::vector<std::string> ListFilesInDir(const std::string& path);
        std::vector<std::string> ListDirsInDir(const std::string& path);
        bool isEmpty(const std::string& path);
        int Mkdir(const std::string& path, mode_t mode);
        int MkdirRecursive(const std::string &path, mode_t mode, bool check_exist);
    }

    namespace impl {
        constexpr absl::Duration kLockRetry = absl::Milliseconds(1);
        int CreateDirIfNotExist(const std::string& path, mode_t mode);
        int DieDirIfNotExist(const std::string& path, mode_t mode);

        int CreateFileIfNotExist(const std::string& path, mode_t mode);
        int DieFileIfNotExist(const std::string& path, mode_t mode);

        int RmDir(const std::string& path);
        int RmDirRecursive(const std::string& path);
        int RmFile(const std::string& path);
        int RenameFile(const std::string& old_path, const std::string& new_path);
        int Read(const std::string& path, std::string& data);
        int Write(const std::string& path, const std::string& data);
    }
}

#endif  // CPP_INTERNAL_GALAXY_FS_INTERNAL_H_
