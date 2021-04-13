#pragma once

#include <io/device.h>

#include <vector>
#include <mutex>

namespace IO
{
    /**
     * @class PseudoSerial
     *
     * @brief Pseudo serial device used for testing
     *
     * @note Only supports write/read functions, does not actually emulate serial device
     */
    class PseudoSerial : public Device
    {
        PseudoSerial* m_connected_serial{ nullptr };

        std::vector<char> m_buffer;
        std::mutex m_buffer_mutex;
        std::condition_variable m_write_cv;

        std::size_t m_read_timeout_ms{ 0 };

    public:
        ~PseudoSerial() = default;

        void connect_to(PseudoSerial& pseudo_serial);

        inline void set_read_timeout(const int timeout_ms)
        {
            m_read_timeout_ms = timeout_ms;
        }

        virtual Error write(const void* const data, const std::size_t length) final;
        virtual Error readsome(void* const buffer, const std::size_t length, std::size_t& read_bytes_n) final;
    };
}  // namespace IO