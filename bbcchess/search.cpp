// system headers
#include <iostream>
#include <numeric>

// local headers
#include "search.hpp"
#include "pieces.hpp"
#include "time_control.hpp"
#include "evaluation.hpp"
#include "zorbist.hpp"

bool Search::is_repetition(BoardRepresentation const & rep) {
    for (int index = 0; index < repetition_index; index++) {
        if (repetition_table[index] == rep.hash_key)
            return true;
    }
    
    return false;
}

void Search::enable_pv_sorting(Moves const & move_list) {
    follow_pv = false;
    
    for (int count = 0; count < move_list.count; count++) {
        if (pv_table[0][ply] == move_list.moves[count]) {
            follow_pv = true;
            break;
        }
    }
}

int Search::score_move(Move const & move, BoardRepresentation const & rep) {
    if (follow_pv && pv_table[0][ply] == move) {
        return 20000;
    }
    
    if (move.get_move_capture()) { // score capture move
        BoardPiece::Pieces target_piece = BoardPiece::P;
        BoardPiece::Pieces start_piece, end_piece;
        
        if (rep.side == white) {
            start_piece = BoardPiece::p;
            end_piece = BoardPiece::k;
        } else {
            start_piece = BoardPiece::P;
            end_piece = BoardPiece::K;
        }
        
        for (int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
            if (BitBoard::get_bit(rep.bitboards[bb_piece], move.get_move_target())) {
                target_piece = BoardPiece::Pieces(bb_piece);
                break;
            }
        }
        
        return Evaluation::mvv_lva[piece_to_index[move.get_move_piece()]][piece_to_index[target_piece]] + 10000;
    } else { // score quiet move
        if (killer_moves[0][ply] == move)
            return 9000;
        else if (killer_moves[1][ply] == move)
            return 8000;
        else
            return history_moves[move.get_move_piece()][move.get_move_target()];
    }
    
    return 0;
}

void Search::print_move_scores(Moves const & move_list, BoardRepresentation const & rep) {
    std::cout << "     Move scores:\n\n";
    
    for (int count = 0; count < move_list.count; count++) {
        std::cout << "     move: ";
        move_list.moves[count].print_move_nonewline();
        std::cout << " score: " << score_move(move_list.moves[count], rep) << "\n";
    }
}

int Search::sort_moves(Moves & move_list, BoardRepresentation const & rep) {
    // move scores
    int move_scores[256];
    
    for (int count = 0; count < move_list.count; count++)
        move_scores[count] = score_move(move_list.moves[count], rep);

    
    int move_idx[256];
    std::iota(move_idx, move_idx + move_list.count, 0);
    std::stable_sort(move_idx, move_idx + move_list.count, [&move_scores](int a, int b) {
        return move_scores[a] > move_scores[b];
    });
    
    Move move_copy[256];
    std::copy(move_list.moves, move_list.moves + move_list.count, move_copy);
    
    for(int i = 0; i < move_list.count; i++) move_list.moves[i] = move_copy[move_idx[i]];
    
    
    return 0;
}

int Search::quiescence(int alpha, int beta, BoardRepresentation const & rep) {
    if ((nodes & 2047) == 0) communicate();
    
    nodes++;
    
    if (ply > MAX_PLY - 1)
        return rep.evaluate();
    
    // evaluate position
    int evaluation = rep.evaluate();
    
    // beta cut off
    if (evaluation >= beta)
        return beta;
    
    if (evaluation > alpha)
        alpha = evaluation;
    
    Moves move_list = rep.generate_moves();
    sort_moves(move_list, rep);
    
    for (int count = 0; count < move_list.count; count++) {
        ply++;
        
        repetition_index++;
        repetition_table[repetition_index] = rep.hash_key;
        
        int move_made;
        BoardRepresentation new_rep = rep.copy_and_move(move_list.moves[count], only_captures, &move_made);
        
        if (!move_made) {
            ply--;
            
            repetition_index--;
            
            continue;
        }
        
        int score = -quiescence(-beta, -alpha, new_rep);
        
        ply--;
        
        repetition_index--;
        
        if (time_control.stopped) return 0;
        
        if (score > alpha) {
            alpha = score;
            
            // beta cut off
            if (score >= beta)
                return beta;
        }
    }
    
    return alpha;
}

bool Search::ok_to_reduce(Move const & move, bool in_check) {
    return !in_check && !move.get_move_capture() && !move.get_move_promoted();
}

int Search::negascout(int alpha, int beta, int depth, BoardRepresentation & rep, bool allow_null) {
    int score;
    
    int hash_flag = hash_flag_alpha;
    
    if ((ply && is_repetition(rep)) || rep.fifty >= 100)
        return 0; // draw by repetition
    
    pv_length[ply] = ply;
    
    bool pv_node = beta - alpha > 1;
    
    /*
     read hash entry if we're not in a root ply and hash entry is available and current node is not a PV node
     */
    if (ply && (score = hash_table.read_hash_entry(alpha, beta, depth, ply, rep)) != no_hash_entry && !pv_node)
        return score;
             
    
    if ((nodes & 2047) == 0) communicate();
    
    if (depth == 0)
        return quiescence(alpha, beta, rep);
    
    // we are too deep, hence there's an overflow of arrays relying on max ply constant
    if (ply > MAX_PLY - 1)
        // evaluate position
        return rep.evaluate();
    
    nodes++;
    
    bool in_check = rep.is_square_attacked(BitBoardSquare(rep.side == white ? get_ls1b_index(rep.bitboards[BoardPiece::K]) :
                                                                                           get_ls1b_index(rep.bitboards[BoardPiece::k])),
                                           TurnColor(rep.side ^ 1));
    if (in_check) depth++;
    
    int legal_moves = 0, moves_searched = 0;
    
    // null move pruning
    if (allow_null && depth >= 1 + R && !in_check) {
        // make null move
        
        ply++;
        
        repetition_index++;
        repetition_table[repetition_index] = rep.hash_key;
        
        U64 og_hash_key = rep.hash_key;
        
        rep.hash_key ^= Zorbist::side_key;
        rep.side = TurnColor(rep.side ^ 1);
        
        if (rep.enpassant != no_sq) rep.hash_key ^= Zorbist::enpassant_keys[rep.enpassant];
        BitBoardSquare og_square = rep.enpassant;
        rep.enpassant = no_sq;

        
        score = -negascout(-beta, -beta + 1, depth - 1 - R, rep, false); // do not allow consequtive null moves
        
        // restore null move
        rep.side = TurnColor(rep.side ^ 1);
        rep.enpassant = og_square;
        rep.hash_key = og_hash_key;
        
        ply--;
        
        repetition_index--;
        
        if (time_control.stopped) return 0;
        
        if (score >= beta)
            return beta;
    }
    
    Moves move_list = rep.generate_moves();
    if (follow_pv) enable_pv_sorting(move_list);
    sort_moves(move_list, rep);
    
    for (int count = 0; count < move_list.count; count++) {
        ply++;
        
        repetition_index++;
        repetition_table[repetition_index] = rep.hash_key;
        
        int move_made;
        BoardRepresentation new_rep = rep.copy_and_move(move_list.moves[count], all_moves, &move_made);
        
        if (!move_made) {
            ply--;
            
            repetition_index--;
            
            continue;
        }
        
        legal_moves++;
        
        if (moves_searched == 0) { // assume principle variation
            // normal search
            score = -negascout(-beta, -alpha, depth - 1, new_rep, true);
        } else {
            // try late move reduction
            if (moves_searched >= full_depth_move &&
                depth >= reduction_limit &&
                ok_to_reduce(move_list.moves[count], in_check)) {
                // search in reduced depth
                score = -negascout(-alpha - 1, -alpha, depth - 2, new_rep, true);
            } else score = alpha + 1; // reduction requirement not met do full depth search
            
            if (score > alpha) { // reduction failed
                // get the score to see if move is potentially better
                score = -negascout(-alpha - 1, -alpha, depth - 1, new_rep, true);
                
                // if fail high, we do a re-search to get the exact score
                if (score > alpha && score < beta)
                    score = -negascout(-beta, -alpha, depth - 1, new_rep, true);
            }
        }
        
        ply--;
        
        repetition_index--;
        
        if (time_control.stopped) return 0;
        
        moves_searched++;
        
        // better alpha
        if (score > alpha) {
            hash_flag = hash_flag_exact;
            
            // story history moves
            if (!move_list.moves[count].get_move_capture()) {
                history_moves[move_list.moves[count].get_move_piece()][move_list.moves[count].get_move_target()] += depth;
            }
            
            alpha = score;
            
            // write PV move
            pv_table[ply][ply] = move_list.moves[count];
            
            // loop over the next ply
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            
            pv_length[ply] = pv_length[ply + 1];
            
            // beta cutoff
            if (score >= beta) {
                hash_table.write_hash_entry(beta, depth, ply, hash_flag_beta, rep);
                
                // store killer moves on quite moves
                if (!move_list.moves[count].get_move_capture()) {
                    killer_moves[1][ply] = killer_moves[0][ply];
                    killer_moves[0][ply] = move_list.moves[count];
                }
                
                return beta;
            }
        }
    }
    
    if (!legal_moves) {
        // checkmate
        if (in_check)
            return -MATE_VALUE + ply;
        
        else // stalemate
            return 0;
    }
    
    hash_table.write_hash_entry(alpha, depth, hash_flag, ply, rep);
    
    // node (move) fails low
    return alpha;
}
