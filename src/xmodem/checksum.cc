#include <xmodem/checksum.h>

namespace XModem
{
    std::uint8_t Checksum::checksum(const char* const data, const std::size_t length)
    {
        std::uint8_t sum{ 0 };

        for ( std::size_t i = 0; i < length; i++ )
        {
            sum += data[i];
        }

        return sum;
    }

    void Checksum::append_verification_bytes(Packet& packet, const char* const data, const std::size_t length) const
    {
        packet.push_back(checksum(data, length));
    }

    bool Checksum::is_verification_ok(const Packet& packet, const char* const data, const std::size_t length) const
    {
        std::uint8_t checksum_value = packet.back();
        
        return checksum(data, length) == checksum_value;
    }
}  // namespace XModem