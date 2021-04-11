#pragma once

enum class Symbol : char
{
    SOH = 0x01,
    EOT = 0x04,
    ACK = 0x06,
    NAK = 0x15,
    ETB = 0x17,
    CAN = 0x18,
    C = 0x43,
};