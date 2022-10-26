#include "zorbist.hpp"

std::array<std::array<U64, 64>, 12> Zorbist::pieces_keys;
std::array<U64, 64> Zorbist::enpassant_keys;
std::array<U64, 16> Zorbist::castle_keys;
U64 Zorbist::side_key;
