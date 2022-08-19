#include <gtest/gtest.h>

#include "ChatRoom.h"

/*
 * Best practice is to test a class via its public interface.
 * As such, the !eval command is private, so it will have
 * to be manipulated in said manner. I hold that it should
 * remain private despite the ease of testing if public. 
 * However, a modification I had to make is to expose the
 * m_history of the chat via getter (no setter though). Via
 * m_history, we can observe the desired behavior in action.
 * Typically, I would write an additional unit for
 * checkForCommand, but I desire to keep this assessment brief
 * as the initial framework took about an hour and a half as I
 * had to familiarize myself with gRPC's latest updates
 * plus an hour or so of exploring alternatives to get the 
 * original source to build without modifications to my active
 * development environment or a VM.
*/
TEST(ChatRoom, CheckForEvaluation)
{
    Renci::ChatRoomService room{};

    grpc::ServerContext* context{ nullptr };
    
    //That which we intend to manipulate to test different outputs to m_history
    Renci::Post post;
    post.set_message("!eval");
    Renci::Response response;
    grpc::Status status{ room.sendMessage(context, &post, &response) };
    EXPECT_TRUE(status.ok());

    //Set this up to expect no response if just the command
    EXPECT_EQ(room.getHistory()[0], "!eval");

    post.set_message("!eval ");
    status = room.sendMessage(context, &post, &response);

    //This check ensures the function executed start to finish.
    //ASSERT with gtest terminates the test early if a fundamental
    //check has not passed. 
    ASSERT_TRUE(status.ok());

    //This should post add an additional message about an error with the !eval command.
    //Assert prevents out-of-bounds access
    ASSERT_EQ(room.getHistory().size(), 3);
    EXPECT_EQ(room.getHistory()[1], "!eval ");
    EXPECT_EQ(room.getHistory()[2], "Invalid input to eval");

    post.set_message("!eval 1 + ");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 5);
    EXPECT_EQ(room.getHistory()[3], "!eval 1 + ");
    EXPECT_EQ(room.getHistory()[4], "Invalid input to eval");

    post.set_message("!eval 1 + n");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 7);
    EXPECT_EQ(room.getHistory()[5], "!eval 1 + n");
    EXPECT_EQ(room.getHistory()[6], "Invalid input to eval");

    post.set_message("!eval 1 + 1");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 9);
    EXPECT_EQ(room.getHistory()[7], "!eval 1 + 1");
    EXPECT_EQ(room.getHistory()[8], "2");

    post.set_message("     !eval 1 + 1");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 11);
    EXPECT_EQ(room.getHistory()[9], "     !eval 1 + 1");
    EXPECT_EQ(room.getHistory()[10], "2");

    post.set_message("!eval 12 + 1");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 13);
    EXPECT_EQ(room.getHistory()[11], "!eval 12 + 1");
    EXPECT_EQ(room.getHistory()[12], "13");

    post.set_message("!eval 2 + 21");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 15);
    EXPECT_EQ(room.getHistory()[13], "!eval 2 + 21");
    EXPECT_EQ(room.getHistory()[14], "23");

    post.set_message("!eval 12 - 2");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 17);
    ASSERT_EQ(room.getHistory()[15], "!eval 12 - 2");
    ASSERT_EQ(room.getHistory()[16], "10");

    post.set_message("!eval 2 - 12");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 19);
    EXPECT_EQ(room.getHistory()[17], "!eval 2 - 12");
    EXPECT_EQ(room.getHistory()[18], "Second number is larger than the first.");

    post.set_message("!eval 1233321333333333333333 + 123");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 21);
    EXPECT_EQ(room.getHistory()[19], "!eval 1233321333333333333333 + 123");
    EXPECT_EQ(room.getHistory()[20], "First number exceeds max value of an unsigned 64 bit integer.");

    post.set_message("!eval 12 + 1233321333333333333333");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 23);
    EXPECT_EQ(room.getHistory()[21], "!eval 12 + 1233321333333333333333");
    EXPECT_EQ(room.getHistory()[22], "Second number exceeds max value of an unsigned 64 bit integer.");

    post.set_message("!eval 12345678901234567890 + 12345678901234567890");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 25);
    EXPECT_EQ(room.getHistory()[23], "!eval 12345678901234567890 + 12345678901234567890");
    EXPECT_EQ(room.getHistory()[24], "Addition of provided integers exceeds max supported number.");

    post.set_message("!eval 12 / 2");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 27);
    EXPECT_EQ(room.getHistory()[25], "!eval 12 / 2");
    EXPECT_EQ(room.getHistory()[26], "6");

    post.set_message("!eval 12 / 14");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 29);
    EXPECT_EQ(room.getHistory()[27], "!eval 12 / 14");
    EXPECT_EQ(room.getHistory()[28], "0");

    post.set_message("!eval 12 / 0");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 31);
    EXPECT_EQ(room.getHistory()[29], "!eval 12 / 0");
    EXPECT_EQ(room.getHistory()[30], "Division by zero is not allowed.");
    
    post.set_message("!eval 19 * 2");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 33);
    EXPECT_EQ(room.getHistory()[31], "!eval 19 * 2");
    EXPECT_EQ(room.getHistory()[32], "38");

    post.set_message("!eval 1234567890123456789 * 12345");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 35);
    EXPECT_EQ(room.getHistory()[33], "!eval 1234567890123456789 * 12345");
    EXPECT_EQ(room.getHistory()[34], "Multiplication of provided integers exceeds max supported number.");

    post.set_message("!eval 19 * 0");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 37);
    EXPECT_EQ(room.getHistory()[35], "!eval 19 * 0");
    EXPECT_EQ(room.getHistory()[36], "0");

    post.set_message("!eval -123 * 123");
    status = room.sendMessage(context, &post, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(room.getHistory().size(), 39);
    EXPECT_EQ(room.getHistory()[37], "!eval -123 * 123");
    EXPECT_EQ(room.getHistory()[38], "Invalid input to eval");
}
