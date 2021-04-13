#pragma once

#include <xmodem/error_detection.h>

namespace XModem
{
    /**
     * @class CRC
     */
    class CRC : public ErrorDetection
    {
        /**
         * @brief Calculates the CRC
         *
         * @param data Pointer to the data to caluclate the CRC of
         * @param length Data length
         * @return std::uint16_t The CRC
         */
        [[nodiscard]] static std::uint16_t crc(const char* const data, const std::size_t length);

    public:
        virtual void
        append_verification_bytes(Packet& packet, const char* const data, const std::size_t length) const final;

        [[nodiscard]] virtual bool
        is_verification_ok(const Packet& packet, const char* const data, const std::size_t length) const final;

        [[nodiscard]] inline virtual Symbol get_init_symbol() const final
        {
            return Symbol::C;
        }

        [[nodiscard]] inline virtual std::size_t get_total_packet_size() const final
        {
            // SOH, Packet number, Inv packet number, CRC16
            return 3 + PACKET_DATA_SIZE + 2;
        }

        [[nodiscard]] inline virtual std::string_view get_method_name() const final
        {
            return "CRC";
        }
    };
}  // namespace XModem