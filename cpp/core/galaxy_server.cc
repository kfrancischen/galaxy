#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <array>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <sys/types.h>

#include "absl/container/flat_hash_map.h"
#include "absl/time/clock.h"
#include "glog/logging.h"
#include "cpp/client.h"
#include "cpp/core/galaxy_fs.h"
#include "cpp/core/galaxy_server.h"
#include "cpp/internal/galaxy_const.h"
#include "cpp/internal/galaxy_stats_internal.h"

using grpc::ServerContext;
using grpc::ServerReader;
using grpc::Status;
using grpc::StatusCode;

using galaxy_schema::Attribute;
using galaxy_schema::Credential;
using galaxy_schema::FileSystemStatus;
using galaxy_schema::FileSystemUsage;
using galaxy_schema::Owner;
using galaxy_schema::WriteMode;
using galaxy_schema::CrossCellCallType;

using galaxy_schema::CreateDirRequest;
using galaxy_schema::CreateDirResponse;
using galaxy_schema::CreateFileRequest;
using galaxy_schema::CreateFileResponse;
using galaxy_schema::DirOrDieRequest;
using galaxy_schema::DirOrDieResponse;
using galaxy_schema::CopyRequest;
using galaxy_schema::CopyResponse;
using galaxy_schema::CrossCellRequest;
using galaxy_schema::CrossCellResponse;
using galaxy_schema::FileOrDieRequest;
using galaxy_schema::FileOrDieResponse;
using galaxy_schema::GetAttrRequest;
using galaxy_schema::GetAttrResponse;
using galaxy_schema::ListAllInDirRecursiveRequest;
using galaxy_schema::ListAllInDirRecursiveResponse;
using galaxy_schema::ListDirsInDirRequest;
using galaxy_schema::ListDirsInDirResponse;
using galaxy_schema::ListFilesInDirRequest;
using galaxy_schema::ListFilesInDirResponse;
using galaxy_schema::ReadMultipleRequest;
using galaxy_schema::ReadMultipleResponse;
using galaxy_schema::ReadRequest;
using galaxy_schema::ReadResponse;
using galaxy_schema::RenameFileRequest;
using galaxy_schema::RenameFileResponse;
using galaxy_schema::RmDirRecursiveRequest;
using galaxy_schema::RmDirRecursiveResponse;
using galaxy_schema::RmDirRequest;
using galaxy_schema::RmDirResponse;
using galaxy_schema::RmFileRequest;
using galaxy_schema::RmFileResponse;
using galaxy_schema::WriteRequest;
using galaxy_schema::WriteResponse;
using galaxy_schema::WriteMultipleRequest;
using galaxy_schema::WriteMultipleResponse;
using galaxy_schema::HealthCheckRequest;
using galaxy_schema::HealthCheckResponse;
using galaxy_schema::RemoteExecutionRequest;
using galaxy_schema::RemoteExecutionResponse;

namespace galaxy
{
    Attribute StatbufToAttribute(const struct stat& statbuf) {
        Owner owner;
        owner.set_uid(statbuf.st_uid);
        owner.set_gid(statbuf.st_gid);

        Attribute attribute;
        attribute.set_dev(statbuf.st_dev);
        attribute.set_ino(statbuf.st_ino);
        attribute.set_mode(statbuf.st_mode);
        attribute.set_nlink(statbuf.st_nlink);
        attribute.mutable_owner()->CopyFrom(owner);
        attribute.set_rdev(statbuf.st_rdev);
        attribute.set_size(statbuf.st_size);
        attribute.set_blksize(statbuf.st_blksize);
        attribute.set_blocks(statbuf.st_blocks);
        attribute.set_atime(statbuf.st_atime);
        attribute.set_mtime(statbuf.st_mtime);
        attribute.set_ctime(statbuf.st_ctime);
        return attribute;
    }

    void GalaxyServerImpl::SetPassword(const std::string &password)
    {
        password_ = password;
    }

    absl::Status GalaxyServerImpl::VerifyPassword(const Credential &cred)
    {
        if (cred.password() != password_)
        {
            return absl::PermissionDeniedError("Wrong password.");
        }
        else
        {
            return absl::OkStatus();
        }
    }

    Status GalaxyServerImpl::GetAttrInternal(ServerContext *context, const GetAttrRequest *request,
                                             GetAttrResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call GetAttr";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call GetAttr.");
        }
        struct stat statbuf;
        std::string path = request->name();
        absl::Status fs_status = GalaxyFs::Instance()->GetAttr(path, &statbuf);
        if (!fs_status.ok())
        {
            LOG(ERROR) << "GetAttr failed during function call GetAttr with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_attr()->CopyFrom(StatbufToAttribute(statbuf));
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::CreateDirIfNotExistInternal(ServerContext *context, const CreateDirRequest *request,
                                                         CreateDirResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call CreateDirIfNotExist.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->CreateDirIfNotExist(request->name(), request->mode());
        if (!fs_status.ok())
        {
            LOG(ERROR) << "CreateDirIfNotExist failed during function call CreateDirIfNotExist with error" << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::DirOrDieInternal(ServerContext *context, const DirOrDieRequest *request,
                                              DirOrDieResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call DirOrDie.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call DirOrDie.");
        }
        std::string out_path;
        absl::Status fs_status = GalaxyFs::Instance()->DieDirIfNotExist(request->name(), out_path);
        if (!fs_status.ok())
        {
            LOG(ERROR) << "DieDirIfNotExist failed during function call DirOrDie with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            reply->set_name(out_path);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::RmDirInternal(ServerContext *context, const RmDirRequest *request,
                                           RmDirResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call RmDir.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call RmDir.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->RmDir(request->name(), request->include_hidden());
        if (!fs_status.ok())
        {
            LOG(ERROR) << "RmDir failed during function call RmDir with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::RmDirRecursiveInternal(ServerContext *context, const RmDirRecursiveRequest *request,
                                                    RmDirRecursiveResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call RmDirRecursive.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call RmDirRecursive.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->RmDirRecursive(request->name(), request->include_hidden());
        if (!fs_status.ok())
        {
            LOG(ERROR) << "RmDirRecursive failed during function call RmDirRecursive with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::ListDirsInDirInternal(ServerContext *context, const ListDirsInDirRequest *request,
                                                   ListDirsInDirResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call ListDirsInDir.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call ListDirsInDir.");
        }
        absl::flat_hash_map<std::string, struct stat> dirs;
        absl::Status fs_status = GalaxyFs::Instance()->ListDirsInDir(request->name(), dirs);
        if (!fs_status.ok())
        {
            LOG(ERROR) << "ListDirsInDir failed during function call ListDirsInDir with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            for (const auto& dir : dirs) {
                (*reply->mutable_sub_dirs())[dir.first].CopyFrom(StatbufToAttribute(dir.second));
            }
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::ListFilesInDirInternal(ServerContext *context, const ListFilesInDirRequest *request,
                                                    ListFilesInDirResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call ListFilesInDir.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call ListFilesInDir.");
        }
        absl::flat_hash_map<std::string, struct stat> files;
        absl::Status fs_status = GalaxyFs::Instance()->ListFilesInDir(request->name(), files, request->include_hidden());
        if (!fs_status.ok())
        {
            LOG(ERROR) << "ListFilesInDir failed during function call ListFilesInDir with error" << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            for (const auto& file : files) {
                (*reply->mutable_sub_files())[file.first].CopyFrom(StatbufToAttribute(file.second));
            }
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::ListAllInDirRecursiveInternal(ServerContext *context, const ListAllInDirRecursiveRequest *request,
                                                           ListAllInDirRecursiveResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call ListAllInDirRecursive.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call ListAllInDirRecursive.");
        }
        absl::flat_hash_map<std::string, struct stat> files;
        absl::flat_hash_map<std::string, struct stat> dirs;
        absl::Status fs_status = GalaxyFs::Instance()->ListAllInDirRecursive(request->name(), dirs, files, request->include_hidden());
        if (!fs_status.ok())
        {
            LOG(ERROR) << "ListFilesInDir failed during function call ListAllInDirRecursive with error" << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            for (const auto& dir : dirs) {
                (*reply->mutable_sub_dirs())[dir.first].CopyFrom(StatbufToAttribute(dir.second));
            }

            for (const auto& file : files) {
                (*reply->mutable_sub_files())[file.first].CopyFrom(StatbufToAttribute(file.second));
            }
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::CreateFileIfNotExistInternal(ServerContext *context, const CreateFileRequest *request,
                                                          CreateFileResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call CreateFileIfNotExist.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call CreateFileIfNotExist.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->CreateFileIfNotExist(request->name(), request->mode());
        if (!fs_status.ok())
        {
            LOG(ERROR) << "CreateFileIfNotExist failed during function call CreateFileIfNotExist with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::FileOrDieInternal(ServerContext *context, const FileOrDieRequest *request,
                                               FileOrDieResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call FileOrDie.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call FileOrDie.");
        }
        std::string out_path;
        absl::Status fs_status = GalaxyFs::Instance()->DieFileIfNotExist(request->name(), out_path);
        if (!fs_status.ok())
        {
            LOG(ERROR) << "DieFileIfNotExist failed during function call FileOrDie with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            reply->set_name(out_path);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::RmFileInternal(ServerContext *context, const RmFileRequest *request,
                                            RmFileResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call RmFile.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call RmFile.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->RmFile(request->name(), !request->is_hidden());
        if (!fs_status.ok())
        {
            LOG(ERROR) << "RmFile failed during function call RmFile with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::RenameFileInternal(ServerContext *context, const RenameFileRequest *request,
                                                RenameFileResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call RenameFile.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call RenameFile.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->RenameFile(request->old_name(), request->new_name());
        if (!fs_status.ok())
        {
            LOG(ERROR) << "RenameFile failed during function call RenameFile with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::ReadInternal(ServerContext *context, const ReadRequest *request,
                                          ReadResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client client during function call Read.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call Read.");
        }
        std::string data;
        absl::Status fs_status = GalaxyFs::Instance()->Read(request->name(), data);
        if (!fs_status.ok())
        {
            LOG(ERROR) << "Read failed client during function call Read with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            reply->set_data(data);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::ReadMultipleInternal(ServerContext *context, const ReadMultipleRequest *request,
                                                  ReadMultipleResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client client during function call ReadMultiple.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call ReadMultiple.");
        }
        for (const auto &path : request->names())
        {
            ReadResponse read_response;
            std::string abs_path;
            absl::Status absl_status = GalaxyFs::Instance()->DieFileIfNotExist(path, abs_path);
            if (absl_status.ok())
            {
                ReadRequest read_request;
                read_request.set_name(path);
                read_request.mutable_cred()->CopyFrom(request->cred());
                Status status = GalaxyServerImpl::ReadInternal(context, &read_request, &read_response);
                if (!status.ok())
                {
                    LOG(ERROR) << "Fail to read " << path;
                }
            }
            (*reply->mutable_data())[abs_path].CopyFrom(read_response);
        }
        return Status::OK;
    }

    Status GalaxyServerImpl::WriteInternal(ServerContext *context, const WriteRequest *request,
                                           WriteResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call Write.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call Write.");
        }
        std::string mode = "w";
        if (request->mode() == WriteMode::APPEND)
        {
            mode = "a";
        }
        absl::Status fs_status = GalaxyFs::Instance()->Write(request->name(), request->data(), mode);
        if (!fs_status.ok())
        {
            LOG(ERROR) << "Write failed during function call Write with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        }
        else
        {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::WriteMultipleInternal(ServerContext *context, const WriteMultipleRequest *request,
                                                   WriteMultipleResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call WriteMultiple.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call WriteMultiple.");
        }
        for (const auto& val : request->data()) {
            WriteResponse write_response;
            std::string path = val.first;
            absl::Status absl_status = GalaxyFs::Instance()->CreateFileIfNotExist(path, 0777);
            if (absl_status.ok())
            {
                std::string abs_path;
                absl::Status fs_status = GalaxyFs::Instance()->DieFileIfNotExist(path, abs_path);
                if (!fs_status.ok())
                {
                    LOG(ERROR) << "WriteMultipleInternal failed during function call WriteMultipleInternal with error " << fs_status;
                    return Status(StatusCode::INTERNAL, fs_status.ToString());
                }
                WriteRequest write_request;
                write_request.set_name(path);
                write_request.mutable_cred()->CopyFrom(request->cred());
                write_request.set_data(val.second);
                write_request.set_mode(request->mode());
                Status status = GalaxyServerImpl::WriteInternal(context, &write_request, &write_response);
                if (!status.ok())
                {
                    LOG(ERROR) << "Fail to write " << path;
                }
            }
            (*reply->mutable_data())[path].CopyFrom(write_response);
        }
        return Status::OK;
    }

    Status GalaxyServerImpl::CopyFileInternal(ServerContext *context, ServerReader<CopyRequest> *request,
                                              CopyResponse *reply)
    {
        CopyRequest copy_request;
        bool is_locked = false;
        while (request->Read(&copy_request))
        {
            if (!is_locked)
            {
                GalaxyFs::Instance()->Lock(copy_request.to_name());
                is_locked = true;
            }

            if (!GalaxyServerImpl::VerifyPassword(copy_request.cred()).ok())
            {
                LOG(ERROR) << "Wrong password from client during function call Write.";
                GalaxyFs::Instance()->Unlock(copy_request.to_name());
                return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call Write.");
            }

            absl::Status fs_status = GalaxyFs::Instance()->Write(copy_request.to_name(), copy_request.data(), "a", false);
            if (!fs_status.ok())
            {
                LOG(ERROR) << "Write failed during function call Write with error " << fs_status;
                GalaxyFs::Instance()->Unlock(copy_request.to_name());
                return Status(StatusCode::INTERNAL, fs_status.ToString());
            }
        }
        GalaxyFs::Instance()->Unlock(copy_request.to_name());
        FileSystemStatus status;
        status.set_return_code(1);
        reply->mutable_status()->CopyFrom(status);
        return Status::OK;
    }

    Status GalaxyServerImpl::CrossCellCallInternal(ServerContext *context, const CrossCellRequest *request,
                                                   CrossCellResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call CrossCellCall.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call CrossCellCall.");
        }
        CopyRequest copy_request;
        auto any_request = request->request();
        any_request.UnpackTo(&copy_request);
        galaxy::client::CopyFile(copy_request.from_name(), copy_request.to_name());
        if (request->call_type() == CrossCellCallType::MOVEFILE) {
            galaxy::client::RmFile(copy_request.from_name());
        }

        FileSystemStatus status;
        CopyResponse response;
        status.set_return_code(1);
        response.mutable_status()->CopyFrom(status);
        reply->set_call_type(request->call_type());
        reply->mutable_response()->PackFrom(response);
        return Status::OK;
    }

    Status GalaxyServerImpl::CheckHealthInternal(ServerContext *context, const HealthCheckRequest *request,
                                                 HealthCheckResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call CheckHealth.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call CheckHealth.");
        }

        struct statvfs statvfsbuf;
        absl::Status disk_status = GalaxyFs::Instance()->GetDiskUsage(&statvfsbuf);

        if (!disk_status.ok())
        {
            LOG(ERROR) << "CheckHealth failed during function call GetDiskUsage with error " << disk_status;
            return Status(StatusCode::INTERNAL, disk_status.ToString());
        }

        struct sysinfo sysinfobuf;
        absl::Status ram_status = GalaxyFs::Instance()->GetRamUsage(&sysinfobuf);
        if (!ram_status.ok())
        {
            LOG(ERROR) << "CheckHealth failed during function call GetRamUsage with error " << ram_status;
            return Status(StatusCode::INTERNAL, ram_status.ToString());
        }
        else
        {
            FileSystemUsage usage;
            double total_disk = (double)(statvfsbuf.f_blocks * statvfsbuf.f_frsize) / (1024 * 1024);
            usage.set_total_disk(total_disk);
            double available_disk = (double)(statvfsbuf.f_bfree * statvfsbuf.f_frsize) / (1024 * 1024);
            usage.set_used_disk(total_disk - available_disk);

            double total_ram = (double)sysinfobuf.totalram * sysinfobuf.mem_unit / (1024 * 1024);
            usage.set_total_ram(total_ram);
            double available_ram = (double)sysinfobuf.freeram * sysinfobuf.mem_unit / (1024 * 1024);
            usage.set_used_ram(total_ram - available_ram);
            reply->set_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
            reply->set_healthy(true);
            reply->mutable_usage()->CopyFrom(usage);
            return Status::OK;
        }
    }

    std::string ExecuteCommand(const std::string& cmd)
    {
        std::array<char, 128> buffer;
        std::string result;
        std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) {
            throw "Cmd \n" + cmd + "\nfailed.";
        }
        while (!feof(pipe.get()))
        {
            if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
                result += buffer.data();
        }
        return result;
    }

    Status GalaxyServerImpl::RemoteExecutionInternal(ServerContext *context, const RemoteExecutionRequest *request,
                                                     RemoteExecutionResponse *reply)
    {
        if (!GalaxyServerImpl::VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call RemoteExecution.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call RemoteExecution.");
        }
        std::string cmd = "";
        if (!request->home_dir().empty()) {
            cmd += "cd " + request->home_dir() + " &&";
        }
        for (const auto& pair : request->env_kargs()) {
            cmd += " " + pair.first + "=" + pair.second;
        }
        cmd += request->main();
        for (const auto& val : request->program_args()) {
            cmd += " " + val;
        }
        reply->set_raw_cmd(cmd);
        try {
            std::string result = ExecuteCommand(cmd);
            reply->set_data(result);
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
        catch (std::string errorMsg)
        {
            LOG(ERROR) << "CheckHealth failed during function call RemoteExecution with error " << errorMsg;
            return Status(StatusCode::INVALID_ARGUMENT, errorMsg);
        }

    }

    //***************************************************************************************//
    // external functions
    Status GalaxyServerImpl::GetAttr(ServerContext *context, const GetAttrRequest *request,
                                     GetAttrResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::GetAttrInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "GetAttr"}});
        return status;
    }

    Status GalaxyServerImpl::CreateDirIfNotExist(ServerContext *context, const CreateDirRequest *request,
                                                 CreateDirResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::CreateDirIfNotExistInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "CreateDirIfNotExist"}});
        return status;
    }

    Status GalaxyServerImpl::DirOrDie(ServerContext *context, const DirOrDieRequest *request,
                                      DirOrDieResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::DirOrDieInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "DirOrDie"}});
        return status;
    }

    Status GalaxyServerImpl::RmDir(ServerContext *context, const RmDirRequest *request,
                                   RmDirResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::RmDirInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "RmDir"}});
        return status;
    }

    Status GalaxyServerImpl::RmDirRecursive(ServerContext *context, const RmDirRecursiveRequest *request,
                                            RmDirRecursiveResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::RmDirRecursiveInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "RmDirRecursive"}});
        return status;
    }

    Status GalaxyServerImpl::ListDirsInDir(ServerContext *context, const ListDirsInDirRequest *request,
                                           ListDirsInDirResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::ListDirsInDirInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "ListDirsInDir"}});
        return status;
    }

    Status GalaxyServerImpl::ListFilesInDir(ServerContext *context, const ListFilesInDirRequest *request,
                                            ListFilesInDirResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::ListFilesInDirInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "ListFilesInDir"}});
        return status;
    }

    Status GalaxyServerImpl::ListAllInDirRecursive(ServerContext *context, const ListAllInDirRecursiveRequest *request,
                                                   ListAllInDirRecursiveResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::ListAllInDirRecursiveInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "ListAllInDirRecursive"}});
        return status;
    }

    Status GalaxyServerImpl::CreateFileIfNotExist(ServerContext *context, const CreateFileRequest *request,
                                                  CreateFileResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::CreateFileIfNotExistInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "CreateFileIfNotExist"}});
        return status;
    }


    Status GalaxyServerImpl::FileOrDie(ServerContext *context, const FileOrDieRequest *request,
                                                  FileOrDieResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::FileOrDieInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "FileOrDie"}});
        return status;
    }

    Status GalaxyServerImpl::RmFile(ServerContext *context, const RmFileRequest *request,
                                                  RmFileResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::RmFileInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "RmFile"}});
        return status;
    }

    Status GalaxyServerImpl::RenameFile(ServerContext *context, const RenameFileRequest *request,
                                                  RenameFileResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::RenameFileInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "RenameFile"}});
        return status;
    }

    Status GalaxyServerImpl::Read(ServerContext *context, const ReadRequest *request,
                                                  ReadResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::ReadInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "Read"}});
        return status;
    }

    Status GalaxyServerImpl::ReadMultiple(ServerContext *context, const ReadMultipleRequest *request,
                                                  ReadMultipleResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::ReadMultipleInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "ReadMultiple"}});
        return status;
    }


    Status GalaxyServerImpl::Write(ServerContext *context, const WriteRequest *request,
                                                  WriteResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::WriteInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "Write"}});
        return status;
    }

    Status GalaxyServerImpl::WriteMultiple(ServerContext *context, const WriteMultipleRequest *request,
                                           WriteMultipleResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::WriteMultipleInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "WriteMultiple"}});
        return status;
    }

    Status GalaxyServerImpl::CopyFile(ServerContext *context, ServerReader<CopyRequest> *request,
                                      CopyResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::CopyFileInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "CopyFile"}});
        return status;
    }

    Status GalaxyServerImpl::CrossCellCall(ServerContext *context, const CrossCellRequest *request,
                                           CrossCellResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::CrossCellCallInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "CrossCellCall"}});
        return status;
    }

    Status GalaxyServerImpl::CheckHealth(ServerContext *context, const HealthCheckRequest *request,
                                         HealthCheckResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::CheckHealthInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "CheckHealth"}});
        if (status.ok()) {
            double disk_usage = reply->usage().used_disk() / reply->usage().total_disk();
            double ram_usage = reply->usage().used_ram() / reply->usage().total_ram();
            opencensus::stats::Record({{stats::internal::DiskUsageMeasure(), disk_usage},
                            {stats::internal::RamUsageMeasure(), ram_usage}},
                            {{stats::internal::MethodKey(), "CheckHealthUsage"}});
        }
        return status;
    }

    Status GalaxyServerImpl::RemoteExecution(ServerContext *context, const RemoteExecutionRequest *request,
                                             RemoteExecutionResponse *reply)
    {
        absl::Time start = absl::Now();
        Status status = GalaxyServerImpl::RemoteExecutionInternal(context, request, reply);
        absl::Time end = absl::Now();
        double latency_ms = absl::ToDoubleMilliseconds(end - start);
        opencensus::stats::Record({{stats::internal::LatencyMsMeasure(), latency_ms},
                                   {stats::internal::QueryCountMeasure(), 1}},
                                  {{stats::internal::MethodKey(), "RemoteExecution"}});
        return status;
    }
} // namespace galaxy
