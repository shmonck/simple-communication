#pragma once

#include <io/device.h>

#include <condition_variable>
#include <mutex>
#include <vector>

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
        /// Pointer to another serial this serial is connected to. User is responsible for PseudoSerial object lifetime
        PseudoSerial* m_connected_serial{ nullptr };

        std::vector<char> m_buffer;
        std::mutex m_buffer_mutex;
        std::condition_variable m_write_cv;

        std::size_t m_read_timeout_ms{ 0 };

    public:
        ~PseudoSerial() = default;

        /**
         * @brief Connect to another pseudo serial
         *
         * @param pseudo_serial The serial to connect to
         * @return true Connected successfuly
         * @return false Connection failed (cannot connect the serial to itself)
         */
        bool connect_to(PseudoSerial& pseudo_serial);

        /**
         * @brief Set the read timeout in ms
         *
         * @param timeout_ms The timeout in ms
         */
        inline void set_read_timeout(const int timeout_ms)
        {
            m_read_timeout_ms = timeout_ms;
        }

        virtual Error write(const void* const data, const std::size_t length) final;
        virtual Error readsome(void* const buffer, const std::size_t length, std::size_t& read_bytes_n) final;
    };
}  // namespace IO