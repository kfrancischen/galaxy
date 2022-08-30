#ifndef GALAXY_EXT_TTL_CLEANER_GALAXY_TTL_CLEANER_H_
#define GALAXY_EXT_TTL_CLEANER_GALAXY_TTL_CLEANER_H_

#include <ctime>
#include <string>

namespace galaxy {
    namespace ext {
        constexpr char kExcludedDir[] = "GALAXY_LOG";

        std::string GetTTLFromPath(const std::string& path);
        std::time_t GetFileModifiedTime(const std::string& path);
        std::time_t GetCurrentTime();
        double GetTTLTime(std::string ttl);
    }
}


#endif  // GALAXY_EXT_TTL_CLEANER_GALAXY_TTL_CLEANER_H_