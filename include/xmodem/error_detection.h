#pragma once

#include <xmodem/packet.h>
#include <xmodem/symbol.h>

#include <string_view>

namespace XModem
{
    class ErrorDetection
    {
    public:
        virtual void append_verification_bytes(Packet& packet, const char* const data, const std::size_t length) const = 0;
        virtual bool is_verification_ok(const Packet& packet, const char* const data, const std::size_t length) const = 0;
        virtual Symbol get_init_symbol() const = 0;
        virtual std::size_t get_total_packet_size() const = 0;
        virtual std::string_view get_method_name() const = 0;
    };
}  // namespace XModem