#ifndef CPP_TOOL_PM2_CLI_H_
#define CPP_TOOL_PM2_CLI_H_

#include <grpcpp/grpcpp.h>

namespace galaxy
{
    void Pm2List(const std::string &cell);
    void Pm2List();

    void Pm2Start(const std::string &cell, const std::string &home_dir, const std::string &json_file, const std::string &job_name);

    void Pm2Stop(const std::string &cell, const std::string &job_name);
    void Pm2Stop(const std::string &job_name);

    void Pm2Restart(const std::string &cell, const std::string &job_name);
    void Pm2Restart(const std::string &job_name);
}  // namespace galaxy



#endif  // CPP_TOOL_FILEUTIL_H_