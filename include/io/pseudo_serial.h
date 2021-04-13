#pragma once

#include <io/device.h>

#include <vector>

namespace IO
{
    /**
     * @class PseudoSerial
     *
     * @brief Pseudo serial device used for testing
     *
     * @note Only supports write/read functions, does not actually emulate serial device
     */
    class PseudoSerial : public Device
    {
        PseudoSerial* m_connected_serial{ nullptr };

        std::vector<char> m_buffer;

    public:
        ~PseudoSerial() = default;

        void connect_to(PseudoSerial& pseudo_serial);

        virtual Error write(const void* const data, const std::size_t length) final;
        virtual Error read(void* const buffer, const std::size_t length) final;
        virtual Error readsome(void* const buffer, const std::size_t length, std::size_t& read_bytes_n) final;
    };
}  // namespace IO