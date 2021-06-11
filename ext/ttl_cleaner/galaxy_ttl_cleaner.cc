#include <regex>
#include <cctype>
#include <clocale>
#include <chrono>
#include <algorithm>
#include "ext/ttl_cleaner/galaxy_ttl_cleaner.h"
#include "cpp/core/galaxy_fs.h"

std::string galaxy::ext::GetTTLFromPath(const std::string& path) {
    std::string path_lower(path);
    std::transform(path_lower.begin(), path_lower.end(), path_lower.begin(),
                    [](unsigned char c){ return std::tolower(c); });
    std::regex reg("ttl=[A-Za-z0-9]+");
    std::smatch sm;
    regex_search(path_lower, sm, reg);
    for (size_t i = 0; i < sm.size(); i++) {
        std::string matched_ttl = sm[i].str();
        matched_ttl.erase(matched_ttl.begin(), matched_ttl.begin() + 4);
        return matched_ttl;
    }
    return "";
}


std::time_t galaxy::ext::GetFileModifiedTime(const std::string& path) {
    GalaxyFs fs("");
    struct stat statbuf;
    auto status = fs.GetAttr(path, &statbuf);
    if (!status.ok()) {
        return -1;
    }
    return statbuf.st_mtime;
}

std::time_t galaxy::ext::GetCurrentTime() {
    auto cur_time = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(cur_time);
}

double galaxy::ext::GetTTLTime(std::string ttl)
{
    if (ttl.empty()) {
        return -1.0;
    }
    std::string ch(1, ttl.back());
    int multiplier;
    if (ch == "m") {
        multiplier = 60;
    } else if (ch == "h") {
        multiplier = 1440; // 60 * 24
    } else if (ch == "d") {
        multiplier = 86400; // 60 * 60 * 24
    } else {
        return -1.0;
    }
    ttl.pop_back();
    int unit = std::stoi(ttl);
    if (unit <= 0) {
        return -1.0;
    }
    return 1.0 * unit * multiplier;
}
