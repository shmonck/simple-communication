#include <xmodem/transmitter.h>
#include <xmodem/checksum.h>
#include <xmodem/crc.h>

#include <spdlog/spdlog.h>

namespace XModem
{
    bool Transmitter::initialize_transmission()
    {
        spdlog::info("Initializing transmission");

        // TODO: Move this somewhere else
        constexpr std::size_t try_count{ 10 };

        for ( std::size_t i = 0; i < try_count; i++ )
        {
            spdlog::info("Try {}/{}", i + 1, try_count);

            Symbol recv_symbol;

            if ( !read_symbol(recv_symbol) )
            {
                return false;
            }

            switch (recv_symbol)
            {
                case Symbol::INV:
                {
                    spdlog::warn("Read timeout");
                    break;
                }

                case Symbol::NAK:
                {
                    spdlog::info("Initialization successful. Receiver wants to use checksum");
                    m_error_detection = std::make_unique<Checksum>();
                    return true;
                }

                case Symbol::C:
                {
                    spdlog::info("Initialization successful. Receiver wants to use CRC");
                    m_error_detection = std::make_unique<CRC>();
                    return true;
                }
                
                default:
                {
                    spdlog::error("Invalid symbol");
                    break;
                }
            }
        }

        spdlog::error("Initialization failed");
        return false;
    }

    bool Transmitter::finalize_transmission() const
    {
        spdlog::info("Finalizing transmission");

        if ( !send_symbol(Symbol::EOT) )
        {
            return false;
        }

        while ( true )
        {
            spdlog::info("Waiting for ACK");

            Symbol recv_symbol;

            if ( !read_symbol(recv_symbol) )
            {
                return false;
            }

            bool success{ false };

            switch ( recv_symbol )
            {
                case Symbol::INV:
                {
                    spdlog::warn("Read timeout");
                    break;
                }

                case Symbol::ACK:
                {
                    spdlog::info("Transmission finalized successfully");
                    success = true;
                    break;
                }

                default:
                {
                    spdlog::error("Invalid symbol");
                    return false;
                }
            }

            if ( success )
            {
                break;
            }
        }

        if ( !send_symbol(Symbol::ETB) )
        {
            return false;
        }

        return true;
    }

    Transmitter::Transmitter(IO::Device& device) : Endpoint(device)
    {
    }

    bool Transmitter::send(std::istream& istream)
    {
        // TODO: Return more error details

        if ( !initialize_transmission() )
        {
            return false;
        }

        std::size_t packet_n{ 1 };

        while ( true )
        {
            Packet packet;
            packet.reserve(m_error_detection->get_total_packet_size());

            packet.push_back(static_cast<char>(Symbol::SOH));
            packet.push_back(static_cast<char>(packet_n % 256));
            packet.push_back(static_cast<char>(255 - (packet_n % 256)));

            packet.resize(packet.size() + PACKET_DATA_SIZE);

            const auto packet_data = packet.data() + 3; // + 3 to skip the header
            std::memset(packet_data, static_cast<char>(Symbol::ETF), PACKET_DATA_SIZE);

            const auto prev_stream_pos = istream.tellg();
            istream.read(packet_data, PACKET_DATA_SIZE);  

            m_error_detection->append_verification_bytes(packet, packet_data, PACKET_DATA_SIZE);

            spdlog::info("Sending packet number {}", packet_n);

            if ( !m_device->write(packet.data(), packet.size()).is_ok() )
            {
                return false;
            }

            spdlog::info("Waiting for response");

            Symbol recv_symbol;

            if ( !read_symbol(recv_symbol) )
            {
                return false;
            }

            bool success{ false };

            switch (recv_symbol)
            {
                case Symbol::INV:
                {
                    spdlog::warn("Read timeout");
                    break;
                }

                case Symbol::NAK:
                {
                    spdlog::warn("The packet was rejected");
                    break;
                }

                case Symbol::CAN:
                {
                    spdlog::warn("Transmission cancelled");
                    return false;
                }

                case Symbol::ACK:
                {
                    spdlog::info("Packet accepted");
                    success = true;
                    break;
                }

                default:
                {
                    spdlog::warn("Unexcepted symbol");
                    break;
                }
            }

            if ( success )
            {
                if ( istream.peek() == EOF )
                {
                    spdlog::info("End of the file");
                    return finalize_transmission();
                }
                else
                {
                    packet_n++;
                }
            }
            else
            {
                istream.clear();
                istream.seekg(prev_stream_pos);
            }
        }

        return false;
    }
}  // namespace XModem