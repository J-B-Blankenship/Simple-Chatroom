#ifndef CHATROOM_H
#define CHATROOM_H

#include <mutex>
#include <grpcpp/grpcpp.h>

#include "Protobuf/ChatRoom.grpc.pb.h"

namespace Renci
{
    class ChatRoomService final : public Renci::ChatRoom::Service
    {
    private:
        std::vector<std::string> history{};
        std::mutex m_mutex{};

    public:
        grpc::Status sendMessage(grpc::ServerContext* contex,
            const Post* post,
            Response* response) override
        {
            //Add some thread safety
            const std::lock_guard<std::mutex> lock{ m_mutex };

            //Send back all updates to chat room that have not been received
            std::vector<std::string>::const_iterator iter = history.begin();
            iter += post->identification();
            response->mutable_messages()->Add(iter, history.cend());

            //Add the new message
            history.push_back(post->message());
            return grpc::Status::OK;
        }

        grpc::Status checkForMessages(grpc::ServerContext* contex,
            const Post* post,
            Response* response) override
        {
            const std::lock_guard<std::mutex> lock{ m_mutex };
            std::vector<std::string>::const_iterator iter = history.begin();
            iter += post->identification();
            response->mutable_messages()->Add(iter, history.cend());
            return grpc::Status::OK;
        }
    };
}

#endif //CHATROOM_H
