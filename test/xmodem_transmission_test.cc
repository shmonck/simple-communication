#include <gtest/gtest.h>

#include <io/pseudo_serial.h>
#include <xmodem/checksum.h>
#include <xmodem/receiver.h>
#include <xmodem/transmitter.h>

#include <array>
#include <iostream>
#include <thread>

#ifndef TEST_RESOURCE_DIR
    #define TEST_RESOURCE_DIR "./"
#endif

class XModemTransmissionTest : public ::testing::Test
{
protected:
    IO::PseudoSerial ps1;
    IO::PseudoSerial ps2;

    XModem::Checksum checksum;

    XModem::Transmitter transmitter{ ps1 };
    XModem::Receiver receiver{ ps2, checksum };

    const std::string_view in_file_name = TEST_RESOURCE_DIR "sample_file.txt";
    const std::string_view out_file_name = TEST_RESOURCE_DIR "sample_file_recv.txt";

    std::ifstream ifstream;
    std::ofstream ofstream;

    void SetUp() final
    {
        ps1.connect_to(ps2);

        ifstream.open(in_file_name.data(), std::ios::in | std::ios::binary);
        ofstream.open(out_file_name.data(), std::ios::out | std::ios::trunc | std::ios::binary);

        if ( !ifstream.is_open() )
        {
            GTEST_FATAL_FAILURE_("Could not open input file");
        }

        if ( !ofstream.is_open() )
        {
            GTEST_FATAL_FAILURE_("Could not open output file");
        }
    }
};

TEST_F(XModemTransmissionTest, SuccessfulTransmission)
{
    std::thread t1([&]() { ASSERT_TRUE(receiver.receive(ofstream)); });

    std::thread t2([&]() { ASSERT_TRUE(transmitter.send(ifstream)); });

    t1.join();
    t2.join();

    // TODO: Check file contents
}