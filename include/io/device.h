#pragma once

#include <common/non_copyable.h>

#include <cstdint>
#include <string_view>

#if defined(IO_OS_WINDOWS)
    #define MAKE_OS_ERROR()                                                                                            \
        (io::Error{                                                                                                    \
            .m_status = Error::Status::OS_ERROR,                                                                       \
            .m_code = GetLastError(),                                                                                  \
        })

    #define OS_CALL_FAILED(c) (!(c))
#elif defined(IO_OS_LINUX)
    #define MAKE_OS_ERROR()                                                                                            \
        (io::Error{                                                                                                    \
            .m_status = Error::Status::OS_ERROR,                                                                       \
            .m_code = errno,                                                                                           \
        })

    #define OS_CALL_FAILED(c) ((c) == -1)

    #define OS_CALL(c)                                                                                                 \
        if ( OS_CALL_FAILED(c) )                                                                                       \
        {                                                                                                              \
            return MAKE_OS_ERROR();                                                                                    \
        }
#endif

namespace io
{
    class Error
    {
    public:
        enum class Status
        {
            OK,
            OS_ERROR,
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
        virtual ~Device() = default;

        virtual Error write(const void* const data, const std::size_t length) = 0;
        virtual Error read(void* const buffer, const std::size_t length) = 0;
        virtual Error readsome(void* const buffer, const std::size_t length, std::size_t& read_bytes_n) = 0;
    };
}  // namespace io