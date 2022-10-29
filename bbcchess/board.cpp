// local headers
#include "board.hpp"
#include "util.hpp"
#include "zorbist.hpp"
#include "attacktables.hpp"
#include "evaluation.hpp"
#include "nnue_eval.hpp"

void BoardRepresentation::print_board() const {
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
                if (BitBoard::get_bit(bitboards[bb_piece], square)) {
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
    std::cout << "     Hash key:  " << std::hex << hash_key << std::dec << "\n";
    std::cout << "     Fifty move:  " << fifty << "\n\n";
}

void BoardRepresentation::print_attacked_square(TurnColor side) const {
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

U64 BoardRepresentation::generate_hash_key() const {
    U64 final_key = 0ULL;
    U64 bitboard;
    
    for (int piece = BoardPiece::P; piece <= BoardPiece::k; piece++) {
        bitboard = bitboards[piece];
        
        while (bitboard) {
            int square = get_ls1b_index(bitboard);
            final_key ^= Zorbist::pieces_keys[piece][square];
            
            BitBoard::pop_bit(bitboard, square);
        }
    }
    
    if (enpassant != no_sq)
        final_key ^= Zorbist::enpassant_keys[enpassant];
    
    final_key ^= Zorbist::castle_keys[castle];
    
    if (side == black) final_key ^= Zorbist::side_key;
    
    return final_key;
}

bool BoardRepresentation::is_square_attacked(BitBoardSquare square, TurnColor side) const {
    if ((side == white) && (bitboards[BoardPiece::P] & AttackTables::Pawn::pawn_attacks[black][square])) return true;
    else if ((side == black) && (bitboards[BoardPiece::p] & AttackTables::Pawn::pawn_attacks[white][square])) return true;
    else if ((side == white ? bitboards[BoardPiece::N] : bitboards[BoardPiece::n]) & AttackTables::Knight::knight_attacks[square]) return true;
    else if ((side == white ? bitboards[BoardPiece::B] : bitboards[BoardPiece::b]) & AttackTables::Bishop::get_bishop_attacks(square, occupancies[both])) return true;
    else if ((side == white ? bitboards[BoardPiece::R] : bitboards[BoardPiece::r]) & AttackTables::Rook::get_rook_attacks(square, occupancies[both])) return true;
    else if ((side == white ? bitboards[BoardPiece::Q] : bitboards[BoardPiece::q]) & AttackTables::Queen::get_queen_attacks(square, occupancies[both])) return true;
    else if ((side == white ? bitboards[BoardPiece::K] : bitboards[BoardPiece::k]) & AttackTables::King::king_attacks[square]) return true;
    
    return false;
}

int BoardRepresentation::make_move(Move const & move, MoveFlag move_flag) {
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
        BitBoard::pop_bit(bitboards[piece], source_square);
        BitBoard::set_bit(bitboards[piece], target_square);
        
        BitBoard::pop_bit(occupancies[side], source_square);
        BitBoard::set_bit(occupancies[side], target_square);
        
        hash_key ^= Zorbist::pieces_keys[piece][source_square];
        hash_key ^= Zorbist::pieces_keys[piece][target_square];
        
        fifty++;
        if (piece == BoardPiece::P || piece == BoardPiece::p)
            fifty = 0;
        
        if (capture) {
            fifty = 0;
            BoardPiece::Pieces start_piece, end_piece;
            
            if (side == white) {
                start_piece = BoardPiece::p;
                end_piece = BoardPiece::k;
            } else {
                start_piece = BoardPiece::P;
                end_piece = BoardPiece::K;
            }
            
            for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
                if (BitBoard::get_bit(bitboards[bb_piece], target_square)) {
                    // remove piece from target square
                    BitBoard::pop_bit(bitboards[bb_piece], target_square);
                    
                    // remove the piece from hash_key
                    hash_key ^= Zorbist::pieces_keys[bb_piece][target_square];
                    break;
                }
            }
            
            BitBoard::pop_bit(occupancies[side ^ 1], target_square);
        }
        
        if (promoted) {
            BitBoard::pop_bit(bitboards[side == white ? BoardPiece::P : BoardPiece::p], target_square);
            hash_key ^= Zorbist::pieces_keys[side == white ? BoardPiece::P : BoardPiece::p][target_square];
            
            BitBoard::set_bit(bitboards[promoted], target_square);
            hash_key ^= Zorbist::pieces_keys[promoted][target_square];
        }
        
        if (enpass) {
            side == white ? BitBoard::pop_bit(bitboards[BoardPiece::p], target_square + 8) :
            BitBoard::pop_bit(bitboards[BoardPiece::P], target_square - 8);
            
            hash_key ^= side == white ? Zorbist::pieces_keys[BoardPiece::p][target_square + 8] : Zorbist::pieces_keys[BoardPiece::P][target_square - 8];
            
            side == white ? BitBoard::pop_bit(occupancies[black], target_square + 8) :
            BitBoard::pop_bit(occupancies[white], target_square - 8);
        }
        
        if (enpassant != no_sq) hash_key ^= Zorbist::enpassant_keys[enpassant];
        
        enpassant = no_sq;
        
        if (double_push) {
            side == white ? enpassant = BitBoardSquare(target_square + 8) :
            enpassant = BitBoardSquare(target_square - 8);
            
            hash_key ^= Zorbist::enpassant_keys[side == white ? target_square + 8 : target_square - 8];
        }
        
        if (castling) {
            switch (target_square) {
                    // white king side
                case g1:
                    BitBoard::pop_bit(bitboards[BoardPiece::R], h1);
                    BitBoard::set_bit(bitboards[BoardPiece::R], f1);
                    
                    hash_key ^= Zorbist::pieces_keys[BoardPiece::R][h1];
                    hash_key ^= Zorbist::pieces_keys[BoardPiece::R][f1];
                    
                    BitBoard::pop_bit(occupancies[white], h1);
                    BitBoard::set_bit(occupancies[white], f1);
                    break;
                    
                    // white queen side
                case c1:
                    BitBoard::pop_bit(bitboards[BoardPiece::R], a1);
                    BitBoard::set_bit(bitboards[BoardPiece::R], d1);
                    
                    hash_key ^= Zorbist::pieces_keys[BoardPiece::R][a1];
                    hash_key ^= Zorbist::pieces_keys[BoardPiece::R][d1];
                    
                    BitBoard::pop_bit(occupancies[white], a1);
                    BitBoard::set_bit(occupancies[white], d1);
                    break;
                    
                    // black king side
                case g8:
                    BitBoard::pop_bit(bitboards[BoardPiece::r], h8);
                    BitBoard::set_bit(bitboards[BoardPiece::r], f8);
                    
                    hash_key ^= Zorbist::pieces_keys[BoardPiece::r][h8];
                    hash_key ^= Zorbist::pieces_keys[BoardPiece::r][f8];
                    
                    BitBoard::pop_bit(occupancies[black], h8);
                    BitBoard::set_bit(occupancies[black], f8);
                    break;
                    
                    // black queen side
                case c8:
                    BitBoard::pop_bit(bitboards[BoardPiece::r], a8);
                    BitBoard::set_bit(bitboards[BoardPiece::r], d8);
                    
                    hash_key ^= Zorbist::pieces_keys[BoardPiece::r][a8];
                    hash_key ^= Zorbist::pieces_keys[BoardPiece::r][d8];
                    
                    BitBoard::pop_bit(occupancies[black], a8);
                    BitBoard::set_bit(occupancies[black], d8);
                    break;
                    
                default:
                    fatal_exit("invalid castling target square");
            }
        }
        
        hash_key ^= Zorbist::castle_keys[castle];
        
        // update castling right
        castle &= castling_rights[source_square];
        castle &= castling_rights[target_square];
        
        hash_key ^= Zorbist::castle_keys[castle];
        
        occupancies[both] = occupancies[white] | occupancies[black];
        
        side = TurnColor(side ^ 1);
        
        hash_key ^= Zorbist::side_key;
        
        
#ifdef DEBUG_ZORBIST
        //
        // ====== debug hash key incremental update =======
        //
        
        // build hash key for the updated position (after move is made0 from scratch
        U64 hash_from_scratch = generate_hash_key();
        
        if (hash_key != hash_from_scratch) {
            std::cout << "\n\nMake move\n";
            std::cout << "move: ";
            move.print_move_nonewline();
            std::cout << "hash key should be: " << std::hex << hash_from_scratch << std::dec << "\n";
            getchar();
        }
#endif
        
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

BoardRepresentation BoardRepresentation::copy_and_move(Move const & move, MoveFlag move_flag, int* move_made) const {
    BoardRepresentation copy = *this;
    int success = copy.make_move(move, move_flag);
    if (move_made) *move_made = success;
    return copy;
}

Moves BoardRepresentation::generate_moves() const {
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

int BoardRepresentation::get_game_phase_score() const {
    /*
     The game phase score of the game is derived from the pieces
     (not counting pawns and kings) that are still on the board.
     The full materal starting position game phase score is:
     
     4 * knight score open +
     4 * bishop score open +
     4 * rook score open +
     2 * queen score open +
     */
    
    int white_piece_scores = 0, black_piece_scores = 0;
    
    for (int piece = BoardPiece::N; piece <= BoardPiece::Q; piece++)
        white_piece_scores += count_bits(bitboards[piece]) * Evaluation::material_score[opening][piece];
    
    for (int piece = BoardPiece::n; piece <= BoardPiece::q; piece++)
        black_piece_scores += count_bits(bitboards[piece]) * -Evaluation::material_score[opening][piece];
    
    return white_piece_scores + black_piece_scores;
}

int BoardRepresentation::hand_evaluate() const {
    int game_phase_score = get_game_phase_score();
    
    GamePhase game_phase = middlegame;
    
    if (game_phase_score > Evaluation::opening_phase_score) game_phase = opening;
    else if (game_phase_score < Evaluation::endgame_phase_score) game_phase = endgame;
    
    int score = 0, opening_score = 0, endgame_score = 0;
    U64 bitboard;
    
    BoardPiece::Pieces piece;
    BitBoardSquare square;
    
    int double_pawns = 0;
    
    for (int bb_piece = BoardPiece::P; bb_piece <= BoardPiece::k; bb_piece++) {
        bitboard = bitboards[bb_piece];
        
        while (bitboard) {
            piece = BoardPiece::Pieces(bb_piece);
            
            square = BitBoardSquare(get_ls1b_index(bitboard));
            
            opening_score += Evaluation::material_score[opening][piece] * 2;
            endgame_score += Evaluation::material_score[endgame][piece] * 2;
            
            opening_score += Evaluation::material_score[opening][piece];
            endgame_score += Evaluation::material_score[endgame][piece];
            
            switch (piece) {
                    // evaluate white pieces
                case BoardPiece::P: // white pawn
                    opening_score += Evaluation::positional_score[opening][BoardPiece::PAWN][square];
                    endgame_score += Evaluation::positional_score[endgame][BoardPiece::PAWN][square];
                    
                    
                    double_pawns = count_bits(bitboards[BoardPiece::P] & Evaluation::file_masks[square]);
                    
                    if (double_pawns > 1) {
                        opening_score += double_pawns * Evaluation::double_pawn_penalty_opening;
                        endgame_score += double_pawns * Evaluation::double_pawn_penalty_endgame;
                    }
                    
                    if ((bitboards[BoardPiece::P] & Evaluation::isolated_masks[square]) == 0) {
                        opening_score += Evaluation::isolated_pawn_penalty_opening;
                        endgame_score += Evaluation::isolated_pawn_penalty_endgame;
                    }
                    
                    if ((Evaluation::passed_masks[white][square] & bitboards[BoardPiece::p]) == 0) {
                        opening_score += Evaluation::passed_pawn_bonus[Evaluation::get_rank[square]];
                        endgame_score += Evaluation::passed_pawn_bonus[Evaluation::get_rank[square]];
                    }
                    
                    break;
                case BoardPiece::N: // white knight
                    opening_score += Evaluation::positional_score[opening][BoardPiece::KNIGHT][square];
                    endgame_score += Evaluation::positional_score[endgame][BoardPiece::KNIGHT][square];
                    
                    break;
                case BoardPiece::B:
                    opening_score += Evaluation::positional_score[opening][BoardPiece::BISHOP][square];
                    endgame_score += Evaluation::positional_score[endgame][BoardPiece::BISHOP][square];
                    
                    // mobility
                    opening_score += (count_bits(AttackTables::Bishop::get_bishop_attacks(square, occupancies[both])) - Evaluation::bishop_unit) * Evaluation::bishop_mobility_opening;
                    endgame_score += (count_bits(AttackTables::Bishop::get_bishop_attacks(square, occupancies[both])) - Evaluation::bishop_unit) * Evaluation::bishop_mobility_endgame;
                    
                    break;
                case BoardPiece::R:
                    opening_score += Evaluation::positional_score[opening][BoardPiece::ROOK][square];
                    endgame_score += Evaluation::positional_score[endgame][BoardPiece::ROOK][square];
                    
                    
                    // semi open file
                    if (!(bitboards[BoardPiece::P] & Evaluation::file_masks[square])) {
                        opening_score += Evaluation::semi_open_file_score;
                        endgame_score += Evaluation::semi_open_file_score;
                    }
                    
                    // open file
                    if(!((bitboards[BoardPiece::P] | bitboards[BoardPiece::p]) & Evaluation::file_masks[square])) {
                        opening_score += Evaluation::open_file_score;
                        endgame_score += Evaluation::open_file_score;
                    }
                    
                    
                    break;
                case BoardPiece::Q:
                    opening_score += Evaluation::positional_score[opening][BoardPiece::QUEEN][square];
                    endgame_score += Evaluation::positional_score[endgame][BoardPiece::QUEEN][square];
                    
                    // mobility
                    opening_score += (count_bits(AttackTables::Queen::get_queen_attacks(square, occupancies[both])) - Evaluation::queen_unit) * Evaluation::queen_mobility_opening;
                    endgame_score += (count_bits(AttackTables::Queen::get_queen_attacks(square, occupancies[both])) - Evaluation::queen_unit) * Evaluation::queen_mobility_endgame;
                    break;
                case BoardPiece::K:
                    opening_score += Evaluation::positional_score[opening][BoardPiece::KING][square];
                    endgame_score += Evaluation::positional_score[endgame][BoardPiece::KING][square];
                    
                    // semi open file
                    if (!(bitboards[BoardPiece::P] & Evaluation::file_masks[square])) {
                        opening_score -= Evaluation::semi_open_file_score;
                        endgame_score -= Evaluation::semi_open_file_score;
                    }
                    
                    // open file
                    if(!((bitboards[BoardPiece::P] | bitboards[BoardPiece::p]) & Evaluation::file_masks[square])) {
                        opening_score -= Evaluation::open_file_score;
                        endgame_score -= Evaluation::open_file_score;
                    }
                    
                    // king safety
                    opening_score += count_bits(AttackTables::King::king_attacks[square] & occupancies[white]) * Evaluation::king_shield_bonus;
                    endgame_score += count_bits(AttackTables::King::king_attacks[square] & occupancies[white]) * Evaluation::king_shield_bonus;
                    
                    break;
                    
                    
                    // evaluate place pieces:
                case BoardPiece::p:
                    opening_score -= Evaluation::positional_score[opening][BoardPiece::PAWN][mirror_score[square]];
                    endgame_score -= Evaluation::positional_score[endgame][BoardPiece::PAWN][mirror_score[square]];
                    
                    double_pawns = count_bits(bitboards[BoardPiece::p] & Evaluation::file_masks[square]);
                    
                    if (double_pawns > 1) {
                        opening_score -= double_pawns * Evaluation::double_pawn_penalty_opening;
                        endgame_score -= double_pawns * Evaluation::double_pawn_penalty_endgame;
                    }
                    
                    if ((bitboards[BoardPiece::p] & Evaluation::isolated_masks[square]) == 0) {
                        opening_score -= Evaluation::isolated_pawn_penalty_opening;
                        endgame_score -= Evaluation::isolated_pawn_penalty_endgame;
                    }
                    
                    if ((Evaluation::passed_masks[black][square] & bitboards[BoardPiece::P]) == 0) {
                        opening_score -= Evaluation::passed_pawn_bonus[Evaluation::get_rank[mirror_score[square]]];
                        endgame_score -= Evaluation::passed_pawn_bonus[Evaluation::get_rank[mirror_score[square]]];
                    }
                    
                    break;
                case BoardPiece::n:
                    opening_score -= Evaluation::positional_score[opening][BoardPiece::KNIGHT][mirror_score[square]];
                    endgame_score -= Evaluation::positional_score[endgame][BoardPiece::KNIGHT][mirror_score[square]];
                    
                    break;
                case BoardPiece::b:
                    opening_score -= Evaluation::positional_score[opening][BoardPiece::BISHOP][mirror_score[square]];
                    endgame_score -= Evaluation::positional_score[endgame][BoardPiece::BISHOP][mirror_score[square]];
                    
                    opening_score -= (count_bits(AttackTables::Bishop::get_bishop_attacks(square, occupancies[both])) - Evaluation::bishop_unit) * Evaluation::bishop_mobility_opening;
                    endgame_score -= (count_bits(AttackTables::Bishop::get_bishop_attacks(square, occupancies[both])) - Evaluation::bishop_unit) * Evaluation::bishop_mobility_endgame;
                    break;
                case BoardPiece::r:
                    opening_score -= Evaluation::positional_score[opening][BoardPiece::ROOK][mirror_score[square]];
                    endgame_score -= Evaluation::positional_score[endgame][BoardPiece::ROOK][mirror_score[square]];
                    
                    // semi open file
                    if (!(bitboards[BoardPiece::p] & Evaluation::file_masks[square])) {
                        opening_score -= Evaluation::semi_open_file_score;
                        endgame_score -= Evaluation::semi_open_file_score;
                    }
                    
                    // open file
                    if(!((bitboards[BoardPiece::P] | bitboards[BoardPiece::p]) & Evaluation::file_masks[square])) {
                        opening_score -= Evaluation::open_file_score;
                        endgame_score -= Evaluation::open_file_score;
                    }
                    
                    break;
                case BoardPiece::q:
                    opening_score -= Evaluation::positional_score[opening][BoardPiece::QUEEN][mirror_score[square]];
                    endgame_score -= Evaluation::positional_score[endgame][BoardPiece::QUEEN][mirror_score[square]];
                    
                    // mobility
                    opening_score -= (count_bits(AttackTables::Queen::get_queen_attacks(square, occupancies[both])) - Evaluation::queen_unit) * Evaluation::queen_mobility_opening;
                    endgame_score -= (count_bits(AttackTables::Queen::get_queen_attacks(square, occupancies[both])) - Evaluation::queen_unit) * Evaluation::queen_mobility_endgame;
                    
                    break;
                case BoardPiece::k:
                    opening_score -= Evaluation::positional_score[opening][BoardPiece::KING][mirror_score[square]];
                    endgame_score -= Evaluation::positional_score[endgame][BoardPiece::KING][mirror_score[square]];
                    
                    // semi open file
                    if (!(bitboards[BoardPiece::p] & Evaluation::file_masks[square])) {
                        opening_score += Evaluation::semi_open_file_score;
                        endgame_score += Evaluation::semi_open_file_score;
                    }
                    
                    // open file
                    if(!((bitboards[BoardPiece::P] | bitboards[BoardPiece::p]) & Evaluation::file_masks[square])) {
                        opening_score += Evaluation::open_file_score;
                        endgame_score += Evaluation::open_file_score;
                    }
                    
                    // king safety
                    opening_score -= count_bits(AttackTables::King::king_attacks[square] & occupancies[black]) * Evaluation::king_shield_bonus;
                    endgame_score -= count_bits(AttackTables::King::king_attacks[square] & occupancies[black]) * Evaluation::king_shield_bonus;
                    break;
                    
            }
            
            BitBoard::pop_bit(bitboard, square);
        }
    }
    
    
    
    if (game_phase == middlegame) score = Evaluation::interpolate_score(opening_score, endgame_score, game_phase_score);
    else score = game_phase == opening ? opening_score : endgame_score;
    
    
    return side == white ? score : -score;
}

int BoardRepresentation::evaluate() const {
    U64 bitboard;
    
    BoardPiece::Pieces piece;
    BitBoardSquare square;
    
    // array of piece codes converted to Stockfish piece codes
    std::array<int, 33> pieces;
    
    // array of square indices converted to Stockfish square indices
    std::array<int, 33> squares;
    
    // pieces and squares current index to write next piece square pair at
    int index = 2;
    
    for (int bb_piece = BoardPiece::P; bb_piece <= BoardPiece::k; bb_piece++) {
        bitboard = bitboards[bb_piece];
        
        while (bitboard) {
            piece = BoardPiece::Pieces(bb_piece);
            
            square = BitBoardSquare(get_ls1b_index(bitboard));
            
            if (piece == BoardPiece::K) {
                pieces[0] = Evaluation::nnue_pieces[piece];
                squares[0] = Evaluation::nnue_squares[square];
            } else if (piece == BoardPiece::k) {
                pieces[1] = Evaluation::nnue_pieces[piece];
                squares[1] = Evaluation::nnue_squares[square];
            } else {
                pieces[index] = Evaluation::nnue_pieces[piece];
                squares[index] = Evaluation::nnue_squares[square];
                index++;
            }
            
            BitBoard::pop_bit(bitboard, square);
        }
    }
    
    pieces[index] = 0;
    squares[index] = 0;
    
    return (evaluate_nnue(side, &pieces[0], &squares[0]) * (100 - fifty) / 100);
}

void BoardRepresentation::generate_pawn_moves(Moves& move_list) const {
    int source_square, target_square;
    
    U64 bitboard = side == white ? bitboards[BoardPiece::P] : bitboards[BoardPiece::p];
    U64 attacks;
    
    while (bitboard) {
        source_square = get_ls1b_index(bitboard);
        
        // get the square above source square
        target_square = side == white ? source_square - 8 : source_square + 8;
        
        // if target square is not off the board
        if ((side == white && !(target_square < a8) && !BitBoard::get_bit(occupancies[both], target_square)) ||
            (side == black && !(target_square > h1) && !BitBoard::get_bit(occupancies[both], target_square))) {
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
                if ((side == white && (source_square >= a2 && source_square <= h2) && !BitBoard::get_bit(occupancies[both], target_square - 8)) ||
                    (side == black && (source_square >= a7 && source_square <= h7) && !BitBoard::get_bit(occupancies[both], target_square + 8)))
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
            
            BitBoard::pop_bit(attacks, target_square);
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
        
        BitBoard::pop_bit(bitboard, source_square);
    }
}

void BoardRepresentation::generate_castling_moves(Moves& move_list) const{
    if ((side == white && (castle & wk)) ||
        (side == black && (castle & bk))) {
        if ((side == white && !BitBoard::get_bit(occupancies[both], f1) && !BitBoard::get_bit(occupancies[both], g1)) ||
            (side == black && !BitBoard::get_bit(occupancies[both], f8) && !BitBoard::get_bit(occupancies[both], g8))) {
            
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
        if ((side == white && !BitBoard::get_bit(occupancies[both], d1) && !BitBoard::get_bit(occupancies[both], c1) && !BitBoard::get_bit(occupancies[both], b1)) ||
            (side == black && !BitBoard::get_bit(occupancies[both], d8) && !BitBoard::get_bit(occupancies[both], c8) && !BitBoard::get_bit(occupancies[both], b8))) {
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

void BoardRepresentation::generate_knight_moves(Moves& move_list) const {
    int source_square, target_square;
    
    U64 bitboard = side == white ? bitboards[BoardPiece::N] : bitboards[BoardPiece::n];
    U64 attacks;
    
    while(bitboard) {
        source_square = get_ls1b_index(bitboard);
        
        attacks = AttackTables::Knight::knight_attacks[source_square] & ~(side == white ? occupancies[white] : occupancies[black]);
        
        while (attacks) {
            target_square = get_ls1b_index(attacks);
            
            // quite move
            if (!BitBoard::get_bit(side == white ? occupancies[black] : occupancies[white], target_square))
                move_list.add_move(Move(source_square, target_square,
                                        side == white ? BoardPiece::N : BoardPiece::n,
                                        0, 0, 0, 0, 0));
            else // capture move
                move_list.add_move(Move(source_square, target_square,
                                        side == white ? BoardPiece::N : BoardPiece::n,
                                        0, 1, 0, 0, 0));
            
            BitBoard::pop_bit(attacks, target_square);
        }
        
        BitBoard::pop_bit(bitboard, source_square);
    }
}

void BoardRepresentation::generate_bishop_moves(Moves& move_list) const {
    int source_square, target_square;
    
    U64 bitboard = side == white ? bitboards[BoardPiece::B] : bitboards[BoardPiece::b];
    U64 attacks;
    
    while(bitboard) {
        source_square = get_ls1b_index(bitboard);
        
        attacks = AttackTables::Bishop::get_bishop_attacks(BitBoardSquare(source_square), occupancies[both]) & ~(side == white ? occupancies[white] : occupancies[black]);
        
        while (attacks) {
            target_square = get_ls1b_index(attacks);
            
            // quite move
            if (!BitBoard::get_bit(side == white ? occupancies[black] : occupancies[white], target_square))
                move_list.add_move(Move(source_square, target_square,
                                        side == white ? BoardPiece::B : BoardPiece::b,
                                        0, 0, 0, 0, 0));
            else // capture move
                move_list.add_move(Move(source_square, target_square,
                                        side == white ? BoardPiece::B : BoardPiece::b,
                                        0, 1, 0, 0, 0));
            
            BitBoard::pop_bit(attacks, target_square);
        }
        
        BitBoard::pop_bit(bitboard, source_square);
    }
}

void BoardRepresentation::generate_rook_moves(Moves& move_list) const {
    int source_square, target_square;
    
    U64 bitboard = side == white ? bitboards[BoardPiece::R] : bitboards[BoardPiece::r];
    U64 attacks;
    
    while(bitboard) {
        source_square = get_ls1b_index(bitboard);
        
        attacks = AttackTables::Rook::get_rook_attacks(BitBoardSquare(source_square), occupancies[both]) & ~(side == white ? occupancies[white] : occupancies[black]);
        
        while (attacks) {
            target_square = get_ls1b_index(attacks);
            
            // quite move
            if (!BitBoard::get_bit(side == white ? occupancies[black] : occupancies[white], target_square))
                move_list.add_move(Move(source_square, target_square,
                                        side == white ? BoardPiece::R : BoardPiece::r,
                                        0, 0, 0, 0, 0));
            else // capture move
                move_list.add_move(Move(source_square, target_square,
                                        side == white ? BoardPiece::R : BoardPiece::r,
                                        0, 1, 0, 0, 0));
            
            BitBoard::pop_bit(attacks, target_square);
        }
        
        BitBoard::pop_bit(bitboard, source_square);
    }
}

void BoardRepresentation::generate_queen_moves(Moves& move_list) const{
    int source_square, target_square;
    
    U64 bitboard = side == white ? bitboards[BoardPiece::Q] : bitboards[BoardPiece::q];
    U64 attacks;
    
    while(bitboard) {
        source_square = get_ls1b_index(bitboard);
        
        attacks = AttackTables::Queen::get_queen_attacks(BitBoardSquare(source_square), occupancies[both]) & ~(side == white ? occupancies[white] : occupancies[black]);
        
        while (attacks) {
            target_square = get_ls1b_index(attacks);
            
            // quite move
            if (!BitBoard::get_bit(side == white ? occupancies[black] : occupancies[white], target_square))
                move_list.add_move(Move(source_square, target_square,
                                        side == white ? BoardPiece::Q : BoardPiece::q,
                                        0, 0, 0, 0, 0));
            else // capture move
                move_list.add_move(Move(source_square, target_square,
                                        side == white ? BoardPiece::Q : BoardPiece::q,
                                        0, 1, 0, 0, 0));
            
            BitBoard::pop_bit(attacks, target_square);
        }
        
        BitBoard::pop_bit(bitboard, source_square);
    }
}

void BoardRepresentation::generate_king_moves(Moves& move_list) const {
    int source_square, target_square;
    
    U64 bitboard = side == white ? bitboards[BoardPiece::K] : bitboards[BoardPiece::k];
    U64 attacks;
    
    while(bitboard) {
        source_square = get_ls1b_index(bitboard);
        
        attacks = AttackTables::King::king_attacks[source_square] & ~(side == white ? occupancies[white] : occupancies[black]);
        
        while (attacks) {
            target_square = get_ls1b_index(attacks);
            
            // quite move
            if (!BitBoard::get_bit(side == white ? occupancies[black] : occupancies[white], target_square))
                move_list.add_move(Move(source_square, target_square,
                                        side == white ? BoardPiece::K : BoardPiece::k,
                                        0, 0, 0, 0, 0));
            else // capture move
                move_list.add_move(Move(source_square, target_square,
                                        side == white ? BoardPiece::K : BoardPiece::k,
                                        0, 1, 0, 0, 0));
            
            BitBoard::pop_bit(attacks, target_square);
        }
        
        BitBoard::pop_bit(bitboard, source_square);
    }
}
