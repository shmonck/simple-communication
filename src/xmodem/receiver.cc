#include <xmodem/receiver.h>

#include <spdlog/spdlog.h>

namespace XModem
{
    bool Receiver::initialize_transmission() const
    {
        const Symbol init_symbol = m_error_detection->get_init_symbol();

        spdlog::info("Initializing transmission");
        spdlog::info("Expecting symbol: {0:x}", static_cast<char>(init_symbol));

        // TODO: Move this somewhere else
        constexpr std::size_t try_count{ 10 };

        // TODO: Consider refactoring some repeatability in this and Transmitter method

        for ( std::size_t i = 0; i < try_count; i++ )
        {
            spdlog::info("Try {}/{}", i + 1, try_count);
            spdlog::info("Sending the initial symbol");

            if ( !m_serial->write(&init_symbol, sizeof(Symbol)).is_ok() )
            {
                spdlog::error("Error occured during serial write");
                return false;
            }

            spdlog::info("Waiting for the response");

            Symbol recv_symbol;
            std::size_t read_bytes_n;

            if ( !m_serial->readsome(&recv_symbol, sizeof(Symbol), read_bytes_n).is_ok() )
            {
                spdlog::error("Error occured during serial read");
                return false;
            }

            if ( read_bytes_n == 0 )
            {
                spdlog::warn("No bytes received");
                continue;
            }

            spdlog::info("Received symbol: {0:x}", static_cast<char>(recv_symbol));

            switch ( recv_symbol )
            {
                case Symbol::SOH:
                {
                    spdlog::info("Initialization succesful");
                    return true;
                }

                case Symbol::EOT: [[fallthrough]];
                case Symbol::ETB: [[fallthrough]];
                case Symbol::CAN: 
                {
                    spdlog::warn("EOT or transmission cancelled");
                    return false;
                }

                default: 
                {
                    spdlog::error("Invalid symbol");
                    return false;
                }
            }
        }

        spdlog::error("Initialization failed");
        return false;
    }

    bool Receiver::is_packet_valid(const Packet& packet, const std::uint16_t packet_n) const
    {
        if ( packet.size() != m_error_detection->get_total_packet_size() )
        {
            spdlog::warn("Invalid packet size");
            return false;
        }

        if ( static_cast<Symbol>(packet.front()) != Symbol::SOH )
        {
            spdlog::warn("Invalid packet header symbol");
            return false;
        }

        if ( static_cast<std::uint8_t>(packet.at(1)) != static_cast<std::uint8_t>(packet_n) )
        {
            spdlog::warn("Invalid packet number");
            return false;
        }

        if ( static_cast<std::uint8_t>(packet.at(1)) + static_cast<std::uint8_t>(packet.at(2)) != 255 )
        {
            spdlog::warn("Invalid packet inverse number");
            return false;
        }

        // + 3 to skip the SOH, Packet number and Inv packet number
        if ( !m_error_detection->is_verification_ok(packet, packet.data() + 3, PACKET_DATA_SIZE) )
        {
            spdlog::warn("Error detected");
            return false;
        }

        return true;
    }

    Receiver::Receiver(IO::Serial& serial, ErrorDetection& error_detection) : Endpoint(serial, error_detection)
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
            spdlog::info("Receiving packet number {}", packet_n);

            while ( got_bytes_n < packet.size() )
            {
                spdlog::info("Got {}/{} bytes", got_bytes_n, packet.size());

                std::size_t read_bytes_n;

                if ( !m_serial->readsome(packet.data() + got_bytes_n, packet.size() - got_bytes_n, read_bytes_n)
                          .is_ok() )
                {
                    spdlog::error("Error occured during serial read");
                    return false;
                }

                spdlog::info("Received {} bytes", read_bytes_n);

                // TODO: This check isn't really valid, fix this

                if ( read_bytes_n == 1 )
                {
                    Symbol recv_symbol = static_cast<Symbol>(packet.at(got_bytes_n));

                    switch ( recv_symbol )
                    {
                        case Symbol::EOT: [[fallthrough]];
                        case Symbol::ETB:
                        {
                            spdlog::info("End of transmission");
                            done = true;
                            break;
                        }

                        case Symbol::CAN: 
                        {
                            spdlog::warn("Transmission cancelled");
                            return false;
                        }

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
                spdlog::info("The packet is valid");

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

            spdlog::info("Sending response symbol");

            if ( !m_serial->write(&resp_symbol, sizeof(Symbol)).is_ok() )
            {
                spdlog::error("Error occured during serial write");
                return false;
            }

            got_bytes_n = 0;
        }

        Symbol resp_symbol{ Symbol::ACK };
        m_serial->write(&resp_symbol, sizeof(Symbol));

        return true;
    }
}  // namespace XModem
