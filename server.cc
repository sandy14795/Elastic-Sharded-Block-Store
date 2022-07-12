#include <iostream>
#include <grpc/grpc.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <mutex>          // std::mutex

#include "hafs.grpc.pb.h"
// #include "block_manager.h"
#include "common.cc"
#include "replicator.h"
#include "SHA256.cc"
using ::Request;
using ::HeartBeatResponse;
using ::ReadRequest;
using ::ReadResponse;
using ::WriteRequest;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

std::mutex mtx; 

class HafsImpl final : public Hafs::Service {
    private:
        // HafsClient otherMirrorClient;
        HeartBeatResponse_Role role; 
        BlockManager blockManager;
        Replicator replicator;
    public:
        int64_t counter = 0;
        explicit HafsImpl(std::string otherMirrorAddress, HeartBeatResponse_Role role, BlockManager blockManager): replicator(otherMirrorAddress, blockManager) {
            this->role = role;
            this->blockManager = blockManager;
            // this->replicator = replicator;
            std::cout << "[Server] Starting up the Ha FS server!" << std::endl;

        }

        Status HeartBeat(ServerContext *context, const Request *req, HeartBeatResponse *res) override {
            res->set_role(this->role);
            res->set_health(replicator.getHealth());
            res->set_blockload(blockManager.hashCommittedBlocks.size());
            return Status::OK;
        }

        Status Read(ServerContext *context, const ReadRequest *req, ReadResponse *res) override {
            //std::cout << "[Server] (Read) addr=" << req->address() << std::endl;
            std::string data;
            //blockManager.lockAddress(req->address());
            blockManager.read(req->address(), &data);
            res->set_data(data);
            res->set_status(ReadResponse_Status_VALID);
            //blockManager.unlockAddress(req->address());
            return Status::OK;
        }

        Status Write(ServerContext *context, const WriteRequest *req, Response *res) override {
            std::cout << "[Server] (Write) addr=" << req->address() << std::endl;
            crash(req->address(), "primaryFail");
            crash(req->address(), "backupFail");

            blockManager.lockAddress(req->address());
            if(!replicator.otherMirrorClient.getIsAlive()) {
                std::cout << "[Server](write) Other Replica down, sending block to replicator after local write!!" << std::endl;
                blockManager.write(req->address(), req->data());
                blockManager.commit(req->address());
                // std::cout << "adding to queue addr: " << req->address() << std::endl;
                replicator.addPendingBlock(req->address());
                res->set_status(Response_Status_VALID);
                blockManager.unlockAddress(req->address());
                return Status::OK;
            } else {
                // std::cout << "Persisting block to other replica!" << std::endl;
                if(replicator.otherMirrorClient.ReplicateBlock(req->address(), req->data())) {
                    crash(req->address(), "primaryFailAfterBackupTemp");
                    blockManager.write(req->address(), req->data());
                    if(replicator.otherMirrorClient.CommitBlock(req->address())) {
                        crash(req->address(), "commitBlockInconsistency");
                        blockManager.commit(req->address());
                        res->set_status(Response_Status_VALID);
                        blockManager.unlockAddress(req->address());
                        return Status::OK;
                    }
                    res->set_status(Response_Status_INVALID);
                    blockManager.unlockAddress(req->address());
                    return Status::OK;
                } else {
                    std::cout << "[Server](write) Write to other replica fail!! Rejecting this write!" << std::endl;
                    res->set_status(Response_Status_INVALID);
                    blockManager.unlockAddress(req->address());
                    return Status::OK;
                }
            }
        }

        Status ReplicateBlock(ServerContext *context, const WriteRequest *req, Response *res) override {
            std::cout << "[Server] (ReplicateBlock) addr=" << req->address() << std::endl;
            blockManager.lockAddress(req->address());
            
            blockManager.write(req->address(), req->data());
            res->set_status(Response_Status_VALID);

            blockManager.unlockAddress(req->address());
            return Status::OK;
        }

        Status CommitBlock(ServerContext *context, const WriteRequest *req, Response *res) override { 
            std::cout << "[Server] (CommitBlock) addr=" << req->address() << std::endl;
            blockManager.lockAddress(req->address());

            blockManager.commit(req->address());
            res->set_status(Response_Status_VALID);
            blockManager.unlockAddress(req->address());
            
            return Status::OK;
        }

        Status CheckConsistancy(ServerContext *context, const ReadRequest *req, Response *res) override 
        {
            std::cout <<"[Server] Creating CheckSum"<<std::endl;
            std::string data1;
            std::string data2;
            blockManager.lockAddress(req->address());        
            blockManager.read(req->address(), &data1);
            replicator.otherMirrorClient.Read(req->address(),&data2);
            std::string pHash = blockManager.CalCheckSum(data1);
            std::string bHash = blockManager.CalCheckSum(data2);
            std::cout <<"[Server] Hash Value primary: "<<pHash<<std::endl;            
            std::cout <<"[Server] Hash Value backup: "<<bHash<<std::endl;
            blockManager.unlockAddress(req->address());
            if(pHash==bHash)
                res->set_status(Response_Status_VALID);
            else
                res->set_status(Response_Status_INVALID);
            return Status::OK;
        }

        void crash(int address, string mask){
            /*if (address == 4096 && mask == "primaryFail" && role == HeartBeatResponse_Role_PRIMARY){
                cout << "[Testing] Primary failing before sending request to backup" << endl;
                exit(1);
            }


            if (address == 8192 && mask == "primaryFailAfterBackupTemp" && role == HeartBeatResponse_Role_PRIMARY){
                cout << "[Testing] Primary failing after receiving ack from backup " << endl;
                exit(1);
            }

            else if (address == 12288 && mask == "commitBlockInconsistency" && role == HeartBeatResponse_Role_PRIMARY) {
                cout << "Committed on backup, present in primary temp. Inconsistency :(" << endl;
                exit(1);
            }

            else if (address == 16384 && mask == "onlyAckMissing" && role == HeartBeatResponse_Role_PRIMARY) {
                cout << "Committed on both primary and backup, at-most once semantics" << endl;
                exit(1);
            }*/
        }


};

int main(int argc, char **argv) {
    std::string serverAddr;
    std::string fsPath;
    std::string role;
    std::string otherMirrorAddress;

    if(!getArg(argc, argv, "SAddr", &serverAddr, 1) || !getArg(argc, argv, "path", &fsPath, 2) || !getArg(argc, argv, "role", &role, 3) || !getArg(argc, argv, "MAddr", &otherMirrorAddress, 4)) {
        exit(1);
    }

    HeartBeatResponse_Role roleEnum;

    if(role.size() == 1 && role[0] == 'p') {
        roleEnum = HeartBeatResponse_Role_PRIMARY;
    } else if(role.size() == 1 && role[0] == 'b') {
        roleEnum = HeartBeatResponse_Role_BACKUP;
    } else {
        std::cout << "Incorrect role specified, can either be 'b' for backup and 'p' for primary!!!" << std::endl;
    }

    std::string server_address = serverAddr;
    HafsImpl service(otherMirrorAddress, roleEnum, BlockManager(fsPath));
    ServerBuilder builder;
    // HafsClient client(grpc::CreateChannel("0.0.0.0:8091", grpc::InsecureChannelCredentials()), "0.0.0.0:8091", false);
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
    return 0;
}
