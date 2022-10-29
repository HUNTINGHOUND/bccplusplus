#include "util.hpp"

unsigned int random_state = 1804289383;

[[noreturn]] void fatal_exit(std::string const & s) {
    std::cerr << s << "\n";
    exit(1);
}

unsigned int get_random_U32_number() {
    // get current state
    unsigned int number = random_state;
    
    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    
    // update random number state
    random_state = number;
    
    // return random number
    return number;
}

U64 get_random_U64_number() {
    // define 4 random numbers
    U64 n1, n2, n3, n4;
    
    // init random numbers slicing 16 bits from MS1B side
    n1 = (U64)(get_random_U32_number() & 0xFFFF);
    n2 = (U64)(get_random_U32_number() & 0xFFFF);
    n3 = (U64)(get_random_U32_number() & 0xFFFF);
    n4 = (U64)(get_random_U32_number() & 0xFFFF);
    
    // return random number
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

int count_bits(U64 bitboard) {
    return __builtin_popcountll(bitboard);
}

int get_ls1b_index(U64 bitboard) {
    if (!bitboard) fatal_exit("cannot get lease significant bit of empty board");
    return __builtin_ffsll(bitboard) - 1;
}

std::chrono::time_point<std::chrono::system_clock> get_time_point() {
    return std::chrono::system_clock::now();
}

long long get_time_diff(std::chrono::time_point<std::chrono::system_clock> const & a, std::chrono::time_point<std::chrono::system_clock> const & b) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count();
}

long long get_time_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
