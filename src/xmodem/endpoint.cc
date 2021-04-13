#include <xmodem/endpoint.h>

#include <spdlog/spdlog.h>

namespace XModem
{
    bool Endpoint::read_symbol(Symbol& symbol) const
    {
        std::size_t read_bytes_n;

        if ( !m_serial->readsome(&symbol, sizeof(Symbol), read_bytes_n).is_ok() )
        {
            spdlog::error("Error occured during serial read");
            return false;
        }

        if ( read_bytes_n == 0 )
        {
            symbol = Symbol::INV;
        }
        else
        {
            spdlog::info("Received symbol: 0x{0:x} ({1})", static_cast<char>(symbol), symbol_string(symbol));
        }

        return true;
    }

    bool Endpoint::send_symbol(const Symbol symbol) const
    {
        spdlog::info("Sending symbol: 0x{0:x} ({1})", static_cast<char>(symbol), symbol_string(symbol));

        if ( !m_serial->write(&symbol, sizeof(Symbol)).is_ok() )
        {
            spdlog::error("Error occured during serial write");
            return false;
        }

        return true;
    }

    Endpoint::Endpoint(IO::Serial& serial) : m_serial(&serial)
    {
    }
}  // namespace XModem