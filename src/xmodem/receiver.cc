#include <xmodem/receiver.h>

namespace XModem
{
    bool Receiver::initialize_transmission() const
    {
        // TODO: Add try count

        const Symbol init_symbol = m_error_detection->get_init_symbol();

        while (true)
        {
            if ( !m_serial->write(&init_symbol, sizeof(Symbol)).is_ok())
            {
                return false;
            }

            Symbol recv_symbol;

            if ( !m_serial->read(&recv_symbol, sizeof(Symbol)).is_ok() )
            {
                return false;
            }

            if (recv_symbol == Symbol::SOH)
            {
                return true;
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

    void Receiver::receive(std::ostream& ostream)
    {
    }
}  // namespace XModem
