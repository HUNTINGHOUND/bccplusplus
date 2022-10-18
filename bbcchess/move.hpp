#ifndef move_hpp
#define move_hpp

// system header
#include <iostream>

// local headers
#include "bitboard.hpp"
#include "pieces.hpp"

class Move {
public:
    
    static const int SOURCE_SQUARE_CONSTANT    = 0x3f;
    static const int TARGET_SQUARE_CONSTANT    = 0xfc0;
    static const int PIECE_CONSTANT            = 0xf000;
    static const int PROMOTED_PIECE_CONSTANT   = 0xf0000;
    static const int CAPTURE_FLAG_CONSTANT     = 0x100000;
    static const int DOUBLE_PUSH_FLAG_CONSTANT = 0x200000;
    static const int ENPASSANT_FLAG_CONSTANT   = 0x400000;
    static const int CASTLING_FLAG_CONSTANT    = 0x800000;
    
    int move;
     
    Move() : move(0) {};
    Move(int move) : move(move) {};
    
    int get_move_source() const {
        return move & SOURCE_SQUARE_CONSTANT;
    }
    
    int get_move_target() const {
        return (move & TARGET_SQUARE_CONSTANT) >> 6;
    }
    
    int get_move_piece() const {
        return (move & PIECE_CONSTANT) >> 12;
    }

    int get_move_promoted() const {
        return (move & PROMOTED_PIECE_CONSTANT) >> 16;
    }
    
    bool get_move_capture() const {
        return move & CAPTURE_FLAG_CONSTANT;
    }
    
    bool get_move_double() const {
        return move & DOUBLE_PUSH_FLAG_CONSTANT;
    }
    
    bool get_move_enpassant() const {
        return move & ENPASSANT_FLAG_CONSTANT;
    }
    
    bool get_move_castling() const {
        return move & CASTLING_FLAG_CONSTANT;
    }
    
    void print_move() const {
        std::cout << square_to_coordinates[get_move_source()] << square_to_coordinates[get_move_target()] << promoted_pieces[get_move_promoted()] << "\n";
    }
    
    void print_move_nonewline() const {
        std::cout << square_to_coordinates[get_move_source()] << square_to_coordinates[get_move_target()] << promoted_pieces[get_move_promoted()];
    }
    
    void encode(int source, int target, int piece, int promoted, bool capture, bool double_push, bool enpassant, bool castling) {
        move = source | (target << 6) | (piece << 12) | (promoted << 16) | (capture << 20) | (double_push << 21) | (enpassant << 22) | (castling << 23);
    }
    
    Move(int source, int target, int piece, int promoted, bool capture, bool double_push, bool enpassant, bool castling) {
        encode(source, target, piece, promoted, capture, double_push, enpassant, castling);
    }
    
};

class Moves {
public:
    Move moves[256];
    int count = 0;
    
    inline void add_move(Move move) {
        moves[count] = move;
        count++;
    }

    void print_move_list() const {
        std::cout << "\n    move    piece   capture   double    enpass    castling\n\n";
        
        for (int move_count = 0; move_count < count; move_count++) {
            Move move = moves[move_count];
            
            #ifdef WIN64
                std::cout << "    " << square_to_coordinates[move.get_move_source()]   <<
                                       square_to_coordinates[move.get_move_target()]   <<
                                       promoted_pieces[move.get_move_promoted()]       <<  "   " <<
                                       BoardPiece::ascii_pieces[move.get_move_piece()] << "       " <<
                                       (move.get_move_capture() ? 1 : 0)               << "         " <<
                                       (move.get_move_double() ? 1 : 0)                << "         " <<
                                       (move.get_move_enpassant() ? 1 : 0)             << "         " <<
                                       (move.get_move_castling() ? 1 : 0)              << "\n";
            #else
                std::cout << "    " << square_to_coordinates[move.get_move_source()]     <<
                                       square_to_coordinates[move.get_move_target()]     <<
                                       promoted_pieces[move.get_move_promoted()]         <<  "   " <<
                                       BoardPiece::unicode_pieces[move.get_move_piece()] << "       " <<
                                       (move.get_move_capture() ? 1 : 0)                 << "         " <<
                                       (move.get_move_double() ? 1 : 0)                  << "         " <<
                                       (move.get_move_enpassant() ? 1 : 0)               << "         " <<
                                       (move.get_move_castling() ? 1 : 0)                << "\n";
            #endif
            
        }
        std::cout << "\n\n    Total number of moves: " << count << "\n\n";
    }
};

enum MoveFlag { all_moves, only_captures };

#endif /* move_hpp */
