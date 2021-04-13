#pragma once

#include <io/serial.h>
#include <xmodem/error_detection.h>

#include <fstream>

namespace XModem
{
    /**
     * @class Endpoint
     *
     * @brief Represents the XModem endpoint
     */
    class Endpoint
    {
    protected:
        /**
         * @brief Reads a single symbol from the serial
         *
         * @param symbol The symbol to write to
         *
         * @note If read operation times out, the symbol is set to Symbol::INV
         *
         * @return true If the read operation was successful or timed out
         * @return false If the read operation failed
         */
        bool read_symbol(Symbol& symbol) const;

        /**
         * @brief Sends a single symbol to the serial
         *
         * @note The function does not check for timeout
         *
         * @param symbol The symbol to be sent
         * @return true If the write operation was successful or timed out
         * @return false If the read operation failed
         */
        bool send_symbol(const Symbol symbol) const;

        /**
         * @brief Weak reference to the serial object
         *
         * @note The user is responsible for the Serial object's lifetime
         */
        IO::Serial* const m_serial;

        Endpoint(IO::Serial& serial);

        /**
         * @brief Initializes the transmission
         *
         * @return true If the initialization was successful
         * @return false If the initialization failed
         */
        virtual bool initialize_transmission() = 0;

        /**
         * @brief Finalizes the transmission
         *
         * @return true If the finalization was successful
         * @return false If the finalization failed
         */
        virtual bool finalize_transmission() const = 0;
    };
}  // namespace XModem