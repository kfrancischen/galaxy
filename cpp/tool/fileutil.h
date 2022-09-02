#ifndef CPP_TOOL_FILEUTIL_H_
#define CPP_TOOL_FILEUTIL_H_

#include <grpcpp/grpcpp.h>

namespace galaxy
{

    void LsCmd(const std::string& path);
    void RmCmd(const std::string& path, bool recursive);

    void CopyCmd(const std::string& from_path, const std::string& to_path, bool overwrite);
    void MoveCmd(const std::string& from_path, const std::string& to_path, bool overwrite);
    void ListCellsCmd();
} // namespace galaxy

#endif // CPP_TOOL_FILEUTIL_H_