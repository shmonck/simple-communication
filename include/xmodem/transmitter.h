#pragma once

#include <xmodem/endpoint.h>

#include <iostream>

namespace XModem
{
    /**
     * @class Transmitter
     *
     * @brief Represents the XModem transmitter
     */
    class Transmitter : public Endpoint
    {
        virtual bool initialize_transmission() final;
        virtual bool finalize_transmission() const final;

        std::unique_ptr<ErrorDetection> m_error_detection;

    public:
        Transmitter(IO::Serial& serial);

        /**
         * @brief Sends a file
         *
         * @param istream The istream object to get the file data from
         * @return true If the file was sent successfully
         * @return false If error occured
         */
        bool send(std::istream& istream);
    };
}  // namespace XModem