#include <xmodem/transmitter.h>

namespace XModem
{
    bool Transmitter::initialize_transmission() const
    {
        // TODO: Add try count

        while (true)
        {
            Symbol recv_symbol;

            if(!m_serial->read(&recv_symbol, sizeof(Symbol)).is_ok())
            {
                return false;
            }

            if (recv_symbol == m_error_detection->get_init_symbol())
            {
                return true;
            }
        }

        return false;
    }

    Transmitter::Transmitter(IO::Serial& serial, ErrorDetection& checksum) : Endpoint(serial, checksum)
    {
    }

    void Transmitter::send(std::istream& istream)
    {
    }
}  // namespace XModem