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
        BoardPiece::Pieces piece = BoardPiece::Pieces(move.get_move_piece());
        
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
        
        return Evaluation::mvv_lva[piece_to_index[piece]][piece_to_index[target_piece]] + 10000;
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

int Search::sort_moves(Moves & move_list, Move best_move, BoardRepresentation const & rep) {
    // move scores
    int move_scores[256];
    
    for (int count = 0; count < move_list.count; count++) {
        if (best_move == move_list.moves[count])
            move_scores[count] = 30000;
        
        else
            move_scores[count] = score_move(move_list.moves[count], rep);
    }
    
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

int Search::evaluate_position(BoardRepresentation const & rep) {
    int eval = eval_table.read_hash_entry(rep);
    if(eval == no_hash_entry) {
        eval = rep.evaluate();
        eval_table.write_hash_entry(eval, rep);
    }
    
    return eval;
}

int Search::quiescence(int alpha, int beta, BoardRepresentation const & rep) {
    if ((nodes & 2047) == 0) communicate();
    
    nodes++;
    
    if (ply > MAX_PLY - 1)
        return evaluate_position(rep);
    
    // evaluate position
    int evaluation = evaluate_position(rep);
    
    // beta cut off
    if (evaluation >= beta)
        return beta;
     
    if (evaluation > alpha)
        alpha = evaluation;
    
    Moves move_list = rep.generate_moves(true);
    sort_moves(move_list, 0, rep);
    
    // Delta pruning based on position evaluation
    int big_delta = Evaluation::absolute_material_score[BoardPiece::Q] + (move_list.has_promotion ? (Evaluation::absolute_material_score[BoardPiece::Q] - 400) : 0);
    if (evaluation < alpha - big_delta)
        return alpha;
    
    for (int count = 0; count < move_list.count; count++) {
        int cap_piece = rep.get_piece_on_square(BitBoardSquare(move_list.moves[count].get_move_target()));
        
        // delta pruning based on current move
        if ((evaluation + Evaluation::absolute_material_score[cap_piece] + 400 < alpha) &&
            (rep.piece_material[rep.side ^ 1] - Evaluation::absolute_material_score[cap_piece] > Evaluation::endgame_phase_score) &&
            (!move_list.moves[count].get_move_promoted()))
            continue;
        
        
        // Do not search bad captures
        // Capturing with lower or equaled valued piece cannot be bad
        // Capturing last remaining piece could lead to winning position
        // Use SEE for others
        if (Evaluation::absolute_material_score_see[move_list.moves[count].get_move_piece()]
            > Evaluation::absolute_material_score_see[cap_piece] &&
            rep.piece_material[rep.side ^ 1] - Evaluation::absolute_material_score[cap_piece] > 0 &&
            rep.see(BitBoardSquare(move_list.moves[count].get_move_target()),
                    BoardPiece::Pieces(cap_piece),
                    BitBoardSquare(move_list.moves[count].get_move_source()),
                    BoardPiece::Pieces(move_list.moves[count].get_move_piece())) < 0)
            continue;
        
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
    pv_length[ply] = ply;
    
    int score, best_score;
    
    Move best_move = 0;
    
    int hash_flag = hash_flag_alpha;
    
    if ((ply && is_repetition(rep)) || rep.fifty >= 100)
        return 0; // draw by repetition
    
    
    bool pv_node = beta - alpha > 1;
    
    /*
     read hash entry if we're not in a root ply and hash entry is available and current node is not a PV node
     */
    if (ply && (score = hash_table.read_hash_entry(alpha, beta, &best_move, depth, ply, rep)) != no_hash_entry && !pv_node)
        return score;
    
    
    if ((nodes & 2047) == 0) communicate();
    
    if (depth == 0)
        return quiescence(alpha, beta, rep);
    
    // we are too deep, hence there's an overflow of arrays relying on max ply constant
    if (ply > MAX_PLY - 1)
        // evaluate position
        return evaluate_position(rep);
    
    nodes++;
    
    bool in_check = rep.is_square_attacked(BitBoardSquare(rep.side == white ? get_ls1b_index(rep.bitboards[BoardPiece::K]) :
                                                                              get_ls1b_index(rep.bitboards[BoardPiece::k])),
                                           TurnColor(rep.side ^ 1));
    if (in_check) depth++;
    
    int legal_moves = 0, moves_searched = 0;
    
    int static_eval = evaluate_position(rep);
    
    
    // evaluation pruning / static null move pruning
    if (depth < 3 && !pv_node && !in_check && std::abs(beta - 1) > -INFINITY_SCORE + 100) {
        int eval_margin = 120 * depth;
        
        if (static_eval - eval_margin >= beta)
            return static_eval - eval_margin;
    }
    
    
    // null move pruning
    if (allow_null &&
        depth >= 1 + R &&
        !in_check) {
        
        int reduction = R;
        if (depth > 6) reduction = R + 1;
        
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
        
        score = -negascout(-beta, -beta + 1, depth - 1 - reduction, rep, false); // do not allow consequtive null moves
        
        // restore null move
        rep.side = TurnColor(rep.side ^ 1);
        rep.enpassant = og_square;
        rep.hash_key = og_hash_key;
        
        ply--;
        repetition_index--;
        
        if (time_control.stopped) return 0;
        if (score >= beta) return score;
    }
    
    
    // razoring
    if (!pv_node && !in_check && depth <= 3) {
        score = static_eval + 125;
        
        int new_score;
        
        // fail low node
        if (score < beta) {
            if (depth == 1) {
                new_score = quiescence(alpha, beta, rep);
                return std::max(new_score, score);
            }
            
            score += 175;
            
            // fail low node
            if (score < beta && depth <= 3) {
                new_score = quiescence(alpha, beta, rep);
                
                if (new_score < beta)
                    return std::max(new_score, score);
            }
        }
    }
    
    
    Moves move_list = rep.generate_moves(false);
    if (follow_pv) enable_pv_sorting(move_list);
    sort_moves(move_list, best_move, rep);
    
    // PVS make first move
    int count = 0;
    for (; moves_searched == 0 && count < move_list.count; count++) {
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
        
        // normal search
        best_score = -negascout(-beta, -alpha, depth - 1, new_rep, true);
        
        ply--;
        repetition_index--;
        
        if (time_control.stopped) return 0;
        
        // better alpha
        if (best_score > alpha) {
            hash_flag = hash_flag_exact;
            
            // store best move so far
            best_move = move_list.moves[count];
            
            // story history moves
            if (!move_list.moves[count].get_move_capture()) history_moves[move_list.moves[count].get_move_piece()][move_list.moves[count].get_move_target()] += depth;
            
            // write PV move
            pv_table[ply][ply] = move_list.moves[count];
            
            // loop over the next ply
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            
            pv_length[ply] = pv_length[ply + 1];
            
            // beta cutoff
            if (best_score >= beta) {
                hash_table.write_hash_entry(best_score, best_move, depth, ply, hash_flag_beta, rep);
                
                // store killer moves on quite moves
                if (!move_list.moves[count].get_move_capture()) {
                    killer_moves[1][ply] = killer_moves[0][ply];
                    killer_moves[0][ply] = move_list.moves[count];
                }
                
                return best_score;
            }
            
            alpha = best_score;
        }
        
        moves_searched++;
    }
    
    // PVS make rest of the moves
    for (; count < move_list.count; count++) {
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
            if (score > alpha && score < beta) {
                score = -negascout(-beta, -alpha, depth - 1, new_rep, true);
                if (score > alpha) {
                    alpha = score;
                    
                    hash_flag = hash_flag_exact;
                }
            }
        }
        
        ply--;
        repetition_index--;
        
        if (time_control.stopped) return 0;
        
        // better prev best
        if (score > best_score) {
            
            // store best move so far
            best_move = move_list.moves[count];
            
            // story history moves
            if (!move_list.moves[count].get_move_capture()) {
                history_moves[move_list.moves[count].get_move_piece()][move_list.moves[count].get_move_target()] += depth;
            }
            
            // write PV move
            pv_table[ply][ply] = move_list.moves[count];
            
            // loop over the next ply
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            
            pv_length[ply] = pv_length[ply + 1];
            
            // beta cutoff
            if (score >= beta) {
                hash_table.write_hash_entry(score, best_move, depth, ply, hash_flag_beta, rep);
                
                // store killer moves on quite moves
                if (!move_list.moves[count].get_move_capture()) {
                    killer_moves[1][ply] = killer_moves[0][ply];
                    killer_moves[0][ply] = move_list.moves[count];
                }
                
                return score;
            }
            
            best_score = score;
        }
        
        moves_searched++;
    }
    
    if (!legal_moves) {
        // checkmate
        if (in_check)
            return -MATE_VALUE + ply;
        
        else // stalemate
            return 0;
    }
    
    hash_table.write_hash_entry(best_score, best_move, depth, hash_flag, ply, rep);
    
    // node (move) fails low
    return best_score;
}
