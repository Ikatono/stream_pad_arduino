#ifndef H_FF2FF9A6BC114C719F44441B6FD238C2
#define H_FF2FF9A6BC114C719F44441B6FD238C2

#include <cstdint>

namespace Configuration
{
    struct Pixel
    {
        uint32_t value;
        //should check for endianness I think
        //but there is no fully compliant way to test at compile time
        #if defined(__BYTE_ORDER__)&&(__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
        uint8_t& green() { return *(reinterpret_cast<uint8_t*>(value)+0); }
        uint8_t& red()   { return *(reinterpret_cast<uint8_t*>(value)+1); }
        uint8_t& blue()  { return *(reinterpret_cast<uint8_t*>(value)+2); }
        uint8_t& white() { return *(reinterpret_cast<uint8_t*>(value)+3); }
        #else
        uint8_t& green() { return *(reinterpret_cast<uint8_t*>(value)+3); }
        uint8_t& red()   { return *(reinterpret_cast<uint8_t*>(value)+2); }
        uint8_t& blue()  { return *(reinterpret_cast<uint8_t*>(value)+1); }
        uint8_t& white() { return *(reinterpret_cast<uint8_t*>(value)+0); }
        #endif
    };
}

#endif //H_FF2FF9A6BC114C719F44441B6FD238C2
