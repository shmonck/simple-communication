#pragma once

#include <io/serial.h>
#include <xmodem/error_detection.h>

#include <fstream>

namespace XModem
{
    class Endpoint
    {
    protected:
        bool read_symbol(Symbol &symbol) const;
        bool send_symbol(const Symbol symbol) const;

        IO::Serial* const m_serial;

        Endpoint(IO::Serial& serial);

        virtual bool initialize_transmission() = 0;
        virtual bool finalize_transmission() const = 0;
    };
}  // namespace XModem