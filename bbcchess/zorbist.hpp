#ifndef zorbist_hpp
#define zorbist_hpp

// system headers
#include <array>

// local headers
#include "pieces.hpp"
#include "util.hpp"
#include "types.hpp"

namespace Zorbist {
// random piece keys
extern std::array<std::array<U64, 64>, 12> pieces_keys;

// random enpassant keys
extern std::array<U64, 64> enpassant_keys;

// random castling keys
extern std::array<U64, 16> castle_keys;

extern U64 side_key;

void init_random_keys();
}




#endif /* zorbist_hpp */
