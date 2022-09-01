
#include "cpp/internal/galaxy_fs_internal.h"

#include "absl/flags/flag.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/core/galaxy_fs.h"

namespace galaxy {

    GalaxyFs::GalaxyFs(const std::string& root) : root_(root){};
    GalaxyFs::~GalaxyFs(){};

    std::unique_ptr<GalaxyFs> GalaxyFs::Instance(std::string root) {
        return std::unique_ptr<GalaxyFs>(new GalaxyFs(root));
    }

    void GalaxyFs::SetRootDir(const std::string& path) {
        root_ = path;
        return;
    }

    absl::Status GalaxyFs::CreateDirIfNotExist(const std::string& path, mode_t mode) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::CreateDirIfNotExist(abs_path, mode);
    }

    absl::Status GalaxyFs::CopyFile(const std::string& from_path, const std::string& to_path) {
        std::string abs_from_path = internal::JoinPath(root_, from_path);
        std::string abs_to_path = internal::JoinPath(root_, to_path);
        return impl::CopyFile(abs_from_path, abs_to_path);
    }

    absl::Status GalaxyFs::MoveFile(const std::string& from_path, const std::string& to_path) {
        std::string abs_from_path = internal::JoinPath(root_, from_path);
        std::string abs_to_path = internal::JoinPath(root_, to_path);
        return impl::MoveFile(abs_from_path, abs_to_path);
    }

    absl::Status GalaxyFs::DieDirIfNotExist(const std::string& path, std::string& out_path) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::DieDirIfNotExist(abs_path, out_path);
    }

    absl::Status GalaxyFs::CreateFileIfNotExist(const std::string& path, mode_t mode) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::CreateFileIfNotExist(abs_path, mode);

    }
    absl::Status GalaxyFs::DieFileIfNotExist(const std::string& path, std::string& out_path) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::DieFileIfNotExist(abs_path, out_path);

    }

    absl::Status GalaxyFs::ListDirsInDir(const std::string& path, absl::flat_hash_map<std::string, struct stat>& sub_dirs) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::ListDirsInDir(abs_path, sub_dirs);
    }

    absl::Status GalaxyFs::ListFilesInDir(const std::string& path, absl::flat_hash_map<std::string, struct stat>& sub_files, bool include_hidden) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::ListFilesInDir(abs_path, sub_files, include_hidden);
    }

    absl::Status GalaxyFs::ListAllInDirRecursive(const std::string& path, absl::flat_hash_map<std::string, struct stat>& sub_dirs,
            absl::flat_hash_map<std::string, struct stat>& sub_files, bool include_hidden) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::ListAllInDirRecursive(abs_path, sub_dirs, sub_files, include_hidden);
    }

    absl::Status GalaxyFs::RmDir(const std::string& path, bool include_hidden) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::RmDir(abs_path, include_hidden);

    }

    absl::Status GalaxyFs::RmDirRecursive(const std::string& path, bool include_hidden) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::RmDirRecursive(abs_path, include_hidden);

    }

    absl::Status GalaxyFs::RmFile(const std::string& path, bool require_lock){
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::RmFile(abs_path, require_lock);

    }

    absl::Status GalaxyFs::RenameFile(const std::string& old_path, const std::string& new_path) {
        std::string abs_old_path = internal::JoinPath(root_, old_path);
        std::string abs_new_path = internal::JoinPath(root_, new_path);
        return impl::RenameFile(abs_old_path, abs_new_path);

    }

    absl::Status GalaxyFs::Read(const std::string& path, std::string& data) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::Read(abs_path, data);
    }

    absl::Status GalaxyFs::Write(const std::string& path, const std::string& data, const std::string& mode, bool require_lock) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::Write(abs_path, data, mode, require_lock);
    }

    absl::Status GalaxyFs::GetAttr(const std::string& path, struct stat *statbuf) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::GetAttr(abs_path, statbuf);
    }

    void GalaxyFs::Lock(const std::string& path) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::Lock(abs_path);
    }

    void GalaxyFs::Unlock(const std::string& path) {
        std::string abs_path = internal::JoinPath(root_, path);
        return impl::Unlock(abs_path);
    }

    absl::Status GalaxyFs::GetDiskUsage(struct statvfs *statvfsbuf) {
        return impl::GetDiskUsage(statvfsbuf);
    }

    absl::Status GalaxyFs::GetRamUsage(struct sysinfo *sysinfobuf) {
        return impl::GetRamUsage(sysinfobuf);
    }
}
