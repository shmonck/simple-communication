#include <xmodem/transmitter.h>

#include <spdlog/spdlog.h>

namespace XModem
{
    bool Transmitter::initialize_transmission() const
    {
        spdlog::info("Initializing transmission");
        spdlog::info("Expecting symbol: {0:x}", static_cast<char>(m_error_detection->get_init_symbol()));

        // TODO: Deduce the error detection method by initial symbol

        // TODO: Move this somewhere else
        constexpr std::size_t try_count{ 10 };

        for ( std::size_t i = 0; i < try_count; i++ )
        {
            spdlog::info("Try {}/{}", i + 1, try_count);

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

            if ( recv_symbol == m_error_detection->get_init_symbol() )
            {
                spdlog::info("Initialization successful");
                return true;
            }
        }

        spdlog::error("Initialization failed");
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
            packet.reserve(m_error_detection->get_total_packet_size());

            packet.push_back(static_cast<char>(Symbol::SOH));
            packet.push_back(static_cast<char>(packet_n));
            packet.push_back(static_cast<char>(255 - packet_n));

            packet.resize(packet.size() + PACKET_DATA_SIZE);

            const auto packet_data = packet.data() + 3; // + 3 to skip the header
            std::memset(packet_data, static_cast<char>(Symbol::ETF), PACKET_DATA_SIZE);

            const auto prev_stream_pos = istream.tellg();
            istream.read(packet_data, PACKET_DATA_SIZE);  

            m_error_detection->append_verification_bytes(packet, packet_data, PACKET_DATA_SIZE);

            spdlog::info("Sending packet number {}", packet_n);

            if ( !m_serial->write(packet.data(), packet.size()).is_ok() )
            {
                spdlog::error("Error occured during serial write");
                return false;
            }

            spdlog::info("Waiting for response");

            Symbol recv_symbol;
            std::size_t read_bytes_n;

            if ( !m_serial->readsome(&recv_symbol, sizeof(Symbol), read_bytes_n).is_ok() )
            {
                spdlog::error("Error occured during serial read");
                return false;
            }

            if ( read_bytes_n == 0 or recv_symbol == Symbol::NAK )
            {
                spdlog::warn("The packet was rejected or serial read timeout");

                istream.clear();
                istream.seekg(prev_stream_pos);
            }
            else
            {
                spdlog::info("Packet accepted");

                if ( istream.peek() == EOF )
                {
                    spdlog::info("End of the file");
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

        spdlog::info("Sending EOT");

        Symbol symbol;

        symbol = Symbol::EOT;
        if ( !m_serial->write(&symbol, sizeof(Symbol)).is_ok() )
        {
            spdlog::error("Error occured during serial write");
            return false;
        }

        spdlog::info("Sending ETB");

        symbol = Symbol::ETB;
        m_serial->write(&symbol, sizeof(Symbol));

        return true;
    }
}  // namespace XModem