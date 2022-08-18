#include <thread>
#include <iostream>

#include "Client.h"

// Check for updates from the chat room periodically
void checkForMessages(Renci::Client& client)
{
    while(true)
    {
        client.checkForMessages();
        sleep(1);
    }
}

int main(int argc, char** argv)
{
    Renci::Client client{ grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()) };
    std::cout << "You have joined the chat room." << std::endl;
    
    std::thread loopCheck{ checkForMessages, std::ref(client) };

    while(true)
    {
        std::string input{};
        getline(std::cin, input);
        client.post(input);
    }

    return 0;
}
