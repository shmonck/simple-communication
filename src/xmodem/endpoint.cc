#include <xmodem/endpoint.h>

namespace XModem
{
    Endpoint::Endpoint(IO::Serial& serial, ErrorDetection& error_detection) :
        m_serial(&serial), m_error_detection(&error_detection)
    {
    
    }
}