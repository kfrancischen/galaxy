#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "glog/logging.h"
#include "cpp/core/galaxy_server.h"
#include "cpp/core/galaxy_fs.h"

using grpc::ServerContext;
using grpc::Status;

using galaxy_schema::Owner;
using galaxy_schema::FileSystemStatus;
using galaxy_schema::Credential;
using galaxy_schema::Attribute;

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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        struct stat statbuf;
        std::string path = request->name();
        if (GalaxyFs::Instance()->GetAttr(path, &statbuf) != 0) {
            LOG(ERROR) << "GetAttr failed.";
            return Status::CANCELLED;
        } else {
            FileSystemStatus status;
            Owner owner;
            status.set_return_code(1);
            owner.set_uid(statbuf.st_uid);
            owner.set_uid(statbuf.st_gid);

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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        if (GalaxyFs::Instance()->CreateDirIfNotExist(request->name(), request->mode()) != 0) {
            LOG(ERROR) << "CreateDirIfNotExist failed.";
            return Status::CANCELLED;
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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        std::string out_path;
        if (GalaxyFs::Instance()->DieDirIfNotExist(request->name(), out_path) != 0) {
            LOG(ERROR) << "DieDirIfNotExist failed.";
            return Status::CANCELLED;
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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        if (GalaxyFs::Instance()->RmDir(request->name()) != 0) {
            LOG(ERROR) << "RmDir failed.";
            return Status::CANCELLED;
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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        if (GalaxyFs::Instance()->RmDirRecursive(request->name()) != 0) {
            LOG(ERROR) << "RmDirRecursive failed.";
            return Status::CANCELLED;
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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        std::vector<std::string> dirs;
        if (GalaxyFs::Instance()->ListDirsInDir(request->name(), dirs) != 0) {
            LOG(ERROR) << "ListDirsInDir failed.";
            return Status::CANCELLED;
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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        std::vector<std::string> files;
        if (GalaxyFs::Instance()->ListFilesInDir(request->name(), files) != 0) {
            LOG(ERROR) << "ListFilesInDir failed.";
            return Status::CANCELLED;
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            *reply->mutable_sub_files() = {files.begin(), files.end()};
            return Status::OK;
        }
    }


    Status GalaxyServerImpl::CreateFileIfNotExist(ServerContext *context, const CreateFileRequest *request,
                                                  CreateFileResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        if (GalaxyFs::Instance()->CreateFileIfNotExist(request->name(), request->mode()) != 0) {
            LOG(ERROR) << "CreateFileIfNotExist failed.";
            return Status::CANCELLED;
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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        std::string out_path;
        if (GalaxyFs::Instance()->DieFileIfNotExist(request->name(), out_path) != 0) {
            LOG(ERROR) << "DieFileIfNotExist failed.";
            return Status::CANCELLED;
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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        if (GalaxyFs::Instance()->RmFile(request->name()) != 0) {
            LOG(ERROR) << "RmFile failed.";
            return Status::CANCELLED;
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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        if (GalaxyFs::Instance()->RenameFile(request->old_name(), request->new_name()) != 0) {
            LOG(ERROR) << "RenameFile failed.";
            return Status::CANCELLED;
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
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        std::string data;
        if (GalaxyFs::Instance()->Read(request->name(), data) != 0) {
            LOG(ERROR) << "Read failed.";
            return Status::CANCELLED;
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            reply->set_data(data);
            return Status::OK;
        }
    }

    Status GalaxyServerImpl::Write(ServerContext *context, const WriteRequest *request,
                                   WriteResponse *reply)
    {
        if (!VerifyPassword(request->cred()).ok()) {
            LOG(ERROR) << "Wrong password from client.";
            return Status::CANCELLED;
        }
        if (GalaxyFs::Instance()->Write(request->name(), request->data()) != 0) {
            LOG(ERROR) << "Write failed.";
            return Status::CANCELLED;
        } else {
            FileSystemStatus status;
            status.set_return_code(1);
            reply->mutable_status()->CopyFrom(status);
            return Status::OK;
        }
    }

} // namespace galaxy
