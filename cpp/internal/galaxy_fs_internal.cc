#include "cpp/internal/galaxy_fs_internal.h"
#include "cpp/internal/galaxy_const.h"

#include <iostream>
#include <cstdio>
#include <limits.h>
#include <fstream>
#include <iterator>
#include <streambuf>
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/strings/str_join.h"
#include "absl/strings/substitute.h"
#include "absl/time/clock.h"
#include "glog/logging.h"
#include <dirent.h>

namespace galaxy {
    namespace internal {

        std::string JoinPath(const std::string& root_path, const std::string& added_path) {
            if (root_path.empty()) {
                LOG(WARNING) << "In local mode.";
                return added_path;
            }
            if (root_path.back() != galaxy::constant::kSeparator) {
                std::string seprator(1, galaxy::constant::kSeparator);
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

        absl::StatusOr<std::string> GetFileAbsDir(const std::string& abs_path) {
            std::vector<std::string> v = absl::StrSplit(abs_path, galaxy::constant::kSeparator);
            if (v.size() < 1) {
                return absl::FailedPreconditionError("Path incorrect.");
            }

            v.pop_back();
            std::string seprator(1, galaxy::constant::kSeparator);
            return absl::StrJoin(v, seprator);
        }

        absl::StatusOr<std::string> GetFileName(const std::string& abs_path) {
            std::vector<std::string> v = absl::StrSplit(abs_path, galaxy::constant::kSeparator);
            if (v.size() < 1) {
                return absl::FailedPreconditionError("Path incorrect.");
            }
            return v.back();
        }

        absl::StatusOr<std::string> GetFileLockName(const std::string& abs_path) {
            absl::StatusOr<std::string> file_name = GetFileName(abs_path);
            absl::StatusOr<std::string> abs_dir = GetFileAbsDir(abs_path);
            if (file_name.ok() && abs_dir.ok()) {
                std::string lock_name_template(galaxy::constant::kLockNameTemplate);
                std::string lock_name = absl::Substitute(lock_name_template, *file_name);
                return JoinPath(*abs_dir, lock_name);
            } else {
                return absl::FailedPreconditionError("Path incorrect.");
            }

        }

        absl::StatusOr<std::vector<std::string>> ListFilesInDir(const std::string& path) {
            if (!ExistDir(path)) {
                return absl::NotFoundError("Input path is not directory or does not exist.");
            }
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

        void _ListFilesInDirRecursive(const std::string& path, std::vector<std::string>& result) {
            absl::StatusOr<std::vector<std::string>> sub_files_or = ListFilesInDir(path);
            if (sub_files_or.ok()) {
                std::vector<std::string> sub_files = *sub_files_or;
                result.reserve(result.size() + std::distance(sub_files.begin(), sub_files.end()));
                result.insert(result.end(), sub_files.begin(), sub_files.end());
            }

            absl::StatusOr<std::vector<std::string>> sub_dirs_or = ListDirsInDir(path);
            if (sub_dirs_or.ok()) {
                for (const auto& val : *sub_dirs_or) {
                    _ListFilesInDirRecursive(val, result);
                }
            }
        }

        absl::StatusOr<std::vector<std::string>> ListFilesInDirRecursive(const std::string& path) {
            if (!ExistDir(path)) {
                return absl::NotFoundError("Input path is not directory or does not exist.");
            }
            std::vector<std::string> sub_files;
            _ListFilesInDirRecursive(path, sub_files);
            return sub_files;
        }

        absl::StatusOr<std::vector<std::string>> ListDirsInDir(const std::string& path) {
            if (!ExistDir(path)) {
                return absl::NotFoundError("Input path is not directory or does not exist.");
            }
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

        void _ListDirsInDirRecursive(const std::string& path, std::vector<std::string>& result) {
            absl::StatusOr<std::vector<std::string>> sub_dirs_or = ListDirsInDir(path);
            if (sub_dirs_or.ok()) {
                std::vector<std::string> sub_dirs = *sub_dirs_or;
                result.reserve(result.size() + std::distance(sub_dirs.begin(), sub_dirs.end()));
                result.insert(result.end(), sub_dirs.begin(), sub_dirs.end());
                for (const auto& val : sub_dirs) {
                    _ListDirsInDirRecursive(val, result);
                }
            }
        }

        absl::StatusOr<std::vector<std::string>> ListDirsInDirRecursive(const std::string& path) {
            if (!ExistDir(path)) {
                return absl::NotFoundError("Input path is not directory or does not exist.");
            }
            std::vector<std::string> sub_dirs;
            _ListDirsInDirRecursive(path, sub_dirs);
            return sub_dirs;
        }

        bool IsEmpty(const std::string& path) {
            if (!ExistDir(path)) {
                return true;;
            }
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
                VLOG(1) << "Directory " << path << " already exist.";
            } else {
                VLOG(1) << "Making directory " << path << ".";
                if (mkdir(path.c_str(), mode) != 0 && errno != EEXIST) {
                    LOG(ERROR) << "Making directory " << path << " failed.";
                    return -1;
                }
            }
            return 0;
        }

        int MkdirRecursive(const std::string &path, mode_t mode) {
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

        absl::Status CreateDirIfNotExist(const std::string &path, mode_t mode) {
            if (internal::MkdirRecursive(path, mode) != 0) {
                return absl::InvalidArgumentError("Invalid argument for CreateDirIfNotExist: " + path + ".");
            } else {
                return absl::OkStatus();
            }
        }

        absl::Status DieDirIfNotExist(const std::string &path, std::string& out_path) {
            if (!internal::ExistDir(path)) {
                LOG(ERROR) << "Path " << path << " does not exist during function call DieDirIfNotExist.";
                return absl::NotFoundError("Path " + path + " does not exist for DieDirIfNotExist.");
            } else {
                out_path = path;
                return absl::OkStatus();
            }
        }

        absl::Status CreateFileIfNotExist(const std::string& path, mode_t mode) {
            absl::StatusOr<std::string> dir = internal::GetFileAbsDir(path);
            if (!dir.ok() || !CreateDirIfNotExist(*dir, mode).ok()){
                return absl::InternalError("CreateFileIfNotExist failed for " + path + " because dir creation failed.");
            }
            if (internal::ExistFile(path)) {
                VLOG(1) << "File " << path << " already exist.";
            } else {
                std::ofstream (path.c_str());
                VLOG(1) << "Creating file " << path <<".";
            }
            return absl::OkStatus();
        }

        absl::Status DieFileIfNotExist(const std::string& path, std::string& out_path) {
            absl::StatusOr<std::string> dir = internal::GetFileAbsDir(path);
            std::string dir_tmp;
            if (!internal::ExistFile(path) || !dir.ok() ||!DieDirIfNotExist(*dir, dir_tmp).ok()) {
                return absl::NotFoundError("Path " + path + " does not exist for DieFileIfNotExist.");
            } else {
                out_path = path;
                return absl::OkStatus();
            }
        }

        absl::Status ListDirsInDir(const std::string& path, std::vector<std::string>& sub_dirs) {
            if (!internal::ExistDir(path)) {
                LOG(ERROR) << "Path " << path << " does not exist during function call ExistDir.";
                return absl::NotFoundError("Path " + path + " does not exist for ListDirsInDir.");
            } else {
                absl::StatusOr<std::vector<std::string>> dirs = internal::ListDirsInDir(path);
                if (!dirs.ok()) {
                    return absl::NotFoundError("Input path is not directory or does not exist.");
                }
                sub_dirs.assign((*dirs).begin(), (*dirs).end());
                return absl::OkStatus();
            }
        }


        absl::Status ListFilesInDir(const std::string& path, std::vector<std::string>& sub_files) {
            if (!internal::ExistDir(path)) {
                LOG(ERROR) << "Path " << path << " does not exist during function call ListFilesInDir.";
                return absl::NotFoundError("Path " + path + " does not exist for ListFilesInDir.");
            } else {
                absl::StatusOr<std::vector<std::string>> files = internal::ListFilesInDir(path);
                if (!files.ok()) {
                    return absl::NotFoundError("Input path is not directory or does not exist.");
                }
                sub_files.assign((*files).begin(), (*files).end());
                return absl::OkStatus();
            }
        }


        absl::Status ListAllInDirRecursive(const std::string& path, std::vector<std::string>& sub_dirs,
            std::vector<std::string>& sub_files) {
            if (!internal::ExistDir(path)) {
                LOG(ERROR) << "Path " << path << " does not exist during function call ListDirsInDirRecursive.";
                return absl::NotFoundError("Path " + path + " does not exist for ListDirsInDirRecursive.");
            } else {
                absl::StatusOr<std::vector<std::string>> dirs = internal::ListDirsInDirRecursive(path);
                if (!dirs.ok()) {
                    return absl::NotFoundError("Input path is not directory or does not exist.");
                }
                absl::StatusOr<std::vector<std::string>> files = internal::ListFilesInDirRecursive(path);
                sub_dirs.assign((*dirs).begin(), (*dirs).end());
                if (!files.ok()) {
                    return absl::NotFoundError("Input path is not directory or does not exist.");
                }
                sub_files.assign((*files).begin(), (*files).end());
                return absl::OkStatus();
            }
        }

        absl::Status RmDir(const std::string& path) {
            if (!internal::ExistDir(path)) {
                LOG(ERROR) << "Directory " << path << " does not exist during function call RmDir.";
                return absl::NotFoundError("Path " + path + " does not exist for RmDir.");
            }
            absl::StatusOr<std::vector<std::string>> file_paths = internal::ListFilesInDir(path);
            if (!file_paths.ok()) {
                return absl::NotFoundError("Input path is not directory or does not exist.");
            }

            for (const auto & file_path : *file_paths) {
                if (!RmFile(file_path, true).ok()) {
                    return absl::InternalError("Removing directory " + file_path + " failed.");
                }
            }
            if (rmdir(path.c_str()) == 0) {
                VLOG(1) << "Removed directory " << path << ".";
                return absl::OkStatus();
            } else {
                LOG(ERROR)  << "Removing directory " << path << " failed during functionn call RmDir.";
                return absl::InternalError("Removing directory " + path + " failed.");
            }
        }

        absl::Status RmDirRecursive(const std::string& path) {
            absl::StatusOr<std::vector<std::string>> dirs_path = internal::ListDirsInDir(path);
            if (!dirs_path.ok()) {
                return absl::NotFoundError("Input path is not directory or does not exist.");
            }

            if (!(*dirs_path).empty()) {
                for (const auto& dir_path : *dirs_path) {
                    if (!RmDirRecursive(dir_path).ok()) {
                        return absl::InternalError("Recursively removing directory " + dir_path + " failed.");
                    }
                }
            }
            return RmDir(path);
        }

        void LockFile(const std::string& lock_name) {
            while (internal::ExistFile(lock_name)) {
                absl::SleepFor(absl::Milliseconds(galaxy::constant::kLockRetrySec));
            }
            CHECK(CreateFileIfNotExist(lock_name, 0777).ok()) << "Creating lock for " + lock_name + " failed.";
        }

        void UnlockFile(const std::string& lock_name) {
            CHECK(internal::ExistFile(lock_name)) << "Lock file does not exist.";
            CHECK(RmFile(lock_name, false).ok()) << "Removing lock for " + lock_name + " failed.";
        }

        void Lock(const std::string& path) {
            absl::StatusOr<std::string> lock_name = internal::GetFileLockName(path);
            CHECK(lock_name.ok()) << "Fail to create lock file.";
            LockFile(*lock_name);
        }

        void Unlock(const std::string& path) {
            absl::StatusOr<std::string> lock_name = internal::GetFileLockName(path);
            CHECK(lock_name.ok()) << "Fail to create lock file.";
            UnlockFile(*lock_name);
        }

        absl::Status RmFile(const std::string& path, bool require_lock) {
            if (!internal::ExistFile(path)) {
                LOG(ERROR) << "File " << path << " does not exist during function call RmFile.";
                return absl::NotFoundError("Path " + path + " does not exist for RmFile.");
            } else {
                absl::StatusOr<std::string> lock_name = internal::GetFileLockName(path);
                if (!lock_name.ok()) {
                    return absl::InternalError("Fail to create lock file.");
                }
                if (require_lock) {
                    LockFile(*lock_name);
                }
                int status = remove(path.c_str());
                if (require_lock) {
                    UnlockFile(*lock_name);
                }
                if ( status!= 0) {
                    LOG(ERROR) << "Removing file " << path << " failed during function call RmFile";
                    return absl::InternalError("Removing file " + path + " failed.");
                } else {
                    VLOG(1) << "Removed file " << path << ".";
                    return absl::OkStatus();
                }
            }
        }

        absl::Status RenameFile(const std::string& old_path, const std::string& new_path) {
            if (!internal::ExistFile(old_path)) {
                return absl::NotFoundError("Path " + old_path + " does not exist for RenameFile.");
            }
            absl::StatusOr<std::string> old_lock_name = internal::GetFileLockName(old_path);
            absl::StatusOr<std::string> new_lock_name = internal::GetFileLockName(new_path);
            if (!old_lock_name.ok() || !new_lock_name.ok()) {
                return absl::InternalError("Fail to create lock file.");
            }
            LockFile(*old_lock_name);
            LockFile(*new_lock_name);
            int status = rename(old_path.c_str(), new_path.c_str());
            UnlockFile(*old_lock_name);
            UnlockFile(*new_lock_name);
            if (status == 0) {
                VLOG(1) << "Renamed from " << old_path << " to " << new_path << ".";
                return absl::OkStatus();
            } else{
                LOG(ERROR) << "Renaming file " << old_path << " failed during function call RenameFile.";
                return absl::InternalError("Renaming file " + old_path + " failed.");
            }
        }

        absl::Status Read(const std::string& path, std::string& data) {
            if (!internal::ExistFile(path)) {
                return absl::NotFoundError("Path " + path + " does not exist for Read.");
            }
            absl::StatusOr<std::string> lock_name = internal::GetFileLockName(path);
            if (!lock_name.ok()) {
                return absl::InternalError("Fail to create lock file.");
            }
            LockFile(*lock_name);
            std::ifstream infile(path);
            std::string out((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
            data = out;
            UnlockFile(*lock_name);
            return absl::OkStatus();
        }

        absl::Status Write(const std::string& path, const std::string& data, const std::string& mode, bool require_lock) {
            absl::StatusOr<std::string> lock_name;
            if (require_lock) {
                lock_name = internal::GetFileLockName(path);
                if (!lock_name.ok()) {
                    return absl::InternalError("Fail to create lock file.");
                }
                LockFile(*lock_name);
            }

            if (!internal::ExistFile(path)) {
                VLOG(1) << "Creating file " << path << ".";
                if (!CreateFileIfNotExist(path, 0777).ok()) {
                    return absl::InternalError("Creating file " + path + " failed.");
                }
            }
            std::ofstream outfile;
            if (mode == "a") {
                outfile.open(path, std::ofstream::app | std::ofstream::binary);
            } else {
                outfile.open(path, std::ofstream::binary);
            }
            outfile << data;
            outfile.close();
            if (require_lock) {
                UnlockFile(*lock_name);
            }
            return absl::OkStatus();
        }

        absl::Status GetAttr(const std::string& path, struct stat *statbuf) {
            if (lstat(path.c_str(), statbuf) == 0) {
                return absl::OkStatus();
            } else {
                return absl::InvalidArgumentError("GetAttr failed for " + path + ".");
            }
        }
    }
}
