/* Example cmd
* GALAXY_fs_global_config=/home/pslx/galaxy/example/cpp/server_config_example.json \
* bazel run -c opt //example/cpp:client_example -- --proto_test=/galaxy/aa-d/Downloads/test1/test.pb
*/

#include <iostream>
#include <string>
#include <vector>

#include "cpp/client.h"
#include "cpp/core/galaxy_flag.h"
#include "glog/logging.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "schema/fileserver.pb.h"

ABSL_FLAG(std::string, mkdir_test, "", "The directory for mkdir test.");
ABSL_FLAG(std::string, rmdir_test, "", "The directory for rmdir test.");
ABSL_FLAG(std::string, createfile_test, "", "The directory for createfile test.");
ABSL_FLAG(std::string, proto_test, "", "The directory for createfile test.");

int main(int argc, char* argv[]) {
    absl::ParseCommandLine(argc, argv);
    FLAGS_log_dir = absl::GetFlag(FLAGS_fs_log_dir);
    google::InitGoogleLogging(argv[0]);
    if (!absl::GetFlag(FLAGS_mkdir_test).empty()) {
        galaxy::client::CreateDirIfNotExist(absl::GetFlag(FLAGS_mkdir_test));
    }
    if (!absl::GetFlag(FLAGS_rmdir_test).empty()) {
        galaxy::client::RmDir(absl::GetFlag(FLAGS_rmdir_test));
    }
    if (!absl::GetFlag(FLAGS_createfile_test).empty()) {
        galaxy::client::CreateFileIfNotExist(absl::GetFlag(FLAGS_createfile_test));
        galaxy::client::Write(absl::GetFlag(FLAGS_createfile_test), "hello world");
        // std::cout << galaxy::client::Read(absl::GetFlag(FLAGS_createfile_test)) << std::endl;
        std::cout << galaxy::client::ReadLarge(absl::GetFlag(FLAGS_createfile_test)) << std::endl;
        std::string data = galaxy::client::ReadLarge("/galaxy/aa-d/Downloads/historical_stock_data/market/Stocks/BABA.txt");
        galaxy::client::WriteLarge("/galaxy/aa-d/Downloads/large_test.txt", data);
        std::cout << galaxy::client::Read("/galaxy/aa-d/some_random_file") << std::endl;
    }
    if (!absl::GetFlag(FLAGS_proto_test).empty()) {
        galaxy_schema::Credential cred;
        cred.set_password("test");
        std::string cred_str;
        cred.SerializeToString(&cred_str);
        galaxy::client::Write(absl::GetFlag(FLAGS_proto_test), cred_str);
        std::string result = galaxy::client::Read(absl::GetFlag(FLAGS_proto_test));
        galaxy_schema::Credential result_cred;
        result_cred.ParseFromString(result);
        std::cout << result_cred.DebugString() << std::endl;
    }

    return 0;
}