#ifndef evaluation_hpp
#define evaluation_hpp

// system headers
#include <array>

// local headers
#include "bitboard.hpp"
#include "boardinfo.hpp"

using U64 = unsigned long long;

namespace Evaluation {

const std::array<int, 12> material_score = {
    100,        // white pawn
    300,        // white knight
    350,        // white bishop
    500,        // white rook
    1000,       // white queen
    10000,      // white king
    -100,       // black pawn
    -300,       // black knight
    -350,       // black bishop
    -500,       // black rook
    -1000,      // black qeen
    -10000      // black king
};

// pawn positional score
const std::array<int, 64> pawn_score = {
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
    5,   5,  10,  20,  20,   5,   5,   5,
    0,   0,   0,   5,   5,   0,   0,   0,
    0,   0,   0, -10, -10,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
const std::array<int, 64> knight_score =
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
const std::array<int, 64> bishop_score =
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,  20,   0,  10,  10,   0,  20,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
     0,   0, -10,   0,   0, -10,   0,   0

};


// rook positional score
const std::array<int, 64> rook_score =
{
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,  10,  20,  20,  10,   0,   0,
    0,   0,   0,  20,  20,   0,   0,   0
    
};

// king positional score
const std::array<int, 64> king_score =
{
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   5,   5,   5,   5,   0,   0,
    0,   5,   5,  10,  10,   5,   5,   0,
    0,   5,  10,  20,  20,  10,   5,   0,
    0,   5,  10,  20,  20,  10,   5,   0,
    0,   0,   5,  10,  10,   5,   0,   0,
    0,   5,   5,  -5,  -5,   0,   5,   0,
    0,   0,   5,   0, -15,   0,  10,   0
};

// Most Valuable Victim, Least Valuable Attack
const std::array<std::array<int, 6>, 6> mvv_lva = {
    105, 205, 305, 405, 505, 605,
    104, 204, 304, 404, 504, 604,
    103, 203, 303, 403, 503, 603,
    102, 202, 302, 402, 502, 602,
    101, 201, 301, 401, 501, 601,
    100, 200, 300, 400, 500, 600
};

// extract rank from a square [square]
const std::array<int, 64> get_rank = {
    7, 7, 7, 7, 7, 7, 7, 7,
    6, 6, 6, 6, 6, 6, 6, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0
};

// double pawns penalty
const int double_pawn_penalty = -10;

// isolated pawn penalty
const int isolated_pawn_penalty = -10;

// semi open file score
const int semi_open_file_score = 10;

// open file score
const int open_file_score = 15;

// king's shield bonus
const int king_shield_bonus = 5;

// passed pawn bonus
const std::array<int, 8> passed_pawn_bonus = { 0, 10, 30, 50, 75, 100, 150, 200 };

/*
          Rank mask            File mask           Isolated mask        Passed pawn mask
        for square a6        for square f2         for square g2          for square c4

    8  0 0 0 0 0 0 0 0    8  0 0 0 0 0 1 0 0    8  0 0 0 0 0 1 0 1     8  0 1 1 1 0 0 0 0
    7  0 0 0 0 0 0 0 0    7  0 0 0 0 0 1 0 0    7  0 0 0 0 0 1 0 1     7  0 1 1 1 0 0 0 0
    6  1 1 1 1 1 1 1 1    6  0 0 0 0 0 1 0 0    6  0 0 0 0 0 1 0 1     6  0 1 1 1 0 0 0 0
    5  0 0 0 0 0 0 0 0    5  0 0 0 0 0 1 0 0    5  0 0 0 0 0 1 0 1     5  0 1 1 1 0 0 0 0
    4  0 0 0 0 0 0 0 0    4  0 0 0 0 0 1 0 0    4  0 0 0 0 0 1 0 1     4  0 0 0 0 0 0 0 0
    3  0 0 0 0 0 0 0 0    3  0 0 0 0 0 1 0 0    3  0 0 0 0 0 1 0 1     3  0 0 0 0 0 0 0 0
    2  0 0 0 0 0 0 0 0    2  0 0 0 0 0 1 0 0    2  0 0 0 0 0 1 0 1     2  0 0 0 0 0 0 0 0
    1  0 0 0 0 0 0 0 0    1  0 0 0 0 0 1 0 0    1  0 0 0 0 0 1 0 1     1  0 0 0 0 0 0 0 0

       a b c d e f g h       a b c d e f g h       a b c d e f g h        a b c d e f g h
*/

// file masks [square]
extern std::array<U64, 64> file_masks;

// rank masks [square]
extern std::array<U64, 64> rank_masks;

// isolated pawn masks [square]
extern std::array<U64, 64> isolated_masks;

// passed pawn masks [square]
extern std::array<std::array<U64, 64>, 2> passed_masks;

// set file or rank mask
inline U64 set_file_rank_mask(int file_number, int rank_number)
{
    // file or rank mask
    BitBoard mask = 0ULL;
    
    // loop over ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            if (file_number != -1)
            {
                // on file match
                if (file == file_number)
                    // set bit on mask
                    mask.set_bit(square);
            }
            
            else if (rank_number != -1)
            {
                // on rank match
                if (rank == rank_number)
                    // set bit on mask
                    mask.set_bit(square);
            }
        }
    }
    
    // return mask
    return mask;
}

inline void init_evaluation_masks() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            
            file_masks[square] |= set_file_rank_mask(file, -1);
            rank_masks[square] |= set_file_rank_mask(-1, rank);
            
            isolated_masks[square] |= set_file_rank_mask(file - 1, -1) | set_file_rank_mask(file + 1, -1);
        }
    }
    
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            
            passed_masks[white][square] |= set_file_rank_mask(file - 1, -1) | set_file_rank_mask(file, -1) | set_file_rank_mask(file + 1, -1);
            passed_masks[black][square] |= set_file_rank_mask(file - 1, -1) | set_file_rank_mask(file, -1) | set_file_rank_mask(file + 1, -1);
            
            for (int i = 0; i < (8 - rank); i++) passed_masks[white][square] &= ~rank_masks[(7 - i) * 8 + file];
            for (int i = 0; i < rank + 1; i++) passed_masks[black][square] &= ~rank_masks[i * 8 + file];
        }
    }
}
}

#endif /* evaluation_hpp */
