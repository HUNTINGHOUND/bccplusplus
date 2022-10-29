#include "zorbist.hpp"

std::array<std::array<U64, 64>, 12> Zorbist::pieces_keys;
std::array<U64, 64> Zorbist::enpassant_keys;
std::array<U64, 16> Zorbist::castle_keys;
U64 Zorbist::side_key;

void Zorbist::init_random_keys() {
    // update pseudo random number state
    random_state = 1804289383;
    
    // loop over piece codes
    for (int piece = BoardPiece::P; piece <= BoardPiece::k; piece++) {
        for (int square = 0; square < 64; square++)
            Zorbist::pieces_keys[piece][square] = get_random_U64_number();
    }
    
    // loop over board squares
    for (int square = 0; square < 64; square++)
        Zorbist::enpassant_keys[square] = get_random_U64_number();
    
    // loop over castling kets
    for (int index = 0; index < 16; index++)
        Zorbist::castle_keys[index] = get_random_U64_number();
    
    Zorbist::side_key = get_random_U64_number();
}
