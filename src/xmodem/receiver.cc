#include <xmodem/receiver.h>

#include <spdlog/spdlog.h>

namespace XModem
{
    bool Receiver::initialize_transmission()
    {
        spdlog::info("Initializing transmission");

        // TODO: Move this somewhere else
        constexpr std::size_t try_count{ 10 };

        // TODO: Consider refactoring some repeatability in this and Transmitter method

        for ( std::size_t i = 0; i < try_count; i++ )
        {
            spdlog::info("Try {}/{}", i + 1, try_count);

            if ( !send_symbol(m_error_detection->get_init_symbol()) )
            {
                return false;
            }

            spdlog::info("Waiting for the response");

            Symbol recv_symbol;

            if ( !read_symbol(recv_symbol) )
            {
                return false;
            }

            switch ( recv_symbol )
            {
                case Symbol::INV:
                {
                    spdlog::warn("Read timeout");
                    break;
                }

                case Symbol::SOH:
                {
                    spdlog::info("Initialization succesful");
                    return true;
                }

                case Symbol::EOT: [[fallthrough]];
                case Symbol::ETB:
                {
                    spdlog::warn("End of transmission");
                    return false;
                }

                case Symbol::CAN: 
                {
                    spdlog::warn("Transmission cancelled");
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

    bool Receiver::finalize_transmission() const
    {
        spdlog::info("Finalizing transmission");

        if(!send_symbol(Symbol::ACK))
        {
            return false;
        }

        spdlog::info("Transmission finalized successfully");
        return true;
    }

    bool Receiver::is_packet_valid(const Packet& packet, const std::uint8_t packet_n) const
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

        if ( static_cast<std::uint8_t>(packet.at(1)) != packet_n )
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

    Receiver::Receiver(IO::Device& device, const ErrorDetection& error_detection) : Endpoint(device), m_error_detection(&error_detection)
    {
    }

    bool Receiver::receive(std::ostream& ostream)
    {
        // TODO: Return more error details

        if ( !initialize_transmission() )
        {
            return false;
        }

        std::size_t packet_n{ 1 };

        Packet packet;
        packet.resize(m_error_detection->get_total_packet_size());
        packet.at(0) = static_cast<char>(Symbol::SOH);

        std::size_t got_bytes_n{ 1 };

        bool done{ false };
        while ( !done )
        {
            spdlog::info("Receiving packet number {}", packet_n);

            while (got_bytes_n == 0)
            {
                Symbol recv_symbol;

                if ( !read_symbol(recv_symbol) )
                {
                    return false;
                }

                switch ( recv_symbol )
                {
                    case Symbol::INV:
                    {
                        spdlog::warn("Read timeout");
                        break;
                    }

                    case Symbol::SOH:
                    {
                        spdlog::info("Start of header received");

                        packet.at(got_bytes_n) = static_cast<char>(Symbol::SOH);
                        got_bytes_n++;

                        break;
                    }

                    case Symbol::EOT: [[fallthrough]];
                    case Symbol::ETB:
                    {
                        spdlog::info("End of transmission");
                        return finalize_transmission();
                    }

                    case Symbol::CAN:
                    {
                        spdlog::warn("Transmission cancelled");
                        return false;
                    }

                    default:
                    {
                        spdlog::error("Invalid symbol");
                        return false;
                    }
                }
            }

            while ( got_bytes_n < packet.size() )
            {
                spdlog::info("Got {}/{} bytes", got_bytes_n, packet.size());

                std::size_t read_bytes_n;

                if ( !m_device->readsome(packet.data() + got_bytes_n, packet.size() - got_bytes_n, read_bytes_n)
                          .is_ok() )
                {
                    spdlog::error("Error occured during serial read");
                    return false;
                }

                if (read_bytes_n == 0)
                {
                    spdlog::warn("Read timeout");
                    continue;
                }

                spdlog::info("Received {} bytes", read_bytes_n);

                got_bytes_n += read_bytes_n;
            }

            spdlog::info("Whole packet received");

            if ( is_packet_valid(packet, static_cast<std::uint8_t>(packet_n % 256)) )
            {
                spdlog::info("The packet is valid");
                
                ostream.write(packet.data() + 3, PACKET_DATA_SIZE);  // + 3 to skip the header

                packet_n++;

                if ( !send_symbol(Symbol::ACK) )
                {
                    return false;
                }
            }
            else
            {
                if ( !send_symbol(Symbol::NAK) )
                {
                    return false;
                }
            }

            got_bytes_n = 0;
        }

        return false;
    }
}  // namespace XModem
