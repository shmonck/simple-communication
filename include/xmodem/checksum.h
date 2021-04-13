#pragma once

#include <xmodem/defines.h>
#include <xmodem/error_detection.h>

namespace XModem
{
    /**
     * @class Checksum
     */
    class Checksum : public ErrorDetection
    {
        /**
         * @brief Calculates the checksum
         *
         * @param data Pointer to the data to calculate the checksum of
         * @param length Data length
         * @return std::uint8_t The checksum
         */
        [[nodiscard]] static std::uint8_t checksum(const char* const data, const std::size_t length);

    public:
        virtual void
        append_verification_bytes(Packet& packet, const char* const data, const std::size_t length) const final;

        [[nodisacrd]] virtual bool
        is_verification_ok(const Packet& packet, const char* const data, const std::size_t length) const final;

        [[nodiscard]] inline virtual Symbol get_init_symbol() const final
        {
            return Symbol::NAK;
        }

        [[nodiscard]] inline virtual std::size_t get_total_packet_size() const final
        {
            // SOH, Packet number, Inv packet number, Checksum byte
            return 3 + PACKET_DATA_SIZE + 1;
        }

        [[nodiscard]] inline virtual std::string_view get_method_name() const final
        {
            return "Checksum";
        }
    };
}  // namespace XModem