#ifndef CLIENT_H
#define CLIENT_H

#include <grpcpp/grpcpp.h>

#include "Protobuf/ChatRoom.grpc.pb.h"

namespace Renci
{
    class Client
    {
    private:
        std::unique_ptr<Renci::ChatRoom::Stub> m_stub;
        uint64_t m_lastMessageId{ 0 };

    public:
        Client(std::shared_ptr<grpc::Channel> channel) :
            m_stub(Renci::ChatRoom::NewStub(channel))
        {}

        void post(const std::string& contents)
        {
            Renci::Post message;
            message.set_message(contents);
            message.set_identification(m_lastMessageId);

            Renci::Response response;

            grpc::ClientContext context;
            grpc::Status status = m_stub->sendMessage(&context, message, &response);
            if(status.ok())
            {
                for(const auto& message : response.messages())
                {
                    std::cout << message << std::endl;
                }
                m_lastMessageId += response.messages_size() + 1; //Include the message sent
            }
            else
            {
                std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
            }
        }

        void checkForMessages()
        {
            Renci::Post message;
            message.set_identification(m_lastMessageId);

            Renci::Response response;

            grpc::ClientContext context;
            grpc::Status status = m_stub->checkForMessages(&context, message, &response);
            if(status.ok())
            {
                if(response.messages_size() == 0)
                {
                    return;
                }

                for(const auto& message : response.messages())
                {
                    std::cout << message << std::endl;
                }
                m_lastMessageId += response.messages_size();
            }
            else
            {
                std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
            }
        }
    };
}

#endif //CLIENT_H
