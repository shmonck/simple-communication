#pragma once

#include <xmodem/endpoint.h>

#include <iostream>

namespace XModem
{
    /**
     * @class Receiver
     *
     * @brief Represents the XModem receiver
     */
    class Receiver : public Endpoint
    {
        virtual bool initialize_transmission() final;
        virtual bool finalize_transmission() const final;

        /**
         * @brief Checks if the packet is valid
         *
         * @param packet The packet to check
         * @param packet_n Packet number
         * @return true If the packet is valid
         * @return false If the packet is invalid
         */
        [[nodiscard]] bool is_packet_valid(const Packet& packet, const std::uint8_t packet_n) const;

        /**
         * @brief Error detection object
         *
         * @note The user is responsible for the ErrorDetection object's lifetime
         */
        const ErrorDetection* const m_error_detection;

    public:
        Receiver(IO::Serial& serial, const ErrorDetection& error_detection);

        /**
         * @brief Receives a file
         *
         * @param ostream The ostream object to write the file data to
         * @return true If the file was received successfuly
         * @return false If error occured
         */
        bool receive(std::ostream& ostream);
    };
}  // namespace XModem