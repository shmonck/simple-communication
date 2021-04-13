#pragma once

#include <io/device.h>
#include <io/handle.h>

#if defined(PLATFORM_WINDOWS)
    #include <windows.h>
#elif defined(PLATFORM_LINUX)
    #include <fcntl.h>
    #include <termios.h>
    #include <unistd.h>
#endif

#include <string_view>

namespace IO
{
    /**
     * @class Serial
     *
     * @brief Serial device
     *
     */
    class Serial : public Device
    {
        /// OS specific handle
        Handle m_handle{ IO_NO_HANDLE };

    public:
        /**
         * @brief Baud rate
         *
         * @todo Currently not all available baud rates are specified
         *
         */
        enum class BaudRate
        {
#if defined(PLATFORM_WINDOWS)
            BAUDRATE_9600 = CBR_9600,
#elif defined(PLATFORM_LINUX)
            BAUDRATE_9600 = B9600,
#endif
        };

        /**
         * @brief Destroy the Serial object
         *
         * @note The destructor calls Serial::close()
         */
        ~Serial();

        /**
         * @brief Opens serial on specified port
         *
         * @param name Port name
         * @return Error Error status
         */
        Error open(const std::string& name);

        /**
         * @brief Closes the port
         *
         * @note This function is called in the destructor
         *
         */
        void close();

        /**
         * @brief Set the baud rate
         *
         * @param baud_rate Requested baud rate
         * @return Error Error status
         */
        Error set_baud_rate(const BaudRate baud_rate);

        /**
         * @brief Set the read timeout in ms
         *
         * @note Linux based OS'es support only tenths of the seconds
         *
         * @param timeout_ms Timeout in ms
         * @return Error Error status
         */
        Error set_read_timeout(const int timeout_ms);

        /**
         * @brief Set the write timeout in ms
         *
         * @note Linux based OS'es support only tenths of the seconds
         *
         * @param timeout_ms Timeout in ms
         * @return Error Error status
         */
        Error set_write_timeout(const int timeout_ms);

        virtual Error write(const void* const data, const std::size_t length) final;
        virtual Error read(void* const buffer, const std::size_t length) final;
        virtual Error readsome(void* const buffer, const std::size_t length, std::size_t& read_bytes_n) final;
    };
}  // namespace IO