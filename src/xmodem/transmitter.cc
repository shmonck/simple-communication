#include <xmodem/transmitter.h>

#include <array>

namespace XModem
{
    bool Transmitter::initialize_transmission() const
    {
        // TODO: Deduce the error detection method by initial symbol

        // TODO: Move this somewhere else
        constexpr std::size_t try_count{ 10 };

        for ( std::size_t i = 0; i < try_count; i++ )
        {
            Symbol recv_symbol;
            std::size_t read_bytes_n;

            if ( !m_serial->readsome(&recv_symbol, sizeof(Symbol), read_bytes_n).is_ok() )
            {
                return false;
            }

            if ( read_bytes_n == 0 )
            {
                continue;
            }

            if ( recv_symbol == m_error_detection->get_init_symbol() )
            {
                return true;
            }
        }

        return false;
    }

    Transmitter::Transmitter(IO::Serial& serial, ErrorDetection& checksum) : Endpoint(serial, checksum)
    {
    }

    bool Transmitter::send(std::istream& istream)
    {
        // TODO: Return more error details

        if ( !initialize_transmission() )
        {
            return false;
        }

        std::uint16_t packet_n{ 1 };

        while ( true )
        {
            Packet packet;
            packet.resize(m_error_detection->get_total_packet_size());

            packet.at(0) = static_cast<char>(Symbol::SOH);
            packet.at(1) = static_cast<char>(packet_n);
            packet.at(2) = static_cast<char>(~packet_n);

            const auto prev_stream_pos = istream.tellg();
            istream.read(packet.data() + 3, PACKET_DATA_SIZE);  // + 3 to skip the previous bytes

            m_error_detection->append_verification_bytes(packet, packet.data() + 3, PACKET_DATA_SIZE);

            if ( !m_serial->write(packet.data(), packet.size()).is_ok() )
            {
                return false;
            }

            Symbol recv_symbol;
            std::size_t read_bytes_n;

            if ( !m_serial->readsome(&recv_symbol, sizeof(Symbol), read_bytes_n).is_ok() )
            {
                return false;
            }

            if ( read_bytes_n == 0 or recv_symbol == Symbol::NAK )
            {
                istream.clear();
                istream.seekg(prev_stream_pos);
            }
            else
            {
                if ( istream.peek() == EOF )
                {
                    break;
                }
                else
                {
                    packet_n++;

                    if ( packet_n == 256 )
                    {
                        packet_n = 0;
                    }
                }
            }
        }

        Symbol symbol;

        symbol = Symbol::EOT;
        if ( !m_serial->write(&symbol, sizeof(Symbol)).is_ok() )
        {
            return false;
        }

        symbol = Symbol::ETB;
        m_serial->write(&symbol, sizeof(Symbol));

        return true;
    }
}  // namespace XModem