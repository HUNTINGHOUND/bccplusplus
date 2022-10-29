// system headers
#include <iostream>

// local headers
#include "move.hpp"
#include "bitboard.hpp"
#include "pieces.hpp"

Move::operator int() const {
    return move;
}

int Move::get_move_source() const {
    return move & SOURCE_SQUARE_CONSTANT;
}

int Move::get_move_target() const {
    return (move & TARGET_SQUARE_CONSTANT) >> 6;
}

int Move::get_move_piece() const {
    return (move & PIECE_CONSTANT) >> 12;
}

int Move::get_move_promoted() const {
    return (move & PROMOTED_PIECE_CONSTANT) >> 16;
}

bool Move::get_move_capture() const {
    return move & CAPTURE_FLAG_CONSTANT;
}

bool Move::get_move_double() const {
    return move & DOUBLE_PUSH_FLAG_CONSTANT;
}

bool Move::get_move_enpassant() const {
    return move & ENPASSANT_FLAG_CONSTANT;
}

bool Move::get_move_castling() const {
    return move & CASTLING_FLAG_CONSTANT;
}

void Move::print_move() const {
    std::cout << square_to_coordinates[get_move_source()] << square_to_coordinates[get_move_target()] << promoted_pieces[get_move_promoted()] << "\n";
}

void Move::print_move_nonewline() const {
    std::cout << square_to_coordinates[get_move_source()] << square_to_coordinates[get_move_target()] << promoted_pieces[get_move_promoted()];
}

void Move::encode(int source, int target, int piece, int promoted, bool capture, bool double_push, bool enpassant, bool castling) {
    move = source | (target << 6) | (piece << 12) | (promoted << 16) | (capture << 20) | (double_push << 21) | (enpassant << 22) | (castling << 23);
}

Move::Move(int source, int target, int piece, int promoted, bool capture, bool double_push, bool enpassant, bool castling) {
    encode(source, target, piece, promoted, capture, double_push, enpassant, castling);
}

void Moves::add_move(Move move) {
    moves[count] = move;
    count++;
}

void Moves::print_move_list() const {
    std::cout << "\n    move    piece   capture   double    enpass    castling\n\n";
    
    for (int move_count = 0; move_count < count; move_count++) {
        Move move = moves[move_count];
        
        #ifdef WIN64
            std::cout << "    " << square_to_coordinates[move.get_move_source()]   <<
                                   square_to_coordinates[move.get_move_target()]   <<
                                   promoted_pieces[move.get_move_promoted()]       <<  "   " <<
                                   BoardPiece::ascii_pieces[move.get_move_piece()] << "       " <<
                                   move.get_move_capture()                         << "         " <<
                                   move.get_move_double()                          << "         " <<
                                   move.get_move_enpassant()                       << "         " <<
                                   move.get_move_castling()                        << "\n";
        #else
            std::cout << "    " << square_to_coordinates[move.get_move_source()]        <<
                                   square_to_coordinates[move.get_move_target()]        <<
                                   promoted_pieces[move.get_move_promoted()]            <<  "   " <<
                                   BoardPiece::unicode_pieces[move.get_move_piece()]    << "       " <<
                                   move.get_move_capture()                              << "         " <<
                                   move.get_move_double()                               << "         " <<
                                   move.get_move_enpassant()                            << "         " <<
                                   move.get_move_castling()                             << "\n";
        #endif
        
    }
    std::cout << "\n\n    Total number of moves: " << count << "\n\n";
}
