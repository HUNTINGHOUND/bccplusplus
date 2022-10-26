#ifndef util_hpp
#define util_hpp

// system headers
#include <stdexcept>
#include <chrono>

// local header
#include "move.hpp"

using U64 = unsigned long long;

inline int count_bits(U64 bitboard) {
    return __builtin_popcountll(bitboard);
}

inline int get_ls1b_index(U64 bitboard) {
    if (!bitboard) throw std::invalid_argument("cannot get lease significant bit of empty board");
    return __builtin_ffsll(bitboard) - 1;
}

// pseudo random number state
extern unsigned int random_state;

U64 get_random_U64_number();

inline std::chrono::time_point<std::chrono::system_clock> get_time_point() {
    return std::chrono::system_clock::now();
}

inline long long get_time_diff(std::chrono::time_point<std::chrono::system_clock> const & a, std::chrono::time_point<std::chrono::system_clock> const & b) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count();
}

inline long long get_time_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
#endif /* util_hpp */
