#include <grpcpp/health_check_service_interface.h>

#include "ChatRoom.h"

int main(int argc, char** argv)
{
    const std::string serverAddress{"0.0.0.0:50051"};
    Renci::ChatRoomService service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::ServerBuilder builder;

    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> chatRoom(builder.BuildAndStart());
    std::cout << "Chat room is available on: " << serverAddress << std::endl;
    chatRoom->Wait();

    return 0;
}
