#ifndef TRACE_HASH_HPP_
#define TRACE_HASH_HPP_

#include <cstdio>

constexpr const char* str_end(const char *str) {
    return *str ? str_end(str + 1) : str;
}

constexpr bool str_slant(const char *str) {
    return *str == '\\' ? true : (*str ? str_slant(str + 1) : false);
}

constexpr const char* r_slant(const char* str) {
    return *str == '\\' ? (str + 1) : r_slant(str - 1);
}
constexpr const char* file_name(const char* str) {
    return str_slant(str) ? r_slant(str_end(str)) : str;
}


// CRC16 Table (zlib polynomial)
static constexpr uint16_t crc_table[] = {
    0x0000, 0x7707, 0xee0e
};

template<size_t idx>
constexpr uint16_t crc16(const char * str)
{
    static_assert(idx < (sizeof(crc_table) / sizeof(uint16_t)), "Trace string too big: %i<%i");
    return (crc16<idx-1>(str) >> 8) ^ crc_table[(crc16<idx-1>(str) ^ str[idx]) & 0x00FF];
}

// This is the stop-recursion function
template<>
constexpr uint16_t crc16<size_t(-1)>(const char * str)
{
    return 0xFFFF;
}

// This doesn't take into account the nul char
#define COMPILE_TIME_CRC16_STR(x) (crc16<sizeof((x))-2>(x) ^ 0xFFFF)
//#define COMPILE_TIME_CRC16_STR(x) (crc16<3>(x) ^ 0xFFFF)

#endif /* TRACE_HASH_HPP_*/