#pragma once

#include <io/device.h>
#include <io/handle.h>

#include <string_view>

namespace io
{
    class Serial : public Device
    {
        Handle m_handle;

    public:
        ~Serial();

        Error open(const std::string& name);
        void close();

        Error set_baud_rate(const int baud_rate);
        Error set_read_timeout(const int timeout_ms);
        Error set_write_timeout(const int timeout_ms);

        virtual Error write(const std::uint8_t* const data, const std::size_t length) final;
        virtual Error read(std::uint8_t* const buffer, const std::size_t length) final;
        virtual Error readsome(std::uint8_t* const buffer, const std::size_t length, std::size_t& read_bytes_n) final;
    };
}  // namespace io