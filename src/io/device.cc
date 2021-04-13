#include <io/device.h>

namespace IO
{
    Error Device::read(void* const buffer, const std::size_t length)
    {
        std::size_t all_read_bytes_n{ 0 };

        while ( all_read_bytes_n < length )
        {
            std::size_t read_bytes_n;
            Error error =
                readsome(static_cast<char*>(buffer) + all_read_bytes_n, length - all_read_bytes_n, read_bytes_n);

            if ( !error.is_ok() )
            {
                return error;
            }

            all_read_bytes_n += read_bytes_n;
        }

        return Error{};
    }
}