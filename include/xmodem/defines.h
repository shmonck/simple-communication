#pragma once

#include <vector>

namespace XModem
{
    constexpr inline std::size_t PACKET_DATA_SIZE{ 128 };

    using Packet = std::vector<std::uint8_t>;
}