// system headers
#include <random>

// local headers
#include "util.hpp"

unsigned int x=123456789, y=362436069, z=521288629;

unsigned int get_random_number() {
    unsigned int t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return z;
}

U64 get_random_U64_number() {
    // define 4 random numbers
    U64 n1, n2, n3, n4;
    
    // init random numbers slicing 16 bits from MS1B side
    n1 = (U64)(get_random_number() & 0xFFFF);
    n2 = (U64)(get_random_number() & 0xFFFF);
    n3 = (U64)(get_random_number() & 0xFFFF);
    n4 = (U64)(get_random_number() & 0xFFFF);
    
    // return random number
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

