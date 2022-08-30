#include <cstdlib>
#include "absl/flags/flag.h"
#include "cpp/core/galaxy_flag.h"

#define EnvToString(envname, dflt)   \
  (!getenv(envname) ? (dflt) : getenv(envname))

#define EnvToInt(envname, dflt)  \
  (!getenv(envname) ? (dflt) : strtol(getenv(envname), NULL, 10))

#define EnvToBool(envname, dflt)   \
  (!getenv(envname) ? (dflt) : memchr("tTyY1\0", getenv(envname)[0], 6) != NULL)

#define GALAXY_DEFINE_string(name, value, meaning) \
  ABSL_FLAG(std::string, name, EnvToString("GALAXY_" #name, value), meaning)

#define GALAXY_DEFINE_int(name, value, meaning) \
  ABSL_FLAG(int, name, EnvToInt("GALAXY_" #name, value), meaning)

#define GALAXY_DEFINE_bool(name, value, meaning) \
  ABSL_FLAG(bool, name, EnvToInt("GALAXY_" #name, value), meaning)

GALAXY_DEFINE_int(fs_rpc_ddl, 10, "The deadline for grpc in seconds.");
// Global configurations
GALAXY_DEFINE_string(fs_global_config, "", "The global configuration (json file) for galaxy filesystems.");
GALAXY_DEFINE_string(fs_cell, "", "Current cell of the galaxy filesystems.");
