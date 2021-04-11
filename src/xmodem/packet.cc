#include <xmodem/packet.h>

/*
namespace XModem
{
    void Packet::append(const void* const data, const std::size_t length)
    {
        // NOTE! Byte order may vary depending on the platform. Should use ntoh/hton... functions

        std::size_t old_size = m_data.size();

        m_data.resize(m_data.size() + length);

        std::memcpy(m_data.data() + old_size, data, length);
    }

    bool Packet::read(void* const buffer, const std::size_t length)
    {
        // NOTE! Byte order may vary depending on the platform. Should use ntoh/hton... functions

        if ( length > m_data.size() )
        {
            return false;
        }

        std::size_t new_size = m_data.size() - length;

        std::memcpy(buffer, m_data.data() + new_size, length);

        m_data.resize(new_size);

        return true;
    }
}  // namespace XModem
*/