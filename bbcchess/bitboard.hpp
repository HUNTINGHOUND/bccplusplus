#ifndef bitboard_hpp
#define bitboard_hpp

// system headers
#include <iostream>

// define bitboard data type
using U64 = unsigned long long;

class BitBoard {
public:
    U64 bitboard;
    
    BitBoard() noexcept : bitboard(0) {};
    BitBoard(U64 board) noexcept : bitboard(board) {};
    
    void set_bit(int square) noexcept {
        bitboard |= (1ULL << square);
    }
    U64 get_bit(int square) const noexcept {
        return bitboard & (1ULL << square);
    }
    void pop_bit(int square) noexcept {
        bitboard &= ~(1ULL << (square));
    }
    
    void print_bitboard() const {
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
                std::cout << " " << (get_bit(square) ? 1 : 0);
            }
            
            // print new line every rank
            std::cout << "\n";
        }
        
        // print board files
        std::cout << "\n     a b c d e f g h\n\n";
        
        // print bitboard as unsigned decimal number
        std::cout << "     Bitboard: " << bitboard << "\n\n";
    }
    
    U64 operator|(BitBoard const & o) const noexcept {
        return bitboard | o.bitboard;
    }
    BitBoard& operator|=(BitBoard const & o) noexcept {
        bitboard |= o.bitboard;
        return *this;
    }
    
    U64 operator>>(int shift) const noexcept {
        return bitboard >> shift;
    }
    U64 operator<<(int shift) const noexcept {
        return bitboard << shift;
    }
    
    U64 operator~() const noexcept {
        return ~bitboard;
    }
    
    U64 operator|(U64 o) const noexcept {
        return bitboard | o;
    }
    
    U64 operator&(U64 o) const noexcept {
        return bitboard & o;
    }
    
    operator U64() const noexcept {
        return bitboard;
    }
};

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

const std::string square_to_coordinates[64] = {
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
