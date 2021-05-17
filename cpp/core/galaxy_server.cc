#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "glog/logging.h"
#include "cpp/core/galaxy_server.h"
#include "cpp/core/galaxy_fs.h"
#include "cpp/internal/galaxy_const.h"

using grpc::ServerContext;
using grpc::Status;
using grpc::StatusCode;
using grpc::ServerReader;
using grpc::ServerWriter;

using galaxy_schema::Owner;
using galaxy_schema::FileSystemStatus;
using galaxy_schema::Credential;
using galaxy_schema::Attribute;
using galaxy_schema::WriteMode;

using galaxy_schema::CreateDirRequest;
using galaxy_schema::CreateDirResponse;
using galaxy_schema::CreateFileRequest;
using galaxy_schema::CreateFileResponse;
using galaxy_schema::DirOrDieRequest;
using galaxy_schema::DirOrDieResponse;
using galaxy_schema::FileOrDieRequest;
using galaxy_schema::FileOrDieResponse;
using galaxy_schema::GetAttrRequest;
using galaxy_schema::GetAttrResponse;
using galaxy_schema::ListDirsInDirRequest;
using galaxy_schema::ListDirsInDirResponse;
using galaxy_schema::ListFilesInDirRequest;
using galaxy_schema::ListFilesInDirResponse;
using galaxy_schema::ListAllInDirRecursiveRequest;
using galaxy_schema::ListAllInDirRecursiveResponse;
using galaxy_schema::ReadRequest;
using galaxy_schema::ReadResponse;
using galaxy_schema::ReadMultipleRequest;
using galaxy_schema::ReadMultipleResponse;
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
using galaxy_schema::DownloadRequest;
using galaxy_schema::DownloadResponse;
using galaxy_schema::UploadRequest;
using galaxy_schema::UploadResponse;

namespace galaxy
{

    void GalaxyServerImpl::SetPassword(const std::string& password) {
        password_ = password;
    }

    absl::Status GalaxyServerImpl::VerifyPassword(const Credential& cred) {
        if (cred.password() != password_) {
            return absl::PermissionDeniedError("Wrong password.");
        } else {
            return absl::OkStatus();
        }
    }

    Status GalaxyServerImpl::GetAttr(ServerContext *context, const GetAttrRequest *request,
                                     GetAttrResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call GetAttr";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call GetAttr.");
        }
        struct stat statbuf;
        std::string path = request->name();
        absl::Status fs_status = GalaxyFs::Instance()->GetAttr(path, &statbuf);
        if (!fs_status.ok()) {
            LOG(ERROR) << "GetAttr failed during function call GetAttr with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            Owner owner;
            status.set_return_code(1);
            owner.set_uid(statbuf.st_uid);
            owner.set_gid(statbuf.st_gid);

            Attribute attributes;
            attributes.set_dev(statbuf.st_dev);
            attributes.set_ino(statbuf.st_ino);
            attributes.set_mode(statbuf.st_mode);
            attributes.set_nlink(statbuf.st_nlink);
            attributes.mutable_owner()->CopyFrom(owner);
            attributes.set_rdev(statbuf.st_rdev);
            attributes.set_size(statbuf.st_size);
            attributes.set_blksize(statbuf.st_blksize);
            attributes.set_blocks(statbuf.st_blocks);
            attributes.set_atime(statbuf.st_atime);
            attributes.set_mtime(statbuf.st_mtime);
            attributes.set_ctime(statbuf.st_ctime);

            reply->mutable_attr()->CopyFrom(attributes);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::CreateDirIfNotExist(ServerContext *context, const CreateDirRequest *request,
                                                 CreateDirResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call CreateDirIfNotExist.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->CreateDirIfNotExist(request->name(), request->mode());
        if (!fs_status.ok()) {
            LOG(ERROR) << "CreateDirIfNotExist failed during function call CreateDirIfNotExist with error" << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::DirOrDie(ServerContext *context, const DirOrDieRequest *request,
                                      DirOrDieResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call DirOrDie.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call DirOrDie.");
        }
        std::string out_path;
        absl::Status fs_status = GalaxyFs::Instance()->DieDirIfNotExist(request->name(), out_path);
        if (!fs_status.ok()) {
            LOG(ERROR) << "DieDirIfNotExist failed during function call DirOrDie with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            reply->set_name(out_path);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::RmDir(ServerContext *context, const RmDirRequest *request,
                                   RmDirResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call RmDir.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call RmDir.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->RmDir(request->name());
        if (!fs_status.ok()) {
            LOG(ERROR) << "RmDir failed during function call RmDir with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::RmDirRecursive(ServerContext *context, const RmDirRecursiveRequest *request,
                                            RmDirRecursiveResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call RmDirRecursive.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call RmDirRecursive.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->RmDirRecursive(request->name());
        if (!fs_status.ok()) {
            LOG(ERROR) << "RmDirRecursive failed during function call RmDirRecursive with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }


    Status GalaxyServerImpl::ListDirsInDir(ServerContext *context, const ListDirsInDirRequest *request,
                                           ListDirsInDirResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call ListDirsInDir.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call ListDirsInDir.");
        }
        std::vector<std::string> dirs;
        absl::Status fs_status = GalaxyFs::Instance()->ListDirsInDir(request->name(), dirs);
        if (!fs_status.ok()) {
            LOG(ERROR) << "ListDirsInDir failed during function call ListDirsInDir with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            *reply->mutable_sub_dirs() = {dirs.begin(), dirs.end()};
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::ListFilesInDir(ServerContext *context, const ListFilesInDirRequest *request,
                                            ListFilesInDirResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call ListFilesInDir.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call ListFilesInDir.");
        }
        std::vector<std::string> files;
        absl::Status fs_status = GalaxyFs::Instance()->ListFilesInDir(request->name(), files);
        if (!fs_status.ok()) {
            LOG(ERROR) << "ListFilesInDir failed during function call ListFilesInDir with error" << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            *reply->mutable_sub_files() = {files.begin(), files.end()};
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::ListAllInDirRecursive(ServerContext *context, const ListAllInDirRecursiveRequest *request,
                                 ListAllInDirRecursiveResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call ListAllInDirRecursive.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call ListAllInDirRecursive.");
        }
        std::vector<std::string> files;
        std::vector<std::string> dirs;
        absl::Status fs_status = GalaxyFs::Instance()->ListAllInDirRecursive(request->name(), dirs, files);
        if (!fs_status.ok()) {
            LOG(ERROR) << "ListFilesInDir failed during function call ListAllInDirRecursive with error" << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            *reply->mutable_sub_files() = {files.begin(), files.end()};
            *reply->mutable_sub_dirs() = {dirs.begin(), dirs.end()};
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::CreateFileIfNotExist(ServerContext *context, const CreateFileRequest *request,
                                                  CreateFileResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call CreateFileIfNotExist.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call CreateFileIfNotExist.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->CreateFileIfNotExist(request->name(), request->mode());
        if (!fs_status.ok()) {
            LOG(ERROR) << "CreateFileIfNotExist failed during function call CreateFileIfNotExist with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::FileOrDie(ServerContext *context, const FileOrDieRequest *request,
                                       FileOrDieResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call FileOrDie.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call FileOrDie.");
        }
        std::string out_path;
        absl::Status fs_status = GalaxyFs::Instance()->DieFileIfNotExist(request->name(), out_path);
        if (!fs_status.ok()) {
            LOG(ERROR) << "DieFileIfNotExist failed during function call FileOrDie with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            reply->set_name(out_path);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::RmFile(ServerContext *context, const RmFileRequest *request,
                                    RmFileResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call RmFile.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call RmFile.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->RmFile(request->name());
        if (!fs_status.ok()) {
            LOG(ERROR) << "RmFile failed during function call RmFile with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::RenameFile(ServerContext *context, const RenameFileRequest *request,
                                        RenameFileResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call RenameFile.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call RenameFile.");
        }
        absl::Status fs_status = GalaxyFs::Instance()->RenameFile(request->old_name(), request->new_name());
        if (!fs_status.ok()) {
            LOG(ERROR) << "RenameFile failed during function call RenameFile with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::Read(ServerContext *context, const ReadRequest *request,
                                  ReadResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client client during function call Read.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call Read.");
        }
        std::string data;
        absl::Status fs_status = GalaxyFs::Instance()->Read(request->name(), data);
        if (!fs_status.ok()) {
            LOG(ERROR) << "Read failed client during function call Read with error "<< fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            reply->set_data(data);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::ReadMultiple(ServerContext *context, const ReadMultipleRequest *request,
                                          ReadMultipleResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client client during function call ReadMultiple.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call ReadMultiple.");
        }
        for (const auto& path : request->names()) {
            ReadResponse read_response;
            std::string abs_path;
            absl::Status absl_status = GalaxyFs::Instance()->DieFileIfNotExist(path, abs_path);
            if (absl_status.ok()) {
                ReadRequest read_request;
                read_request.set_name(path);
                read_request.mutable_cred()->CopyFrom(request->cred());
                Status status = GalaxyServerImpl::Read(context, &read_request, &read_response);
                if (!status.ok()) {
                    LOG(ERROR) << "Fail to read " << path;
                }
            }
            (*reply->mutable_data())[abs_path].CopyFrom(read_response);
        }
        return Status::OK;
    }

    Status GalaxyServerImpl::Write(ServerContext *context, const WriteRequest *request,
                                   WriteResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client during function call Write.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call Write.");
        }
        std::string mode = "w";
        if (request->mode() == WriteMode::APPEND) {
            mode = "a";
        }
        absl::Status fs_status = GalaxyFs::Instance()->Write(request->name(), request->data(), mode);
        if (!fs_status.ok()) {
            LOG(ERROR) << "Write failed during function call Write with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::DownloadFile(ServerContext *context, const DownloadRequest *request,
                                       ServerWriter<DownloadResponse> *reply)
    {
        if (!VerifyPassword(request->cred()).ok())
        {
            LOG(ERROR) << "Wrong password from client during function call DownloadFile.";
            return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call DownloadFile.");
        }
        std::string out_path;
        absl::Status fs_status = GalaxyFs::Instance()->DieFileIfNotExist(request->from_name(), out_path);
        if (!fs_status.ok()) {
            LOG(ERROR) << "DownloadFile failed during function call DownloadFile with error " << fs_status;
            return Status(StatusCode::INTERNAL, fs_status.ToString());
        } else {
            GalaxyFs::Instance()->Lock(request->from_name());
            std::ifstream infile(out_path, std::ifstream::binary);
            std::vector<char> buffer (galaxy::constant::kChunkSize + 1, 0);
            while (infile) {
                infile.read(buffer.data(), sizeof(buffer));
                std::streamsize s = ((infile) ? galaxy::constant::kChunkSize : infile.gcount());
                buffer[s] = 0;
                DownloadResponse response;
                FileSystemStatus status;
                status.set_return_code(1);
                response.mutable_status()->CopyFrom(status);
                response.set_data(std::string(buffer.data()));
                reply->Write(response);
            }
            GalaxyFs::Instance()->Unlock(request->from_name());
            infile.close();
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::UploadFile(ServerContext *context, ServerReader<UploadRequest> *request,
                                        UploadResponse *reply)
    {
        UploadRequest write_request;
        bool is_locked = false;
        while (request->Read(&write_request)) {
            if (!is_locked) {
                GalaxyFs::Instance()->Lock(write_request.to_name());
                is_locked = true;
            }

            if (!VerifyPassword(write_request.cred()).ok()) {
                LOG(ERROR) << "Wrong password from client during function call Write.";
                GalaxyFs::Instance()->Unlock(write_request.to_name());
                return Status(StatusCode::PERMISSION_DENIED, "Wrong password from client during function call Write.");
            }

            absl::Status fs_status = GalaxyFs::Instance()->Write(write_request.to_name(), write_request.data(), "a", false);
            if (!fs_status.ok()) {
                LOG(ERROR) << "Write failed during function call Write with error " << fs_status;
                GalaxyFs::Instance()->Unlock(write_request.to_name());
                return Status(StatusCode::INTERNAL, fs_status.ToString());
            }

        }
        GalaxyFs::Instance()->Unlock(write_request.to_name());
        FileSystemStatus status;
        status.set_return_code(1);
        reply->mutable_status()->CopyFrom(status);
        return Status::OK;
    }

} // namespace galaxy
