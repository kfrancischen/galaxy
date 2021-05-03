#include "cpp/internal/galaxy_fs_internal.h"

#include <iostream>
#include <cstdio>
#include <limits.h>
#include <fstream>
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace galaxy {
    namespace internal {

        std::string JoinPath(const std::string& root_path, const std::string& added_path) {
            if (root_path.back() != kSeperator) {
                std::string seprator(1, kSeperator);
                return absl::StrCat(root_path, seprator, added_path);
            } else {
                return absl::StrCat(root_path, added_path);
            }
        }

        bool ExistDir(const std::string& path) {
            struct stat buffer;
            if ( stat(path.c_str(), &buffer) != 0 ) {
                return false;
            } else if (buffer.st_mode & S_IFDIR) {
                return true;
            } else {
                return false;
            }
        }

        bool ExistFile(const std::string& path) {
            struct stat buffer;
            if ( stat(path.c_str(), &buffer) != 0 ) {
                return false;
            } else if (buffer.st_mode & S_IFREG) {
                return true;
            } else {
                return false;
            }
        }

        std::string GetFileAbsDir(const std::string& abs_path) {
            std::vector<std::string> v = absl::StrSplit(abs_path, kSeperator);
            CHECK(v.size() >= 1) << "Wrong format of path.";
            v.pop_back();
            std::string seprator(1, kSeperator);
            return absl::StrJoin(v, seprator);
        }

        std::vector<std::string> ListFilesInDir(const std::string& path) {
            CHECK(ExistDir(path)) << " is not directory or does not exist.";
            std::vector<std::string> file_paths;

            DIR* dirp = opendir(path.c_str());
            struct dirent* dp;
            while ((dp = readdir(dirp)) != NULL) {
                if (dp->d_name[0] == '.') {
                    // Ignore all the hidden files.
                    continue;
                }
                std::string file_path = JoinPath(path, dp->d_name);
                if (ExistFile(file_path)) {
                    file_paths.push_back(file_path);
                }
            }
            closedir(dirp);
            return file_paths;
        }

        std::vector<std::string> ListDirsInDir(const std::string& path) {
            CHECK(ExistDir(path)) << " is not directory or does not exist.";
            std::vector<std::string> dir_paths;
            DIR* dirp = opendir(path.c_str());
            struct dirent* dp;
            while ((dp = readdir(dirp)) != NULL) {
                if (dp->d_name[0] == '.') {
                    // Ignore all the hidden files.
                    continue;
                }
                std::string dir_path = JoinPath(path, dp->d_name);
                if (ExistDir(dir_path)) {
                    dir_paths.push_back(dir_path);
                }
            }
            closedir(dirp);
            return dir_paths;
        }

        bool IsEmpty(const std::string& path) {
            CHECK(ExistDir(path)) << " is not directory or does not exist.";
            DIR* dirp = opendir(path.c_str());
            struct dirent * dp;
            while ((dp = readdir(dirp)) != NULL) {
                if (dp->d_name[0] == '.') {
                    // Ignore all the hidden files.
                    continue;
                }
                return false;
            }
            closedir(dirp);
            return true;
        }

        int Mkdir(const std::string& path, mode_t mode) {
            if (ExistDir(path)) {
                LOG(INFO) << "Directory " << path << " already exist.";
            } else {
                LOG(INFO) << "Making directory " + path << ".";
                if (mkdir(path.c_str(), mode) != 0 && errno != EEXIST) {
                    LOG(ERROR) << "Making directory " + path << " failed.";
                    return -1;
                }
            }
            return 0;
        }

        int MkdirRecursive(const std::string &path, mode_t mode, bool check_exist) {
            if (check_exist) {
                CHECK(internal::ExistDir(path)) << "path does not exist.";
            }
            char tmp[PATH_MAX];
            char *p;

            strcpy(tmp, path.c_str());

            for (p = tmp + 1; *p; p++) {
               if (*p == '/') {
                    *p = 0;
                    if (Mkdir(tmp, mode) == -1) {
                        return -1;
                    }
                    *p = '/';
                }
            }
            if (Mkdir(tmp, mode) == -1) {
                return -1;
            }
            return 0;
        }

    }

    namespace impl {
        int CreateDirIfNotExist(const std::string &path, mode_t mode) {
            return internal::MkdirRecursive(path, mode, false);
        }

        int DieDirIfNotExist(const std::string &path, mode_t mode) {
            return internal::MkdirRecursive(path, mode, true);
        }

        int CreateFileIfNotExist(const std::string& path, mode_t mode) {
            std::string dir = internal::GetFileAbsDir(path);
            CHECK_EQ(CreateDirIfNotExist(dir, mode), 0) << "Creating directory failed.";
            if (internal::ExistFile(path)) {
                LOG(INFO) << "File " << path << " already exist.";
            } else {
                std::ofstream (path.c_str());
                LOG(INFO) << "Creating file " << path <<".";
            }
            return 0;
        }

        int DieFileIfNotExist(const std::string& path, mode_t mode) {
            std::string dir = internal::GetFileAbsDir(path);
            DieDirIfNotExist(dir, mode);
            CHECK(!internal::ExistFile(path)) << "File does not exist";
            return 0;
        }

        int RmDir(const std::string& path) {
            if (!internal::ExistDir(path)) {
                LOG(WARNING) << "Directory " << path << " does not exist.";
                return 0;
            }
            std::vector<std::string> file_paths = internal::ListFilesInDir(path);
            for (const auto & file_path : file_paths) {
                RmFile(file_path);
            }
            if (rmdir(path.c_str()) == 0) {
                LOG(INFO) << "Removed directory " << path << ".";
                return 0;
            } else {
                LOG(ERROR) << "Removing directory " << path << " failed.";
                return -1;
            }
        }

        int RmDirRecursive(const std::string& path) {
            std::vector<std::string> dirs_path = internal::ListDirsInDir(path);
            if (!dirs_path.empty()) {
                for (const auto& dir_path : dirs_path) {
                    RmDirRecursive(dir_path);
                }
            }
            return RmDir(path);
        }

        int RmFile(const std::string& path) {
            if (!internal::ExistFile(path)) {
                LOG(WARNING) << "File " << path << " does not exist.";
                return 1;
            } else {
                if (remove(path.c_str()) != 0) {
                    LOG(ERROR) << "Removing file " << path << " failed.";
                    return -1;
                } else {
                    LOG(INFO) << "Removed file " << path << ".";
                    return 0;
                }
            }
        }

        int RenameFile(const std::string& old_path, const std::string& new_path) {
            CHECK(!internal::ExistFile(old_path)) << old_path + " does not exist";
            result = rename(old_path, new_path);
            if (result == 0) {
                LOG(INFO) << "Renamed from " << old_path << " to " << new_path;
                return 0;
            } else{
                LOG(ERROR) << "Renaming file " << old_path << " failed".
                return -1;
            }
        }
    }
}