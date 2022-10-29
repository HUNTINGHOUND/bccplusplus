#ifndef util_hpp
#define util_hpp

// system headers
#include <stdexcept>
#include <chrono>
#include <iostream>

// local header
#include "move.hpp"
#include "types.hpp"

extern unsigned int random_state;

[[noreturn]] void fatal_exit(std::string const & s);

// generate 32-bit pseudo legal numbers
unsigned int get_random_U32_number();

U64 get_random_U64_number();


int count_bits(U64 bitboard);

int get_ls1b_index(U64 bitboard);

U64 get_random_U64_number();

std::chrono::time_point<std::chrono::system_clock> get_time_point();

long long get_time_diff(std::chrono::time_point<std::chrono::system_clock> const & a, std::chrono::time_point<std::chrono::system_clock> const & b);

long long get_time_ms();

#endif /* util_hpp */
