#pragma once

enum class Symbol : char
{
    SOH = 0x01,  // Start of header
    EOT = 0x04,  // End of transmission
    ACK = 0x06,  // Ackonwledge
    NAK = 0x15,  // Not acknowledge
    ETB = 0x17,  // End of block
    CAN = 0x18,  // Cancel
    ETF = 0x1A,  // End of the file
    C = 0x43,    // ASCII 'C'
};