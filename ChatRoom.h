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

        void checkForCommands(const std::string& message)
        {
            for(uint64_t i = 0; i < message.size(); i++)
            {
                //Check for empty space before a possible command
                if(message[i] == ' ')
                {
                    continue;
                }
                else if(message[i] != '!')
                {
                    return; //Extra characters before command is invalid
                }
                //Start of a command
                else if(message[i] == '!')
                {
                    if(message.find("echo ", i) != std::string::npos)
                    {
                        history.push_back(message.substr(i + 6)); //6 comes from ! plus 'echo' plus the space
                        return; //Prevent further checking of commands
                    }
                }
            }
        }

    public:
        grpc::Status sendMessage(grpc::ServerContext* contex,
            const Post* post,
            Response* response) override
        {
            //Add some thread safety
            const std::lock_guard<std::mutex> lock{ m_mutex };

            for(uint64_t i = post->identification(); i < history.size(); i++)
            {
                std::string* temp = response->add_messages();
                *temp = history[i];
            }

            //Add the new message
            history.push_back(post->message());
            checkForCommands(post->message());
            return grpc::Status::OK;
        }

        grpc::Status checkForMessages(grpc::ServerContext* contex,
            const Post* post,
            Response* response) override
        {
            //Add some thread safety
            const std::lock_guard<std::mutex> lock{ m_mutex };
            
            for(uint64_t i = post->identification(); i < history.size(); i++)
            {
                std::string* temp = response->add_messages();
                *temp = history[i];
            }
            return grpc::Status::OK;
        }
    };
}

#endif //CHATROOM_H
