#pragma once

#include <common/non_copyable.h>

#include <cstdint>
#include <string_view>

#if defined(PLATFORM_WINDOWS)
    #define MAKE_OS_ERROR()                                                                                            \
        (IO::Error{                                                                                                    \
            .m_status = Error::Status::OS_ERROR,                                                                       \
            .m_code = static_cast<int>(GetLastError()),                                                                \
        })

    #define OS_CALL_OR_ERROR(c)                                                                                        \
        if ( (!(c)) )                                                                                                  \
        {                                                                                                              \
            return MAKE_OS_ERROR();                                                                                    \
        }
#elif defined(PLATFORM_LINUX)
    #define MAKE_OS_ERROR()                                                                                            \
        (IO::Error{                                                                                                    \
            .m_status = Error::Status::OS_ERROR,                                                                       \
            .m_code = errno,                                                                                           \
        })

    #define OS_CALL_OR_ERROR(c)                                                                                        \
        if ( ((c) == -1) )                                                                                             \
        {                                                                                                              \
            return MAKE_OS_ERROR();                                                                                    \
        }
#endif

namespace IO
{
    /**
     * @class Error
     *
     * @brief A class that holds error status and OS specific code
     *
     */
    class Error
    {
    public:
        enum class Status
        {
            /// No error
            OK = 0x00,

            /// OS specific error
            OS_ERROR = 0x01,

            /// No connected device (in case of PseudoTerminal)
            NO_DEVICE = 0x02,
        };

        /// Error status
        Status m_status{ Status::OK };

        /// OS specific error code
        int m_code{ 0 };

        /**
         * @brief Returns true if no error occured, otherwise false
         */
        [[nodiscard]] inline bool is_ok() const
        {
            return m_status == Status::OK;
        }

        /**
         * @brief Returns error code string if supported by OS
         *
         * @todo Currently not implemented
         */
        [[nodiscard]] const std::string_view get_error_string() const;
    };

    /**
     * @class Device
     *
     * @brief Generic Input/Output device class that allows reading and writing
     */
    class Device : public NonCopyable
    {
    public:
        virtual ~Device() = default;

        /**
         * @brief Writes to the device
         *
         * @param data Pointer to the data
         * @param length Data length
         * @return Error Error status
         */
        virtual Error write(const void* const data, const std::size_t length) = 0;

        /**
         * @brief Reads from the device
         *
         * @note This function will block until all specified bytes are read
         *
         * @param buffer Pointer to the buffer
         * @param length Data length
         * @return Error Error status
         */
        Error read(void* const buffer, const std::size_t length);

        /**
         * @brief Reads up to \p length bytes from the device
         *
         * @note This function will block until at least 1 byte is read, unless timeout is specified
         *
         * @param buffer Pointer to the buffer
         * @param length Data length
         * @param read_bytes_n Number of read bytes
         * @return Error Error status
         */
        virtual Error readsome(void* const buffer, const std::size_t length, std::size_t& read_bytes_n) = 0;
    };
}  // namespace IO