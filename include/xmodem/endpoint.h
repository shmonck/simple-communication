#pragma once

#include <io/serial.h>
#include <xmodem/error_detection.h>

#include <fstream>

namespace XModem
{
    class Endpoint
    {
    protected:
        IO::Serial* const m_serial;
        ErrorDetection* const m_error_detection;

        Endpoint(IO::Serial& serial, ErrorDetection& error_detection);

        virtual bool initialize_transmission() const = 0;
    };
}  // namespace XModem