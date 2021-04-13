#include <io/pseudo_serial.h>

#include <cstring>

namespace IO
{
    bool PseudoSerial::connect_to(PseudoSerial& pseudo_serial)
    {
        if ( &pseudo_serial == this )
        {
            return false;
        }

        m_connected_serial = &pseudo_serial;
        pseudo_serial.m_connected_serial = this;

        return true;
    }

    std::size_t PseudoSerial::available()
    {
        std::lock_guard lock(m_buffer_mutex);
        return m_buffer.size();
    }

    Error PseudoSerial::write(const void* const data, const std::size_t length)
    {
        if ( length == 0 )
        {
            return Error{};
        }

        if ( m_connected_serial == nullptr )
        {
            return Error{
                .m_status = Error::Status::NO_DEVICE,
            };
        }

        {
            std::lock_guard lock(m_connected_serial->m_buffer_mutex);

            auto& other_buffer = m_connected_serial->m_buffer;
            const std::size_t prev_size = other_buffer.size();

            other_buffer.resize(prev_size + length);
            std::memcpy(other_buffer.data() + prev_size, data, length);
        }

        m_connected_serial->m_write_cv.notify_one();

        return Error{};
    }

    Error PseudoSerial::readsome(void* const buffer, const std::size_t length, std::size_t& read_bytes_n)
    {
        std::unique_lock lock(m_buffer_mutex);

        if ( m_buffer.empty() )
        {
            if ( m_read_timeout_ms > 0 )
            {
                if ( !m_write_cv.wait_for(
                         lock, std::chrono::milliseconds(m_read_timeout_ms), [&]() { return m_buffer.size() > 0; }) )
                {
                    read_bytes_n = 0;
                    return Error{};
                }
            }
            else
            {
                m_write_cv.wait(lock);
            }
        }

        read_bytes_n = std::min(m_buffer.size(), length);

        const std::size_t new_size = m_buffer.size() - read_bytes_n;

        std::memcpy(buffer, m_buffer.data() + new_size, read_bytes_n);

        m_buffer.resize(new_size);

        return Error{};
    }
}  // namespace IO