#ifndef magic_hpp
#define magic_hpp

// local headers
#include "pieces.hpp"
#include "types.hpp"
#include "bitboard.hpp"

U64 generate_magic_number();

U64 find_magic_number(BitBoardSquare square, int relevant_bits, BoardPiece::RorB bishop);

void init_magic_numbers();

#endif /* magic_hpp */
