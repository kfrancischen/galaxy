#ifndef CPP_CORE_GALAXY_CONST_H_
#define CPP_CORE_GALAXY_CONST_H_

namespace galaxy {
    namespace constant {
        constexpr char kSeparator = '/';
        constexpr char kCellSuffix[] = "-d";
        constexpr char kCellPrefix[] = "/galaxy";
        constexpr char kLockNameTemplate[] = ".$0.lock";
        constexpr char kLocalPrefix[] = "/LOCAL";
        constexpr int kChunkSize = 1048576;  // 1MB
        constexpr int kLockRetrySec = 1;
    }  // namespace const
}  // namespace galaxy

#endif  //CPP_CORE_GALAXY_CONST_H_