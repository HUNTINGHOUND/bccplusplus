#ifndef bitmanipulation_hpp
#define bitmanipulation_hpp

// system header
#include <string>


// local heaer
#include "move.hpp"
#include "bitboard.hpp"
#include "pieces.hpp"
#include "attacktables.hpp"
#include "util.hpp"
#include "boardinfo.hpp"
#include "evaluation.hpp"

using U64 = unsigned long long;

class BoardRepresentation {
    
public:
    
    // piece bitboard
    BitBoard bitboards[12];
    
    // occupancy bitboards
    BitBoard occupancies[3];
    
    // side to move
    TurnColor side = white;
    
    // enpanssant square
    BitBoardSquare enpassant = no_sq;
    
    // castling rights
    int castle;
    
    void print_board() const {
        // print offset
        std::cout << "\n";
        
        // loop over over board ranks
        for (int rank = 0; rank < 8; rank++) {
            // loop over board files
            for (int file = 0; file < 8; file++) {
                // init square
                 BitBoardSquare square = BitBoardSquare(rank * 8 + file);
                
                // print ranks
                if (!file)
                    std::cout << "  " << 8 - rank << " ";
                
                // define piece variable
                int piece = -1;
                
                // loop over all piece bitboards
                for (int bb_piece = BoardPiece::P; bb_piece <= BoardPiece::k; bb_piece++) {
                    if (bitboards[bb_piece].get_bit(square)) {
                        piece = bb_piece;
                        break;
                    }
                }
                
                // print different piece set depending on OS
                #ifdef WIN64
                    std::cout << " " << (piece == -1 ? '.' : BoardPiece::ascii_pieces[piece]);
                #else
                    std::cout << " " << (piece == -1 ? "." : BoardPiece::unicode_pieces[piece]);
                #endif
            }
            
            std::cout << "\n";
        }
        
        std::cout << "\n     a b c d e f g h\n\n";
        std::cout << "     Side:     " << (side ? "black" : "white") << "\n";
        std::cout << "     Enpassant:   " << (enpassant != no_sq ?   square_to_coordinates[enpassant] : "no") << "\n";
        std::cout << "     Castling:  " << (castle & wk ? 'K' : '-') << (BoardRepresentation::castle & wq ? 'Q' : '-') << (castle & bk ? 'k' : '-') << (castle & bq ? 'q' : '-') << "\n\n";
    }
    
    void print_attacked_square(TurnColor side) const {
        std::cout << "\n";
        
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                
                if (!file)
                    std::cout << "  " << 8 - rank << " ";
                
                std::cout << " " << is_square_attacked(BitBoardSquare(square), side);
            }
            
            std::cout << "\n";
        }
        
        std::cout << "\n     a b c d e f g h\n\n";
    }
    
    void parse_fen(std::string const & fen, size_t fen_idx = 0) {
        std::fill(bitboards, bitboards + 12, 0ULL);
        std::fill(occupancies, occupancies + 3, 0ULL);
        
        side = white;
        enpassant = no_sq;
        castle = 0;
        
        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                
                if (isalpha(fen[fen_idx])) {
                    BoardPiece::Pieces piece = BoardPiece::char_pieces[fen[fen_idx]];
                    bitboards[piece].set_bit(square);
                } else if (isnumber(fen[fen_idx])) {
                    int offset = fen[fen_idx] - '0';
                    file += offset - 1;
                }
                
                fen_idx++;
            }
            fen_idx++;
        }
        
        
        if (fen[fen_idx] == 'w') side = white;
        else side = black;
        
        fen_idx += 2;
        
        while (fen[fen_idx] != ' ') {
            switch (fen[fen_idx]) {
                case 'K':
                    castle |= wk;
                    break;
                case 'Q':
                    castle |= wq;
                    break;
                case 'k':
                    castle |= bk;
                    break;
                case 'q':
                    castle |= bq;
                    break;
                case '-':
                    break;
                    
                default:
                    throw std::invalid_argument("Invalid fen");
            }
            
            fen_idx++;
        }
        
        fen_idx++;
        
        if (fen[fen_idx] != '-') {
            int file = fen[fen_idx] - 'a';
            int rank = 8 - (fen[fen_idx + 1] - '0');
            
            enpassant = BitBoardSquare(rank * 8 + file);
        } else enpassant = no_sq;
        
        for (int piece = BoardPiece::P; piece <= BoardPiece::K; piece++)
            occupancies[white] |= bitboards[piece];
        
        for (int piece = BoardPiece::p; piece <= BoardPiece::k; piece++)
            occupancies[black] |= bitboards[piece];
        
        occupancies[both] |= occupancies[white];
        occupancies[both] |= occupancies[black];
    }
    
    bool is_square_attacked(BitBoardSquare square, TurnColor side) const {
        if ((side == white) && (bitboards[BoardPiece::P] & AttackTables::Pawn::pawn_attacks[black][square])) return true;
        else if ((side == black) && (bitboards[BoardPiece::p] & AttackTables::Pawn::pawn_attacks[white][square])) return true;
        else if ((side == white ? bitboards[BoardPiece::N] : bitboards[BoardPiece::n]) & AttackTables::Knight::knight_attacks[square]) return true;
        else if ((side == white ? bitboards[BoardPiece::B] : bitboards[BoardPiece::b]) & AttackTables::Bishop::get_bishop_attacks(square, occupancies[both])) return true;
        else if ((side == white ? bitboards[BoardPiece::R] : bitboards[BoardPiece::r]) & AttackTables::Rook::get_rook_attacks(square, occupancies[both])) return true;
        else if ((side == white ? bitboards[BoardPiece::Q] : bitboards[BoardPiece::q]) & AttackTables::Queen::get_queen_attacks(square, occupancies[both])) return true;
        else if ((side == white ? bitboards[BoardPiece::K] : bitboards[BoardPiece::k]) & AttackTables::King::king_attacks[square]) return true;

        return false;
    }
    
    int make_move(Move const & move, MoveFlag move_flag) {
        if (move_flag == all_moves) {
            BitBoardSquare source_square = BitBoardSquare(move.get_move_source());
            BitBoardSquare target_square = BitBoardSquare(move.get_move_target());
            BoardPiece::Pieces piece = BoardPiece::Pieces(move.get_move_piece());
            BoardPiece::Pieces promoted = BoardPiece::Pieces(move.get_move_promoted());
            bool capture = move.get_move_capture();
            bool double_push = move.get_move_double();
            bool enpass = move.get_move_enpassant();
            bool castling = move.get_move_castling();
            
            // move piece
            bitboards[piece].pop_bit(source_square);
            bitboards[piece].set_bit(target_square);
            
            occupancies[side].pop_bit(source_square);
            occupancies[side].set_bit(target_square);
            
            if (capture) {
                BoardPiece::Pieces start_piece, end_piece;
                
                if (side == white) {
                    start_piece = BoardPiece::p;
                    end_piece = BoardPiece::k;
                } else {
                    start_piece = BoardPiece::P;
                    end_piece = BoardPiece::K;
                }
                
                for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
                    if (bitboards[bb_piece].get_bit(target_square)) {
                        bitboards[bb_piece].pop_bit(target_square);
                        break;
                    }
                }
                
                occupancies[side ^ 1].pop_bit(target_square);
            }
            
            if (promoted) {
                bitboards[side == white ? BoardPiece::P : BoardPiece::p].pop_bit(target_square);
                bitboards[promoted].set_bit(target_square);
            }
            
            if (enpass) {
                side == white ? bitboards[BoardPiece::p].pop_bit(target_square + 8) :
                                bitboards[BoardPiece::P].pop_bit(target_square - 8);
                
                side == white ? occupancies[black].pop_bit(target_square + 8) :
                                occupancies[white].pop_bit(target_square - 8);
            }
            
            enpassant = no_sq;
            
            if (double_push) {
                side == white ? enpassant = BitBoardSquare(target_square + 8) :
                                enpassant = BitBoardSquare(target_square - 8);
            }
            
            if (castling) {
                switch (target_square) {
                        // white king side
                    case g1:
                        bitboards[BoardPiece::R].pop_bit(h1);
                        bitboards[BoardPiece::R].set_bit(f1);
                        
                        occupancies[white].pop_bit(h1);
                        occupancies[white].set_bit(f1);
                        break;
                        
                        // white queen side
                    case c1:
                        bitboards[BoardPiece::R].pop_bit(a1);
                        bitboards[BoardPiece::R].set_bit(d1);
                        
                        occupancies[white].pop_bit(a1);
                        occupancies[white].set_bit(d1);
                        break;
                        
                        // black king side
                    case g8:
                        bitboards[BoardPiece::r].pop_bit(h8);
                        bitboards[BoardPiece::r].set_bit(f8);
                        
                        occupancies[black].pop_bit(h8);
                        occupancies[black].set_bit(f8);
                        break;
                        
                        // black queen side
                    case c8:
                        bitboards[BoardPiece::r].pop_bit(a8);
                        bitboards[BoardPiece::r].set_bit(d8);
                        
                        occupancies[black].pop_bit(a8);
                        occupancies[black].set_bit(d8);
                        break;
                        
                    default:
                        throw std::invalid_argument("invalid castling target square");
                }
            }
            
            // update castling right
            castle &= castling_rights[source_square];
            castle &= castling_rights[target_square];
            
            occupancies[both] = occupancies[white] | occupancies[black];
            
            side = TurnColor(side ^ 1);
            
            if (is_square_attacked(side == white ? BitBoardSquare(get_ls1b_index(bitboards[BoardPiece::k])) :
                                                   BitBoardSquare(get_ls1b_index(bitboards[BoardPiece::K])), side))
                return 0;
            
            return 1;
        } else { // only captures
            if (move.get_move_capture())
                return make_move(move, all_moves);
            
            return 0;
        }
    }
    
    // Returns copy of the board with the move made, pointer indicates whether or not the move was legal
    BoardRepresentation copy_and_move(Move const & move, MoveFlag move_flag, int* move_made) const {
        BoardRepresentation copy = *this;
        int success = copy.make_move(move, move_flag);
        if (move_made) *move_made = success;
        return copy;
    }
    
private:
    void generate_pawn_moves(Moves& move_list) const {
        int source_square, target_square;
        
        BitBoard bitboard = side == white ? bitboards[BoardPiece::P] : bitboards[BoardPiece::p];
        BitBoard attacks;
        
        while (bitboard) {
            source_square = get_ls1b_index(bitboard);
            
            // get the square above source square
            target_square = side == white ? source_square - 8 : source_square + 8;
            
            // if target square is not off the board
            if ((side == white && !(target_square < a8) && !occupancies[both].get_bit(target_square)) ||
                (side == black && !(target_square > h1) && !occupancies[both].get_bit(target_square))) {
                // promotion?
                if ((side == white && source_square >= a7 && source_square <= h7) ||
                    (side == black && source_square >= a2 && source_square <= h2)) {
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            side == white ? BoardPiece::Q : BoardPiece::q,
                                            0, 0, 0, 0));
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            side == white ? BoardPiece::R : BoardPiece::r,
                                            0, 0, 0, 0));
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            side == white ? BoardPiece::B : BoardPiece::b,
                                            0, 0, 0, 0));
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            side == white ? BoardPiece::N : BoardPiece::n,
                                            0, 0, 0, 0));
                } else {
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            0, 0, 0, 0, 0));
                    
                    // double pawn push
                    if ((side == white && (source_square >= a2 && source_square <= h2) && !occupancies[both].get_bit(target_square - 8)) ||
                        (side == black && (source_square >= a7 && source_square <= h7) && !occupancies[both].get_bit(target_square + 8)))
                        move_list.add_move(Move(source_square,
                                                side == white ? target_square - 8 : target_square + 8,
                                                side == white ? BoardPiece::P : BoardPiece::p,
                                                0, 0, 1, 0, 0));
                }
            }
            
            attacks = (side == white ? occupancies[black] : occupancies[white]) & AttackTables::Pawn::pawn_attacks[side][source_square];
            
            while (attacks) {
                target_square = get_ls1b_index(attacks);
                
                // promotion?
                if ((side == white && source_square >= a7 && source_square <= h7) ||
                    (side == black && source_square >= a2 && source_square <= h2)) {
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            side == white ? BoardPiece::Q : BoardPiece::q,
                                            1, 0, 0, 0));
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            side == white ? BoardPiece::R : BoardPiece::r,
                                            1, 0, 0, 0));
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            side == white ? BoardPiece::B : BoardPiece::b,
                                            1, 0, 0, 0));
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            side == white ? BoardPiece::N : BoardPiece::n,
                                            1, 0, 0, 0));
                } else
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            0, 1, 0, 0, 0));
                
                attacks.pop_bit(target_square);
            }
            
            if (enpassant != no_sq) {
                U64 enpassant_attacks = AttackTables::Pawn::pawn_attacks[side][source_square] & (1ULL << enpassant);
                
                if (enpassant_attacks) {
                    int target_enpassant = get_ls1b_index(enpassant_attacks);
                    move_list.add_move(Move(source_square, target_enpassant,
                                            side == white ? BoardPiece::P : BoardPiece::p,
                                            0, 1, 0, 1, 0));
                }
            }
            
            bitboard.pop_bit(source_square);
        }
    }
    
    void generate_castling_moves(Moves& move_list) const{
        if ((side == white && (castle & wk)) ||
            (side == black && (castle & bk))) {
            if ((side == white && !occupancies[both].get_bit(f1) && !occupancies[both].get_bit(g1)) ||
                (side == black && !occupancies[both].get_bit(f8) && !occupancies[both].get_bit(g8))) {
                
                // if g1 or g8 is attacked, this move will be pruned
                if ((side == white && !is_square_attacked(e1, black) && !is_square_attacked(f1, black)) ||
                    (side == black && !is_square_attacked(e8, white) && !is_square_attacked(f8, white)))
                    side == white ? move_list.add_move(Move(e1, g1,
                                                            side == white ? BoardPiece::K : BoardPiece::k,
                                                            0, 0, 0, 0, 1)) :
                                    move_list.add_move(Move(e8, g8,
                                                            side == white ? BoardPiece::K : BoardPiece::k,
                                                            0, 0, 0, 0, 1));
            }
        }
        
        if ((side == white && (castle & wq)) ||
            (side == black && (castle & bq))) {
            
            // if c1 or c8 is attacked, this move will be pruned
            if ((side == white && !occupancies[both].get_bit(d1) && !occupancies[both].get_bit(c1) && !occupancies[both].get_bit(b1)) ||
                (side == black && !occupancies[both].get_bit(d8) && !occupancies[both].get_bit(c8) && !occupancies[both].get_bit(b8))) {
                if ((side == white && !is_square_attacked(e1, black) && !is_square_attacked(d1, black)) ||
                    (side == black && !is_square_attacked(e8, white) && !is_square_attacked(d8, white)))
                    side == white ? move_list.add_move(Move(e1, c1,
                                                            side == white ? BoardPiece::K : BoardPiece::k,
                                                            0, 0, 0, 0, 1)) :
                                    move_list.add_move(Move(e8, c8,
                                                            side == white ? BoardPiece::K : BoardPiece::k,
                                                            0, 0, 0, 0, 1));
            }
        }
    }
    
    void generate_knight_moves(Moves& move_list) const{
        int source_square, target_square;
        BitBoard bitboard = side == white ? bitboards[BoardPiece::N] : bitboards[BoardPiece::n];
        BitBoard attacks;
        while(bitboard) {
            source_square = get_ls1b_index(bitboard);
            
            attacks = AttackTables::Knight::knight_attacks[source_square] & ~(side == white ? occupancies[white] : occupancies[black]);
            
            while (attacks) {
                target_square = get_ls1b_index(attacks);
                
                // quite move
                if (!(side == white ? occupancies[black] : occupancies[white]).get_bit(target_square))
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::N : BoardPiece::n,
                                            0, 0, 0, 0, 0));
                else // capture move
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::N : BoardPiece::n,
                                            0, 1, 0, 0, 0));
                
                attacks.pop_bit(target_square);
            }
            
            bitboard.pop_bit(source_square);
        }
    }

    void generate_bishop_moves(Moves& move_list) const{
        int source_square, target_square;
        BitBoard bitboard = side == white ? bitboards[BoardPiece::B] : bitboards[BoardPiece::b];
        BitBoard attacks;
        while(bitboard) {
            source_square = get_ls1b_index(bitboard);
            
            attacks = AttackTables::Bishop::get_bishop_attacks(BitBoardSquare(source_square), occupancies[both]) & ~(side == white ? occupancies[white] : occupancies[black]);
            
            while (attacks) {
                target_square = get_ls1b_index(attacks);
                
                // quite move
                if (!(side == white ? occupancies[black] : occupancies[white]).get_bit(target_square))
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::B : BoardPiece::b,
                                            0, 0, 0, 0, 0));
                else // capture move
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::B : BoardPiece::b,
                                            0, 1, 0, 0, 0));
                
                attacks.pop_bit(target_square);
            }
            
            bitboard.pop_bit(source_square);
        }
    }
    
    void generate_rook_moves(Moves& move_list) const{
        int source_square, target_square;
        BitBoard bitboard = side == white ? bitboards[BoardPiece::R] : bitboards[BoardPiece::r];
        BitBoard attacks;
        while(bitboard) {
            source_square = get_ls1b_index(bitboard);
            
            attacks = AttackTables::Rook::get_rook_attacks(BitBoardSquare(source_square), occupancies[both]) & ~(side == white ? occupancies[white] : occupancies[black]);
            
            while (attacks) {
                target_square = get_ls1b_index(attacks);
                
                // quite move
                if (!(side == white ? occupancies[black] : occupancies[white]).get_bit(target_square))
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::R : BoardPiece::r,
                                            0, 0, 0, 0, 0));
                else // capture move
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::R : BoardPiece::r,
                                            0, 1, 0, 0, 0));
                
                attacks.pop_bit(target_square);
            }
            
            bitboard.pop_bit(source_square);
        }
    }
    
    void generate_queen_moves(Moves& move_list) const{
        int source_square, target_square;
        BitBoard bitboard = side == white ? bitboards[BoardPiece::Q] : bitboards[BoardPiece::q];
        BitBoard attacks;
        while(bitboard) {
            source_square = get_ls1b_index(bitboard);
            
            attacks = AttackTables::Queen::get_queen_attacks(BitBoardSquare(source_square), occupancies[both]) & ~(side == white ? occupancies[white] : occupancies[black]);
            
            while (attacks) {
                target_square = get_ls1b_index(attacks);
                
                // quite move
                if (!(side == white ? occupancies[black] : occupancies[white]).get_bit(target_square))
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::Q : BoardPiece::q,
                                            0, 0, 0, 0, 0));
                else // capture move
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::Q : BoardPiece::q,
                                            0, 1, 0, 0, 0));
                
                attacks.pop_bit(target_square);
            }
            
            bitboard.pop_bit(source_square);
        }
    }
    
    void generate_king_moves(Moves& move_list) const{
        int source_square, target_square;
        BitBoard bitboard = side == white ? bitboards[BoardPiece::K] : bitboards[BoardPiece::k];
        BitBoard attacks;
        while(bitboard) {
            source_square = get_ls1b_index(bitboard);
            
            attacks = AttackTables::King::king_attacks[source_square] & ~(side == white ? occupancies[white] : occupancies[black]);
            
            while (attacks) {
                target_square = get_ls1b_index(attacks);
                
                // quite move
                if (!(side == white ? occupancies[black] : occupancies[white]).get_bit(target_square))
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::K : BoardPiece::k,
                                            0, 0, 0, 0, 0));
                else // capture move
                    move_list.add_move(Move(source_square, target_square,
                                            side == white ? BoardPiece::K : BoardPiece::k,
                                            0, 1, 0, 0, 0));
                
                attacks.pop_bit(target_square);
            }
            
            bitboard.pop_bit(source_square);
        }
    }
    
public:
    Moves generate_moves() const{
        Moves move_list;
        
        generate_pawn_moves(move_list);
        generate_castling_moves(move_list);
        generate_knight_moves(move_list);
        generate_bishop_moves(move_list);
        generate_rook_moves(move_list);
        generate_queen_moves(move_list);
        generate_king_moves(move_list);
        
        return move_list;
    }
    
    int evalutate() const {
        int score = 0;
        BitBoard bitboard;
        
        BoardPiece::Pieces piece;
        BitBoardSquare square;
        
        for (int bb_piece = BoardPiece::P; bb_piece <= BoardPiece::k; bb_piece++) {
            bitboard = bitboards[bb_piece];
            
            while (bitboard) {
                piece = BoardPiece::Pieces(bb_piece);
                
                square = BitBoardSquare(get_ls1b_index(bitboard));
                
                score += Evaluation::material_score[piece];
                
                switch (piece) {
                    // evaluate white pieces
                    case BoardPiece::P: score += Evaluation::pawn_score[square]; break;
                    case BoardPiece::N: score += Evaluation::knight_score[square]; break;
                    case BoardPiece::B: score += Evaluation::bishop_score[square]; break;
                    case BoardPiece::R: score += Evaluation::rook_score[square]; break;
                    case BoardPiece::K: score += Evaluation::king_score[square]; break;
                        
                    // evaluate place pieces:
                    case BoardPiece::p: score -= Evaluation::pawn_score[mirror_score[square]]; break;
                    case BoardPiece::n: score -= Evaluation::knight_score[mirror_score[square]]; break;
                    case BoardPiece::b: score -= Evaluation::bishop_score[mirror_score[square]]; break;
                    case BoardPiece::r: score -= Evaluation::rook_score[mirror_score[square]]; break;
                    case BoardPiece::k: score -= Evaluation::king_score[mirror_score[square]]; break;
                }
                
                bitboard.pop_bit(square);
            }
        }
        
        return (side == white) ? score : -score; 
    }
};





#endif /* bitmanipulation_hpp */
