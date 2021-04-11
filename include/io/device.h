#pragma once

#include <io/non_copyable.h>

#include <cstdint>

namespace io
{
    class Error
    {
    public:
        enum class Status
        {
            OK
        };

        Status m_status{ Status::OK };
        int m_code{ 0 };

        [[nodiscard]] inline bool is_ok() const
        {
            return m_status == Status::OK;
        }

        [[nodiscard]] const std::string_view get_error_string() const;
    };

    class Device : public NonCopyable
    {
    public:
        virtual ~Device();

        virtual Error write(const std::uint8_t* const data, const std::size_t length) = 0;
        virtual Error read(std::uint8_t* const buffer, const std::size_t length) = 0;
        virtual Error readsome(std::uint8_t* const buffer, const std::size_t length, std::size_t& read_bytes_n) = 0;
    };
}  // namespace io