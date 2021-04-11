#include <xmodem/crc.h>

namespace XModem
{
    std::uint16_t CRC::crc(const char* const data, const std::size_t length)
    {
        std::uint16_t crc{ 0 };

        for ( std::size_t i = 0; i < length; i++ )
        {
            crc ^= (static_cast<std::uint16_t>(data[i]) << 8);

            for ( int j = 0; j < 8; j++ )
            {
                if ( crc & 0x8000 )
                {
                    crc <<= 1;
                    crc ^= 0x1021;
                }
                else
                {
                    crc <<= 1;
                }
            }
        }

        return crc;
    }

    void CRC::append_verification_bytes(Packet& packet, const char* const data, const std::size_t length) const
    {
        const std::uint16_t crc_value = crc(data, length);

        const std::uint8_t crc_hi = static_cast<std::uint8_t>((crc_value & 0xFF00) >> 8);
        const std::uint8_t crc_lo = static_cast<std::uint8_t>(crc_value & 0x00FF);

        packet.push_back(crc_hi);
        packet.push_back(crc_lo);
    }

    bool CRC::is_verification_ok(const Packet& packet, const char* const data, const std::size_t length) const
    {
        const std::uint8_t crc_hi = packet.at(packet.size() - 2);
        const std::uint8_t crc_lo = packet.at(packet.size() - 1);

        const std::uint16_t crc_value =
            (static_cast<std::uint16_t>(crc_hi) << 8) | (static_cast<std::uint16_t>(crc_lo));

        return crc(data, length) == crc_value;
    }
}  // namespace XModem