#ifndef CPP_CORE_GALAXY_CONST_H_
#define CPP_CORE_GALAXY_CONST_H_

namespace galaxy {
    namespace constant {
        constexpr char kSeparator = '/';
        constexpr char kCellSuffix[] = "-d";
        constexpr char kCellPrefix[] = "/galaxy";
        constexpr char kLockNameTemplate[] = ".$0.lock";
        constexpr int kChunkSize = 1048576;  // 1024 * 1024
    }  // namespace const
}  // namespace galaxy

#endif  //CPP_CORE_GALAXY_CONST_H_