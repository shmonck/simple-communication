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
         * @note The function will return false if @p pseudo_serial is this serial
         * @note This function is not thread safe and should be called before any write/read operation
         * @note Connecting two serials to one serial is undefined behavior
         *
         * @param pseudo_serial The serial to connect to
         */
        bool connect_to(PseudoSerial& pseudo_serial);

        /**
         * @brief Set the read timeout in ms
         *
         * @note This function is not thread safe and should only be used before calling any read operation
         *
         * @param timeout_ms The timeout in ms
         */
        inline void set_read_timeout(const int timeout_ms)
        {
            m_read_timeout_ms = timeout_ms;
        }

        /**
         * @brief Returns available bytes to read
         *
         * @note This function is thread safe
         *
         * @return std::size_t Available bytes to read
         */
        [[nodiscard]] std::size_t available();

        /// @note This function is thread safe
        virtual Error write(const void* const data, const std::size_t length) final;

        /// @note This function is thread safe
        virtual Error readsome(void* const buffer, const std::size_t length, std::size_t& read_bytes_n) final;
    };
}  // namespace IO
