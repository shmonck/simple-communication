#pragma once

#include <xmodem/packet.h>
#include <xmodem/symbol.h>

#include <string_view>

namespace XModem
{
    /**
     * @class ErrorDetection
     *
     * @brief Generic error detection class
     */
    class ErrorDetection
    {
    public:
        /**
         * @brief Appends verification bytes (like checksum) to the packet
         *
         * @param packet The packet to append to
         * @param data Pointer to the data to do the verification on
         * @param length Data length
         */
        virtual void
        append_verification_bytes(Packet& packet, const char* const data, const std::size_t length) const = 0;

        /**
         * @brief Verifies the packet
         *
         * @param packet The packet to verify
         * @param data Pointer to the data to do the verification on
         * @param length Data length
         * @return true Verification is successful
         * @return false Verification failed
         */
        virtual bool
        is_verification_ok(const Packet& packet, const char* const data, const std::size_t length) const = 0;

        /**
         * @brief Returns the symbol used to initialize the transmission
         */
        virtual Symbol get_init_symbol() const = 0;

        /**
         * @brief Returns the total packet size (header + packet data + verification bytes)
         */
        virtual std::size_t get_total_packet_size() const = 0;

        /**
         * @brief Returns method name string
         */
        virtual std::string_view get_method_name() const = 0;
    };
}  // namespace XModem