#ifndef CPP_CORE_GALAXY_FS_H_
#define CPP_CORE_GALAXY_FS_H_

#include <string>
#include <memory>

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

        int CreateDirIfNotExist(const std::string& path, mode_t mode=0777);
        int DieDirIfNotExist(const std::string& path, mode_t mode=0777);

        int CreateFileIfNotExist(const std::string& path, mode_t mode=0777);
        int DieFileIfNotExist(const std::string& path, mode_t mode=0777);

        int RmDir(const std::string& path);
        int RmDirRecursive(const std::string& path);
        int RmFile(const std::string& path);
        int RenameFile(const std::string& old_path, const std::string& new_path);

        int Read(const std::string& path, std::string& data);
        int Write(const std::string& path, const std::string& data);

    private:
        std::string root_;
    };
} //  namespace galaxy.
#endif   //  CPP_CORE_GALAXY_FS_H_
