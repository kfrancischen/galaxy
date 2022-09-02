#include "cpp/tool/pm2_cli.h"
#include <iostream>
#include <fstream>
#include <string>
#include "absl/flags/flag.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/client.h"
#include "glog/logging.h"

namespace galaxy
{

    void Pm2List(const std::string &cell)
    {
        client::RemoteExecute(cell, "", "pm2", {"list"});
    }

    void Pm2List()
    {
        std::vector<std::string> cells = client::ListCells();
        for (const auto& cell : cells) {
            std::cout << "Checking cell: [" << cell << "]" << std::endl;
            Pm2List(cell);
        }
    }

    void Pm2Start(const std::string &cell, const std::string &home_dir, const std::string &json_file, const std::string &job_name)
    {
        client::RemoteExecute(cell, home_dir, "pm2", {"start", json_file, "--only", job_name});
    }

    void Pm2Stop(const std::string &cell, const std::string &job_name)
    {
        client::RemoteExecute(cell, "", "pm2", {"stop", job_name});
    }

    void Pm2Stop(const std::string &job_name) {
        std::vector<std::string> cells = client::ListCells();
        for (const auto& cell : cells) {
            std::cout << "Checking cell: [" << cell << "]" << std::endl;
            Pm2Stop(cell, job_name);
        }
    }

    void Pm2Restart(const std::string &cell, const std::string &job_name)
    {
        client::RemoteExecute(cell, "", "pm2", {"restart", job_name});
    }

    void Pm2Restart(const std::string &job_name) {
        std::vector<std::string> cells = client::ListCells();
        for (const auto& cell : cells) {
            std::cout << "Checking cell: [" << cell << "]" << std::endl;
            Pm2Restart(cell, job_name);
        }
    }
}
