#ifndef bitboard_hpp
#define bitboard_hpp

// system headers
#include <iostream>
#include <array>

// define bitboard data type
using U64 = unsigned long long;

namespace BitBoard {
inline void set_bit(U64& bitboard, int square) {
    bitboard |= (1ULL << square);
}

inline U64 get_bit(U64 bitboard, int square) {
    return bitboard & (1ULL << square);
}

inline void pop_bit(U64& bitboard, int square) {
    bitboard &= ~(1ULL << (square));
}

inline void print_bitboard(U64 bitboard) {
    std::cout << "\n";

    // loop over board ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over board files
        for (int file = 0; file < 8; file++)
        {
            // convert file & rank into square index
            int square = rank * 8 + file;
            
            // print ranks
            if (!file)
                std::cout << "  " << 8 - rank << " ";
            
            // print bit state (either 1 or 0)
            std::cout << " " << (get_bit(bitboard, square) ? 1 : 0);
        }
        
        // print new line every rank
        std::cout << "\n";
    }
    
    // print board files
    std::cout << "\n     a b c d e f g h\n\n";
    
    // print bitboard as unsigned decimal number
    std::cout << "     Bitboard: " << bitboard << "\n\n";
}
}

// board squares
enum BitBoardSquare {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// mirror positional score tables for opposite side
const std::array<BitBoardSquare, 128> mirror_score =
{
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

const std::array<std::string, 64> square_to_coordinates = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};


#endif /* bitboard_hpp */
