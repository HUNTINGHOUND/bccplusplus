#ifndef zorbist_hpp
#define zorbist_hpp

// system headers
#include <array>

// local headers
#include "pieces.hpp"
#include "util.hpp"

using U64 = unsigned long long;

namespace Zorbist {
// random piece keys
extern std::array<std::array<U64, 64>, 12> pieces_keys;

// random enpassant keys
extern std::array<U64, 64> enpassant_keys;

// random castling keys
extern std::array<U64, 16> castle_keys;

extern U64 side_key;
}

inline void init_random_keys() {
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



#endif /* zorbist_hpp */
