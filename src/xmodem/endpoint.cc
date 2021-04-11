#include <xmodem/endpoint.h>

namespace XModem
{
    Endpoint::Endpoint(IO::Serial& serial, ErrorDetection& checksum) : m_serial(&serial), m_checksum(&checksum)
    {
    
    }
}