#ifndef attacktables_hpp
#define attacktables_hpp

// local headers
#include "boardinfo.hpp"
#include "bitboard.hpp"
#include "pieces.hpp"

using U64 = unsigned long long;

namespace AttackTables {

/*
        not A file
  8  0 1 1 1 1 1 1 1
  7  0 1 1 1 1 1 1 1
  6  0 1 1 1 1 1 1 1
  5  0 1 1 1 1 1 1 1
  4  0 1 1 1 1 1 1 1
  3  0 1 1 1 1 1 1 1
  2  0 1 1 1 1 1 1 1
  1  0 1 1 1 1 1 1 1
     a b c d e f g h
        not H file
     
  8  1 1 1 1 1 1 1 0
  7  1 1 1 1 1 1 1 0
  6  1 1 1 1 1 1 1 0
  5  1 1 1 1 1 1 1 0
  4  1 1 1 1 1 1 1 0
  3  1 1 1 1 1 1 1 0
  2  1 1 1 1 1 1 1 0
  1  1 1 1 1 1 1 1 0
     a b c d e f g h
       not HG file
  8  1 1 1 1 1 1 0 0
  7  1 1 1 1 1 1 0 0
  6  1 1 1 1 1 1 0 0
  5  1 1 1 1 1 1 0 0
  4  1 1 1 1 1 1 0 0
  3  1 1 1 1 1 1 0 0
  2  1 1 1 1 1 1 0 0
  1  1 1 1 1 1 1 0 0
     a b c d e f g h
       not AB file
  8  0 0 1 1 1 1 1 1
  7  0 0 1 1 1 1 1 1
  6  0 0 1 1 1 1 1 1
  5  0 0 1 1 1 1 1 1
  4  0 0 1 1 1 1 1 1
  3  0 0 1 1 1 1 1 1
  2  0 0 1 1 1 1 1 1
  1  0 0 1 1 1 1 1 1
     a b c d e f g h
*/

constexpr const U64 not_a_file = 18374403900871474942ULL;

constexpr const U64 not_h_file = 9187201950435737471ULL;

constexpr const U64 not_hg_file = 4557430888798830399ULL;

constexpr const U64 not_ab_file = 18229723555195321596ULL;

void init_leapers_attacks();
void init_sliders_attacks(BoardPiece::RorB rorb);

U64 set_occupancy(int index, int bits_in_mask, BitBoard attack_mask);

namespace Pawn {
extern U64 pawn_attacks[2][64];

U64 mask_pawn_attacks(TurnColor side, BitBoardSquare square);
}

namespace Knight {
extern U64 knight_attacks[64];

U64 mask_knight_attacks(BitBoardSquare square);
}

namespace King {
extern U64 king_attacks[64];

U64 mask_king_attacks(BitBoardSquare square);
}

namespace Bishop {
extern U64 bishop_masks[64];
extern U64 bishop_attacks[64][512];
extern U64 bishop_magic_numbers[64];

// bishop relevant occupancy bit count for every square on board
const int bishop_relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

U64 mask_bishop_attacks(BitBoardSquare square);
U64 bishop_attacks_on_the_fly(BitBoardSquare square, U64 block);
inline U64 get_bishop_attacks(BitBoardSquare square, U64 occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square];
    
    return bishop_attacks[square][occupancy];
}
}

namespace Rook {
extern U64 rook_masks[64];
extern U64 rook_attacks[64][4096];
extern U64 rook_magic_numbers[64];

// rook relevant occupancy bit count for every square on board
const int rook_relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

U64 mask_rook_attacks(BitBoardSquare square);
U64 rook_attacks_on_the_fly(BitBoardSquare square, U64 block);
inline U64 get_rook_attacks(BitBoardSquare square, U64 occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square];
    
    return rook_attacks[square][occupancy];
}
}

namespace Queen {
inline U64 get_queen_attacks(BitBoardSquare square, U64 occupancy) {
    return Rook::get_rook_attacks(square, occupancy) | Bishop::get_bishop_attacks(square, occupancy);
}
}

}


#endif /* attacktables_hpp */
