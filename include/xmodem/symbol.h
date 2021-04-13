#pragma once

#include <unordered_map>

namespace XModem
{
    enum class Symbol : char
    {
        INV = 0x00,  // Invalid symbol
        SOH = 0x01,  // Start of header
        EOT = 0x04,  // End of transmission
        ACK = 0x06,  // Ackonwledge
        NAK = 0x15,  // Not acknowledge
        ETB = 0x17,  // End of block
        CAN = 0x18,  // Cancel
        ETF = 0x1A,  // End of the file
        C = 0x43,    // ASCII 'C'
    };

    inline std::string_view symbol_string(const Symbol symbol)
    {
        // TODO: Consider using some macro for this

        const static std::unordered_map<Symbol, std::string_view> symbol_string_map{
            { Symbol::INV, "INV" }, { Symbol::SOH, "SOH" }, { Symbol::EOT, "EOT" },
            { Symbol::ACK, "ACK" }, { Symbol::NAK, "NAK" }, { Symbol::ETB, "ETB" },
            { Symbol::CAN, "CAN" }, { Symbol::ETF, "ETF" }, { Symbol::C, "C" },
        };

        try
        {
            return symbol_string_map.at(symbol);
        }
        catch ( ... )
        {
            return "UNKNOWN";
        }
    }
}  // namespace XModem