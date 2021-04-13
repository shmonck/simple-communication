#include <gtest/gtest.h>

#include <io/pseudo_serial.h>

TEST(PseudoSerialTest, SimpleWriteRead)
{
    IO::PseudoSerial ps1;
    IO::PseudoSerial ps2;

    ps1.connect_to(ps2);


}
