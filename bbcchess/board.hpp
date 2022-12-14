#ifndef bitmanipulation_hpp
#define bitmanipulation_hpp

// system header
#include <array>

// local header
#include "types.hpp"
#include "boardinfo.hpp"
#include "bitboard.hpp"
#include "move.hpp"
#include "pieces.hpp"
#include "evaluation.hpp"

// #define DEBUG_ZORBIST

class BoardRepresentation {
public:
    
    // piece bitboard
    std::array<U64, 12> bitboards;
    
    // occupancy bitboards
    std::array<U64, 3> occupancies;
    
    // side to move
    TurnColor side = white;
    
    // enpanssant square
    BitBoardSquare enpassant = no_sq;
    
    // almost unique identifier
    U64 hash_key;
    
    // castling rights
    int castle;
    
    int fifty = 0;
    
    std::array<int, 2> piece_material = {2 * Evaluation::absolute_material_score[BoardPiece::N] + 2 * Evaluation::absolute_material_score[BoardPiece::B] + 2 * Evaluation::absolute_material_score[BoardPiece::R] + 2 * Evaluation::absolute_material_score[BoardPiece::Q],
        2 * Evaluation::absolute_material_score[BoardPiece::n] + 2 * Evaluation::absolute_material_score[BoardPiece::b] + 2 * Evaluation::absolute_material_score[BoardPiece::r] + 2 * Evaluation::absolute_material_score[BoardPiece::q]};
    
    
    GamePhase phase = opening;
    
    void print_board() const;
    
    void print_attacked_square(TurnColor side) const;
    
    U64 generate_hash_key() const;
    
    void parse_fen(std::string const & fen, size_t fen_idx = 0);
    
    bool is_square_attacked(BitBoardSquare square, TurnColor side) const;
    
    U64 attacks_to(U64 occupied, BitBoardSquare sq) const;
    
    U64 get_least_valuable_piece(U64 attadef, int bySide, int &piece) const;
    
    int get_piece_on_square(BitBoardSquare sq) const;
    
    int see(BitBoardSquare to_square, BoardPiece::Pieces target, BitBoardSquare fr_square, int a_piece) const;
    
    int make_move(Move const & move, MoveFlag move_flag);
    
    // Returns copy of the board with the move made, pointer indicates whether or not the move was legal
    BoardRepresentation copy_and_move(Move const & move, MoveFlag move_flag, int* move_made) const;
    
    Moves generate_moves(bool qs) const;
        
    int evaluate() const;
    
private:
    void generate_pawn_moves(Moves& move_list, bool qs) const;
    
    void generate_castling_moves(Moves& move_list) const;
    
    void generate_knight_moves(Moves& move_list, bool qs) const;

    void generate_bishop_moves(Moves& move_list, bool qs) const;
    
    void generate_rook_moves(Moves& move_list, bool qs) const;
    
    void generate_queen_moves(Moves& move_list, bool qs) const;
    
    void generate_king_moves(Moves& move_list, bool qs) const;
};





#endif /* bitmanipulation_hpp */
