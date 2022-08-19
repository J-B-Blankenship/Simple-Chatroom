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
        std::vector<std::string> m_history{};
        std::mutex m_mutex{};

        //Positive or negative numbers was not specified. Assumption of only positive numbers (ie. unsigned integers).
        //Minor modifications necessary to allow for negative numbers.

        //Additional Features: support negative numbers, strip leading zeros from input,
        //better messages for supported behavior vs. unsupported behavior
        void parseEvaluationCommand(const std::string& message)
        {
            //Nothing provided or minimum amount of characters for a valid expression not met
            if(message.size() == 0 || message.size() < 5)
            {
                m_history.push_back("Invalid input to eval");
                return;
            }

            uint64_t firstNumber{ 0 };
            uint64_t secondNumber{ 0 };
            uint64_t startOfSecondNumber{ 0 };
            bool parsingFirstNumber = true;
            bool parsingSecondNumber = false;

            //Steps of the below:
            //Check for valid 1st number
            //Check for a valid operator
            //Check for valid 2nd number
            for(uint64_t i = 0; i < message.size(); i++)
            {
                if(parsingFirstNumber)
                {
                    if(message[i] >= '0' && message[i] <= '9')
                    {
                        continue;
                    }
                    else if(message[i] == ' ')
                    {
                        //A clever solution from a colleague with a past employer and project I worked.
                        //We could not find any flaws with it at the time for super long inputs or inputs
                        //beyond the max value.
                        if((message.substr(0, i) > "18446744073709551615"
                            && message.substr(0,i).size() == 20)
                            || message.substr(0, i).size() > 20)
                        {
                            m_history.push_back("First number exceeds max value of an unsigned 64 bit integer.");
                            return;
                        }
                        firstNumber = stoull(message.substr(0, i));
                        parsingFirstNumber = false;
                    }
                    //Invalid character. Only numbers or a single space allowed
                    else
                    {
                        m_history.push_back("Invalid input to eval");
                        return;
                    }
                }
                //Operator
                else if(!parsingFirstNumber && !parsingSecondNumber)
                {
                    if(message[i] == '*' || message[i] == '/'
                        || message[i] == '-' || message[i] == '+')
                    {
                        //Check for the space following immediately after.
                        i++;
                        if(message[i] == ' ')
                        {
                            parsingSecondNumber = true;
                            startOfSecondNumber = i + 1;
                            continue;
                        }
                        else
                        {
                            m_history.push_back("Invalid input to eval");
                            return;
                        }
                    }
                    //A valid operator was not provided
                    else
                    {
                        m_history.push_back("Invalid input to eval");
                        return;
                    }
                }
                //Repeat of the first
                else if(parsingSecondNumber)
                {
                    //Terminate at the end of message and instantiate secondNumber
                    if(message[i] >= '0' && message[i] <= '9'
                        && i == message.size() - 1)
                    {
                        //A clever solution from a colleague with a past employer and project I worked.
                        //We could not find any flaws with it at the time for super long inputs or inputs
                        //beyond the max value.
                        if((message.substr(startOfSecondNumber) > "18446744073709551615"
                            && message.substr(startOfSecondNumber).size() == 20)
                            || message.substr(startOfSecondNumber).size() > 20)
                        {
                            m_history.push_back("Second number exceeds max value of an unsigned 64 bit integer.");
                            return;
                        }
                        secondNumber = stoull(message.substr(startOfSecondNumber));
                        parsingSecondNumber = false;
                    }
                    else if(message[i] >= '0' && message[i] <= '9')
                    {
                        continue;
                    }
                    //Any character not a digit is invalid.
                    else
                    {
                        m_history.push_back(std::string{"Invalid input to eval"});
                        return;
                    }
                }
            }

            //Now to do the operation specified
            if(message.find('+') != std::string::npos)
            {
                //Checking for overflow.
                if(std::numeric_limits<uint64_t>::max() - firstNumber < secondNumber)
                {
                    m_history.emplace_back("Addition of provided integers exceeds max supported number.");
                    return;
                }
                m_history.push_back(std::to_string(firstNumber + secondNumber));
            }
            else if(message.find('-') != std::string::npos)
            {
                if(firstNumber < secondNumber)
                {
                    m_history.emplace_back("Second number is larger than the first.");
                    return;
                }
                m_history.push_back(std::to_string(firstNumber - secondNumber));
            }
            else if(message.find('*') != std::string::npos)
            {
                //Too lazy to bother checking second if-statement with the interaction of a 0. Just return 0 here.
                if(firstNumber == 0 || secondNumber == 0)
                {
                    m_history.push_back("0");
                    return;
                }
                //This should catch overflow in every case where the result is at least 1 more than second number.
                //However, it will not catch the cases for less than 1 and greater than 0.
                //I am not entirely sure how one would check this with the resulting truncation if swapped to a 
                //signed integer. Using the % mod operator may work. If a remainder and first < second,
                //then that should result in exceeding the max unsigned int. I will do this and test it
                //if I think about it near the end.
                if(std::numeric_limits<uint64_t>::max() / firstNumber < secondNumber)
                {
                    m_history.emplace_back("Multiplication of provided integers exceeds max supported number.");
                    return;
                }
                m_history.push_back(std::to_string(firstNumber * secondNumber));
            }
            else if(message.find('/') != std::string::npos)
            {
                if(secondNumber == 0)
                {
                    m_history.emplace_back("Division by zero is not allowed.");
                    return;
                }
                m_history.push_back(std::to_string(firstNumber / secondNumber));
            }
            else
            {
                //gRPC does not exit with a std::runtime_error if I recall correctly. It may though.
                throw std::runtime_error{"parseEvaluationCommand has reached a section that should be unreachable."};
                m_history.emplace_back("A critical error has occurred at runtime. This should be unreachable.");
            }
        }

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
                        m_history.push_back(message.substr(i + 6)); //6 comes from ! plus 'echo' plus the space
                        return; //Prevent further checking of commands
                    }
                    else if(message.find("eval ", i) != std::string::npos)
                    {
                        parseEvaluationCommand(message.substr(i + 6));
                        return;
                    }
                }
            }
        }

    public:
        const std::vector<std::string>& getHistory() const { return m_history; }

        grpc::Status sendMessage(grpc::ServerContext* context,
            const Post* post,
            Response* response) override
        {
            //Add some thread safety
            const std::lock_guard<std::mutex> lock{ m_mutex };

            for(uint64_t i = post->identification(); i < m_history.size(); i++)
            {
                std::string* temp = response->add_messages();
                *temp = m_history[i];
            }

            //Add the new message
            m_history.push_back(post->message());
            checkForCommands(post->message());
            return grpc::Status::OK;
        }

        grpc::Status checkForMessages(grpc::ServerContext* contex,
            const Post* post,
            Response* response) override
        {
            //Add some thread safety
            const std::lock_guard<std::mutex> lock{ m_mutex };
            
            for(uint64_t i = post->identification(); i < m_history.size(); i++)
            {
                std::string* temp = response->add_messages();
                *temp = m_history[i];
            }
            return grpc::Status::OK;
        }
    };
}

#endif //CHATROOM_H
