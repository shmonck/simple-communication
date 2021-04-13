#include <common/system.h>
#include <io/serial.h>

#include <string>

namespace IO
{
    Serial::~Serial()
    {
        close();
    }

    Error Serial::open(const std::string& name)
    {
#if defined(PLATFORM_WINDOWS)
        m_handle = CreateFile(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

        if ( m_handle == INVALID_HANDLE_VALUE )
        {
            m_handle = NULL;

            return MAKE_OS_ERROR();
        }
#elif defined(PLATFORM_LINUX)
        m_handle = ::open(name.c_str(), O_RDWR);

        if ( m_handle == -1 )
        {
            m_handle = 0;

            return MAKE_OS_ERROR();
        }
#endif
        return Error{};
    }

    void Serial::close()
    {
        if ( m_handle == IO_NO_HANDLE )
        {
            return;
        }

#if defined(PLATFORM_WINDOWS)
        CloseHandle(m_handle);
#elif defined(PLATFORM_LINUX)
        ::close(m_handle);
#endif
    }

    Error Serial::set_baud_rate(const BaudRate baud_rate)
    {
#if defined(PLATFORM_WINDOWS)
        DCB dcb;
        ZeroMemory(&dcb, sizeof(DCB));
        dcb.DCBlength = sizeof(dcb);

        OS_CALL_OR_ERROR(GetCommState(m_handle, &dcb));

        dcb.BaudRate = static_cast<DWORD>(baud_rate);

        OS_CALL_OR_ERROR(SetCommState(m_handle, &dcb));
#elif defined(PLATFORM_LINUX)
        termios termios_;

        OS_CALL_OR_ERROR(tcgetattr(m_handle, &termios_));
        OS_CALL_OR_ERROR(cfsetspeed(&termios_, static_cast<speed_t>(baud_rate)));
        OS_CALL_OR_ERROR(tcsetattr(m_handle, TCSANOW, &termios_));
#endif
        return Error{};
    }

    Error Serial::set_read_timeout(const int timeout_ms)
    {
#if defined(PLATFORM_WINDOWS)
        COMMTIMEOUTS commtimeouts;

        OS_CALL_OR_ERROR(GetCommTimeouts(m_handle, &commtimeouts));

        commtimeouts.ReadTotalTimeoutConstant = timeout_ms;

        OS_CALL_OR_ERROR(SetCommTimeouts(m_handle, &commtimeouts));
#elif defined(PLATFORM_LINUX)
        // TODO: Implement this
#endif
        return Error{};
    }

    Error Serial::set_write_timeout(const int timeout_ms)
    {
#if defined(PLATFORM_WINDOWS)
        COMMTIMEOUTS commtimeouts;

        OS_CALL_OR_ERROR(GetCommTimeouts(m_handle, &commtimeouts));

        commtimeouts.WriteTotalTimeoutConstant = timeout_ms;

        OS_CALL_OR_ERROR(SetCommTimeouts(m_handle, &commtimeouts));
#elif defined(PLATFORM_LINUX)
        // TODO: Implement this
#endif
        return Error{};
    }

    Error Serial::write(const void* const data, const std::size_t length)
    {
        std::size_t all_written_bytes_n{ 0 };

        while ( all_written_bytes_n < length )
        {
            std::size_t written_bytes_n;

#if defined(PLATFORM_WINDOWS)
            DWORD dword_written_bytes_n;

            OS_CALL_OR_ERROR(WriteFile(m_handle,
                                       static_cast<const char*>(data) + all_written_bytes_n,
                                       length - all_written_bytes_n,
                                       &dword_written_bytes_n,
                                       NULL));

            written_bytes_n = static_cast<std::size_t>(dword_written_bytes_n);
#elif defined(PLATFORM_LINUX)
            int int_written_bytes_n =
                ::write(m_handle, static_cast<const char*>(data) + all_written_bytes_n, length - all_written_bytes_n);

            if ( int_written_bytes_n == -1 )
            {
                return MAKE_OS_ERROR();
            }

            written_bytes_n = static_cast<std::size_t>(int_written_bytes_n);
#endif

            all_written_bytes_n += written_bytes_n;
        }

        return Error{};
    }

    Error Serial::readsome(void* const buffer, const std::size_t length, std::size_t& read_bytes_n)
    {
#if defined(PLATFORM_WINDOWS)
        DWORD dword_read_bytes_n;
        OS_CALL_OR_ERROR(ReadFile(m_handle, buffer, length, &dword_read_bytes_n, NULL));

        read_bytes_n = static_cast<std::size_t>(dword_read_bytes_n);
#elif defined(PLATFORM_LINUX)
        int int_read_bytes_n = ::read(m_handle, buffer, length);

        if ( int_read_bytes_n == -1 )
        {
            return MAKE_OS_ERROR();
        }
        else if ( int_read_bytes_n == 0 )
        {
            // TODO: Handle this case
        }

        read_bytes_n = static_cast<std::size_t>(int_read_bytes_n);
#endif
        return Error{};
    }
}  // namespace IO