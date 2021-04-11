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
    class Serial : public Device
    {
        Handle m_handle{ IO_NO_HANDLE };

    public:
        enum class BaudRate
        {
    #if defined(PLATFORM_WINDOWS)
            BAUDRATE_9600 = CBR_9600,
    #elif defined(PLATFORM_LINUX)
            BAUDRATE_9600 = B9600,
    #endif
        };

        ~Serial();

        Error open(const std::string& name);
        void close();

        Error set_baud_rate(const BaudRate baud_rate);
        Error set_read_timeout(const int timeout_ms);
        Error set_write_timeout(const int timeout_ms);

        virtual Error write(const void* const data, const std::size_t length) final;
        virtual Error read(void* const buffer, const std::size_t length) final;
        virtual Error readsome(void* const buffer, const std::size_t length, std::size_t& read_bytes_n) final;
    };
}