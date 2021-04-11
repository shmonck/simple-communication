#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>

namespace XModem
{
    class Packet
    {
        std::vector<char> m_data;

    public:
        void append(const void* const data, const std::size_t length);
        bool read(void* const buffer, const std::size_t length);

        [[nodiscard]] inline const char* get_data() const
        {
            return m_data.data();
        }

        [[nodiscard]] inline std::size_t size() const
        {
            return m_data.size();
        }

        template <class T>
        requires std::is_standard_layout_v<T> && std::is_trivial_v<T>
        Packet& operator<<(const T& t)
        {
            append(&t, sizeof(T));
            return *this;
        }

        template <class T>
        requires std::is_standard_layout_v<T> && std::is_trivial_v<T>
        Packet& operator>>(T& t)
        {
            read(&t, sizeof(T));
            return *this;
        }
    };
}  // namespace transfer