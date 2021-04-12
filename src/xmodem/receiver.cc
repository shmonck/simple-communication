#include <xmodem/receiver.h>

namespace XModem
{
    bool Receiver::initialize_transmission() const
    {
        const Symbol init_symbol = m_error_detection->get_init_symbol();

        // TODO: Move this somewhere else
        constexpr std::size_t try_count{ 10 };

        // TODO: Consider refactoring some repeatability in this and Transmitter method

        for ( std::size_t i = 0; i < try_count; i++ )
        {
            if ( !m_serial->write(&init_symbol, sizeof(Symbol)).is_ok() )
            {
                return false;
            }

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

            switch ( recv_symbol )
            {
                case Symbol::SOH: return true;

                case Symbol::EOT: [[fallthrough]];
                case Symbol::ETB: [[fallthrough]];
                case Symbol::CAN: return false;

                default: break;
            }
        }

        return false;
    }

    bool Receiver::is_packet_valid(const Packet& packet, const std::uint16_t packet_n) const
    {
        if ( packet.size() != m_error_detection->get_total_packet_size() )
        {
            return false;
        }

        if ( static_cast<Symbol>(packet.front()) != Symbol::SOH )
        {
            return false;
        }

        if ( static_cast<std::uint8_t>(packet.at(1)) != static_cast<std::uint8_t>(packet_n) )
        {
            return false;
        }

        if ( static_cast<std::uint8_t>(packet.at(1)) + static_cast<std::uint8_t>(packet.at(2)) != 255 )
        {
            return false;
        }

        // + 3 to skip the SOH, Packet number and Inv packet number
        if ( !m_error_detection->is_verification_ok(packet, packet.data() + 3, PACKET_DATA_SIZE) )
        {
            return false;
        }

        return true;
    }

    Receiver::Receiver(IO::Serial& serial, ErrorDetection& checksum) : Endpoint(serial, checksum)
    {
    }

    bool Receiver::receive(std::ostream& ostream)
    {
        // TODO: Return more error details

        if ( !initialize_transmission() )
        {
            return false;
        }

        std::uint16_t packet_n{ 1 };

        Packet packet;
        packet.resize(m_error_detection->get_total_packet_size());
        packet.at(0) = static_cast<char>(Symbol::SOH);

        std::size_t got_bytes_n{ 1 };

        bool done{ false };
        while ( !done )
        {
            while ( got_bytes_n < packet.size() )
            {
                std::size_t read_bytes_n;

                if ( !m_serial->readsome(packet.data() + got_bytes_n, packet.size() - got_bytes_n, read_bytes_n)
                          .is_ok() )
                {
                    return false;
                }

                if ( read_bytes_n == 1 )
                {
                    Symbol recv_symbol = static_cast<Symbol>(packet.at(got_bytes_n));

                    switch ( recv_symbol )
                    {
                        case Symbol::EOT: [[fallthrough]];
                        case Symbol::ETB:
                        {
                            done = true;
                            break;
                        }

                        case Symbol::CAN: return false;

                        default: break;
                    }
                }

                if ( done )
                {
                    break;
                }

                got_bytes_n += read_bytes_n;
            }

            if ( done )
            {
                break;
            }

            Symbol resp_symbol;

            if ( is_packet_valid(packet, packet_n) )
            {
                resp_symbol = Symbol::ACK;
                ostream.write(packet.data() + 3, PACKET_DATA_SIZE);  // + 3 to skip the header

                packet_n++;
                if ( packet_n == 256 )
                {
                    packet_n = 0;
                }
            }
            else
            {
                resp_symbol = Symbol::NAK;
            }

            if ( !m_serial->write(&resp_symbol, sizeof(Symbol)).is_ok() )
            {
                return false;
            }

            got_bytes_n = 0;
        }

        Symbol resp_symbol{ Symbol::ACK };
        m_serial->write(&resp_symbol, sizeof(Symbol));

        return true;
    }
}  // namespace XModem
