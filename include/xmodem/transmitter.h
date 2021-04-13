#pragma once

#include <xmodem/endpoint.h>

#include <iostream>

namespace XModem
{
    class Transmitter : public Endpoint
    {
        virtual bool initialize_transmission() final;
        virtual bool finalize_transmission() const final;

        std::unique_ptr<ErrorDetection> m_error_detection;

    public:
        Transmitter(IO::Serial& serial);

        bool send(std::istream& istream);
    };
}  // namespace XModem