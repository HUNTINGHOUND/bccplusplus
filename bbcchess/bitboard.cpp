// system headers
#include <iostream>

// local headers
#include "bitboard.hpp"

void BitBoard::set_bit(U64& bitboard, int square) {
    bitboard |= (1ULL << square);
}

U64 BitBoard::get_bit(U64 const & bitboard, int square) {
    return bitboard & (1ULL << square);
}

void BitBoard::pop_bit(U64& bitboard, int square) {
    bitboard &= ~(1ULL << (square));
}

void BitBoard::print_bitboard(U64 const & bitboard) {
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
