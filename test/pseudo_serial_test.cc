#include <gtest/gtest.h>

#include <io/pseudo_serial.h>
#include <string>
#include <thread>

class PseudoSerialsTest : public ::testing::Test
{
protected:
    IO::PseudoSerial ps1;
    IO::PseudoSerial ps2;

    const std::string test_msg = "hello, world!";

    void SetUp() final
    {
        ps1.connect_to(ps2);
    }
};

TEST_F(PseudoSerialsTest, SequentialWriteRead)
{
    ASSERT_EQ(ps1.available(), 0);
    ASSERT_EQ(ps2.available(), 0);

    ps1.write(test_msg.data(), test_msg.length());

    ASSERT_EQ(ps1.available(), 0);
    ASSERT_EQ(ps2.available(), test_msg.length());

    std::string recv_msg;
    recv_msg.resize(test_msg.length());

    std::size_t read_bytes_n;

    ASSERT_TRUE(ps2.readsome(recv_msg.data(), test_msg.length(), read_bytes_n).is_ok());

    ASSERT_EQ(read_bytes_n, test_msg.length());
    ASSERT_EQ(test_msg, recv_msg);
    ASSERT_EQ(ps2.available(), 0);
}

TEST_F(PseudoSerialsTest, ReadTimeout)
{
    ps1.set_read_timeout(10);

    ASSERT_EQ(ps1.available(), 0);

    std::size_t read_bytes_n;
    char buffer[1];

    ASSERT_TRUE(ps1.readsome(buffer, 1, read_bytes_n).is_ok());
    ASSERT_EQ(read_bytes_n, 0);
}

TEST_F(PseudoSerialsTest, AsyncWriteRead)
{
    std::thread t1([&]() { ps1.write(test_msg.data(), test_msg.length()); });

    std::thread t2([&]() {
        std::string recv_msg;
        recv_msg.resize(test_msg.length());

        ASSERT_TRUE(ps2.read(recv_msg.data(), recv_msg.length()).is_ok());
        ASSERT_EQ(recv_msg, test_msg);
    });

    t1.join();
    t2.join();
}

TEST_F(PseudoSerialsTest, AsyncWriteRead_DelayedWrite)
{
    std::thread t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ps1.write(test_msg.data(), test_msg.length());
    });

    std::thread t2([&]() {
        std::string recv_msg;
        recv_msg.resize(test_msg.length());

        ASSERT_TRUE(ps2.read(recv_msg.data(), recv_msg.length()).is_ok());
        ASSERT_TRUE(recv_msg == test_msg);
    });

    t1.join();
    t2.join();
}

TEST_F(PseudoSerialsTest, AsyncWriteRead_ReadTimeout)
{
    ps2.set_read_timeout(5);

    std::thread t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        ps1.write(test_msg.data(), test_msg.length());
    });

    std::thread t2([&]() {
        std::string recv_msg;
        recv_msg.resize(test_msg.length());

        std::size_t read_bytes_n;

        ASSERT_TRUE(ps2.readsome(recv_msg.data(), recv_msg.length(), read_bytes_n).is_ok());
        ASSERT_TRUE(read_bytes_n == 0);
    });

    t1.join();
    t2.join();
}

TEST_F(PseudoSerialsTest, AsyncWriteRead_ReadTimeoutThenSuccess)
{
    ps2.set_read_timeout(5);

    std::thread t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        ps1.write(test_msg.data(), test_msg.length());
    });

    std::thread t2([&]() {
        std::string recv_msg;
        recv_msg.resize(test_msg.length());

        std::size_t read_bytes_n;

        ASSERT_TRUE(ps2.readsome(recv_msg.data(), recv_msg.length(), read_bytes_n).is_ok());
        ASSERT_TRUE(read_bytes_n == 0);

        ASSERT_TRUE(ps2.readsome(recv_msg.data(), recv_msg.length(), read_bytes_n).is_ok());
        ASSERT_TRUE(read_bytes_n == test_msg.length());
        ASSERT_EQ(recv_msg, test_msg);
    });

    t1.join();
    t2.join();
}