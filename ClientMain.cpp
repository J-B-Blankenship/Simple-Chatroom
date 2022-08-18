#include "Client.h"

int main(int argc, char** argv)
{
    Renci::Client client{ grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()) };
    while(true)
    {
    }

    return 0;
}
