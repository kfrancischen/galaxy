
#include "cpp/internal/galaxy_fs_internal.h"

#include "absl/flags/flag.h"
#include "cpp/core/galaxy_fs.h"
#include "cpp/core/galaxy_flag.h"

namespace galaxy {

    GalaxyFs::GalaxyFs(const std::string& root) : root_(root){};
    GalaxyFs::~GalaxyFs(){};

    std::unique_ptr<GalaxyFs> GalaxyFs::Instance() {
        return std::unique_ptr<GalaxyFs>(new GalaxyFs(absl::GetFlag(FLAGS_fs_root)));
    }

    void GalaxyFs::SetRootDir(const std::string& path) {
        root_ = path;
        return;
    }

    int GalaxyFs::CreateDirIfNotExist(const std::string& path, mode_t mode) {
        std::string abs_path = internal::JoinPath(root_, path);
        return galaxy::impl::CreateDirIfNotExist(abs_path, mode);

    }
    int GalaxyFs::DieDirIfNotExist(const std::string& path, std::string& out_path) {
        std::string abs_path = internal::JoinPath(root_, path);
        return galaxy::impl::DieDirIfNotExist(abs_path, out_path);
    }

    int GalaxyFs::CreateFileIfNotExist(const std::string& path, mode_t mode) {
        std::string abs_path = internal::JoinPath(root_, path);
        return galaxy::impl::CreateFileIfNotExist(abs_path, mode);

    }
    int GalaxyFs::DieFileIfNotExist(const std::string& path, std::string& out_path) {
        std::string abs_path = internal::JoinPath(root_, path);
        return galaxy::impl::DieFileIfNotExist(abs_path, out_path);

    }

    int GalaxyFs::ListDirsInDir(const std::string& path, std::vector<std::string>& sub_dirs) {
        std::string abs_path = internal::JoinPath(root_, path);
        int return_val = galaxy::impl::ListDirsInDir(abs_path, sub_dirs);
        return return_val;
    }

    int GalaxyFs::ListFilesInDir(const std::string& path, std::vector<std::string>& sub_files) {
        std::string abs_path = internal::JoinPath(root_, path);
        int return_val = galaxy::impl::ListFilesInDir(abs_path, sub_files);
        return return_val;
    }

    int GalaxyFs::RmDir(const std::string& path) {
        std::string abs_path = internal::JoinPath(root_, path);
        return galaxy::impl::RmDir(abs_path);

    }

    int GalaxyFs::RmDirRecursive(const std::string& path) {
        std::string abs_path = internal::JoinPath(root_, path);
        return galaxy::impl::RmDirRecursive(abs_path);

    }

    int GalaxyFs::RmFile(const std::string& path){
        std::string abs_path = internal::JoinPath(root_, path);
        return galaxy::impl::RmFile(abs_path, true);

    }

    int GalaxyFs::RenameFile(const std::string& old_path, const std::string& new_path) {
        std::string abs_old_path = internal::JoinPath(root_, old_path);
        std::string abs_new_path = internal::JoinPath(root_, new_path);
        return galaxy::impl::RenameFile(abs_old_path, abs_new_path);

    }

    int GalaxyFs::Read(const std::string& path, std::string& data) {
        std::string abs_path = internal::JoinPath(root_, path);
        return galaxy::impl::Read(abs_path, data);
    }

    int GalaxyFs::Write(const std::string& path, const std::string& data) {
        std::string abs_path = internal::JoinPath(root_, path);
        return galaxy::impl::Write(abs_path, data);
    }

    int GalaxyFs::GetAttr(const std::string& path, struct stat *statbuf) {
        std::string abs_path = internal::JoinPath(root_, path);
        return galaxy::impl::GetAttr(abs_path, statbuf);
    }
}
