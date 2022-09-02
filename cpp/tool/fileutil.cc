#include "cpp/tool/fileutil.h"

#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "absl/flags/flag.h"
#include "glog/logging.h"
#include "cpp/client.h"

namespace galaxy
{

    void LsCmd(const std::string& path) {
        std::map<std::string, std::string> sub_dirs = client::ListDirsInDir(path);
        std::map<std::string, std::string> sub_files = client::ListFilesInDir(path);
        for (const auto& dir : sub_dirs) {
            std::cout << "\tDirectory Entry: " << dir.first << std::endl;
        }
        for (const auto& file : sub_files) {
            std::cout << "\tFile Entry:      " << file.first << std::endl;
        }
    }

    void RmCmd(const std::string& path, bool recursive) {
        if (recursive) {
            client::RmDirRecursive(path);
        } else {
            client::RmDir(path);
        }
        client::RmFile(path);
        std::cout << "Done removing " << path << std::endl;
    }

    void ListCellsCmd() {
        std::vector<std::string> cells = client::ListCells();
        std::cout << "In total " << cells.size() << " cells:" << std::endl;
        for (const auto& cell : cells) {
            std::cout << "\t[cell]: " << cell << std::endl;
        }
    }


    void CopyFileCmd(const std::string& from_path, const std::string& to_path, bool overwrite) {
        if (!overwrite && !galaxy::client::FileOrDie(to_path).empty()) {
            LOG(WARNING) << "File already exist. Pass.";
            return;
        }
        galaxy::client::CopyFile(from_path, to_path);
    }

    void MoveFileCmd(const std::string& from_path, const std::string& to_path, bool overwrite) {
        if (!overwrite && !galaxy::client::FileOrDie(to_path).empty()) {
            LOG(WARNING) << "File already exist. Pass";
            return;
        }
        galaxy::client::MoveFile(from_path, to_path);
    }

    void CopyDirCmd(const std::string& from_path, const std::string& to_path, bool overwrite) {
        try {
            std::map<std::string, std::string> sub_files = client::ListFilesInDirRecursive(from_path);
            // Needs to push back from_path in case from_path is a file name.
            std::vector<std::string> all_files;
            for (const auto& sub_file : sub_files) {
                all_files.push_back(sub_file.first);
            }
            if (all_files.empty()) {
                all_files.push_back(from_path);
            }
            std::vector<std::thread> threads;
            for (auto& file : all_files) {
                std::string new_file(file);
                new_file.replace(0, from_path.length(), to_path);
                threads.push_back(std::thread(CopyFileCmd, file, new_file, overwrite));
            }

            for (auto& th : threads) {
                th.join();
            }
        }
        catch (std::string errorMsg)
        {
            LOG(FATAL) << errorMsg;
        }
    }

    void MoveDirCmd(const std::string& from_path, const std::string& to_path, bool overwrite) {
        try {
            CopyDirCmd(from_path, to_path, overwrite);
            client::RmDirRecursive(from_path);
            client::RmFile(from_path);
            std::cout << "Done moving from " << from_path << " to " << to_path << std::endl;
        }
        catch (std::string errorMsg)
        {
            LOG(FATAL) << errorMsg;
        }
    }
}
