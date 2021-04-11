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
        ErrorDetection* const m_checksum;

        Endpoint(IO::Serial& serial, ErrorDetection& checksum);

        virtual bool initialize_transmission() const = 0;
    };
}  // namespace XModem