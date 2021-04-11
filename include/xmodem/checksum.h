#pragma once

#include <xmodem/error_detection.h>
#include <xmodem/defines.h>

namespace XModem
{
	class Checksum : public ErrorDetection
    {
        static std::uint8_t checksum(const char* const data, const std::size_t length);

    public:
        virtual void
        append_verification_bytes(Packet& packet, const char* const data, const std::size_t length) const final;

        virtual bool verify_packet(const Packet& packet, const char* const data, const std::size_t length) const final;

        inline virtual Symbol get_init_symbol() const final
        {
            return Symbol::NAK;
        }

        inline virtual std::size_t get_total_packet_size() const final
        {
            // SOH, Packet number, Inv packet number, Checksum byte
            return 3 + PACKET_DATA_SIZE + 1;
        }

        inline virtual std::string_view get_method_name() const final
        {
            return "Checksum";
        }
    };
}