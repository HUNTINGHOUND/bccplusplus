#ifndef evaluation_hpp
#define evaluation_hpp

// system headers
#include <array>

// local headers
#include "bitboard.hpp"
#include "boardinfo.hpp"
#include "types.hpp"

namespace Evaluation {

// convert BBC piece code to Stockfish piece codes
const std::array<int, 12> nnue_pieces = { 6, 5, 4, 3, 2, 1, 12, 11, 10, 9, 8, 7 };

const std::array<int, 64> nnue_squares = {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

// material_score[piece]
const std::array<int, 12> material_score = {
    200, 820, 850, 1270, 2530, 12000, -200, -820, -850, -1270, -2530, -12000
};

// same as material score, but the values are all positive
const std::array<int, 12> absolute_material_score = {
    200, 820, 850, 1270, 2530, 12000, 200, 820, 850, 1270, 2530, 12000
};

// used by SEE, adjust the knight value so that bishop capture knight will not alway be a losing move
const std::array<int, 12> absolute_material_score_see = {
    200, 850, 850, 1270, 2530, 12000, 200, 850, 850, 1270, 2530, 12000
};

// Most Valuable Victim, Least Valuable Attack
const std::array<std::array<int, 6>, 6> mvv_lva = {{
    {{105, 205, 305, 405, 505, 605}},
    {{104, 204, 304, 404, 504, 604}},
    {{103, 203, 303, 403, 503, 603}},
    {{102, 202, 302, 402, 502, 602}},
    {{101, 201, 301, 401, 501, 601}},
    {{100, 200, 300, 400, 500, 600}}
}};

const int opening_phase_score = 12384;

const int endgame_phase_score = 3500;

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

// set file or rank mask
U64 set_file_rank_mask(int file_number, int rank_number);

void init_evaluation_masks();

int interpolate_score(int opening_score, int endgame_score, int game_phase_score);
}

#endif /* evaluation_hpp */
