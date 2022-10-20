#ifndef search_hpp
#define search_hpp
// system headers
#include <numeric>
#include <array>

// local headers
#include "board.hpp"
#include "move.hpp"

#define MAX_PLY 64

class Search {
public:
    
    int nodes = 0;
    int ply = 0;
    Move best_move;
    
    bool follow_pv = false;
    
    // killer moves [id][ply]
    std::array<std::array<int, MAX_PLY>, 2> killer_moves = {};
    
    // history moves [piece][square]
    std::array<std::array<int, 64>, 12> history_moves = {};
    
    /*
          ================================
                Triangular PV table
          --------------------------------
            PV line: e2e4 e7e5 g1f3 b8c6
          ================================
               0    1    2    3    4    5
          
          0    m1   m2   m3   m4   m5   m6
          
          1    0    m2   m3   m4   m5   m6
          
          2    0    0    m3   m4   m5   m6
          
          3    0    0    0    m4   m5   m6
           
          4    0    0    0    0    m5   m6
          
          5    0    0    0    0    0    m6
    */
    std::array<int, MAX_PLY> pv_length = {};
    std::array<std::array<Move, MAX_PLY>, MAX_PLY> pv_table = {};
    
    void enable_pv_sorting(Moves const & move_list) {
        follow_pv = false;
        
        for (int count = 0; count < move_list.count; count++) {
            if (pv_table[0][ply] == move_list.moves[count]) {
                follow_pv = true;
                break;
            }
        }
    }
    
    int score_move(Move const & move, BoardRepresentation const & rep) {
        if (follow_pv && pv_table[0][ply] == move) {
            std::cout << "current PV move: ";
            move.print_move_nonewline();
            std::cout << " ply: " << ply << "\n";
            
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
                if (rep.bitboards[bb_piece].get_bit(move.get_move_target())) {
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
    
    void print_move_scores(Moves const & move_list, BoardRepresentation const & rep) {
        std::cout << "     Move scores:\n\n";
        
        for (int count = 0; count < move_list.count; count++) {
            std::cout << "     move: ";
            move_list.moves[count].print_move_nonewline();
            std::cout << " score: " << score_move(move_list.moves[count], rep) << "\n";
        }
    }
    
    int sort_moves(Moves & move_list, BoardRepresentation const & rep) {
        // move scores
        int move_scores[256];
        
        for (int count = 0; count < move_list.count; count++)
            move_scores[count] = score_move(move_list.moves[count], rep);
        
        
        // loop over current move within a move list
        for (int current_move = 0; current_move < move_list.count; current_move++)
        {
            // loop over next move within a move list
            for (int next_move = current_move + 1; next_move < move_list.count; next_move++)
            {
                // compare current and next move scores
                if (move_scores[current_move] < move_scores[next_move])
                {
                    // swap scores
                    int temp_score = move_scores[current_move];
                    move_scores[current_move] = move_scores[next_move];
                    move_scores[next_move] = temp_score;
                    
                    // swap moves
                    int temp_move = move_list.moves[current_move];
                    move_list.moves[current_move] = move_list.moves[next_move];
                    move_list.moves[next_move] = temp_move;
                }
            }
        }
        
        /*
        int move_idx[256];
        std::iota(move_idx, move_idx + move_list.count, 0);
        std::stable_sort(move_idx, move_idx + move_list.count, [&move_scores](int a, int b) {
            return move_scores[a] > move_scores[b];
        });
        
        Move move_copy[256];
        std::copy(move_list.moves, move_list.moves + move_list.count, move_copy);
        
        for(int i = 0; i < move_list.count; i++) move_list.moves[i] = move_copy[move_idx[i]];
        */
        
        return 0;
    }
    
    int quiescence(int alpha, int beta, BoardRepresentation const & rep) {
        nodes++;
        
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
            
            int move_made;
            BoardRepresentation new_rep = rep.copy_and_move(move_list.moves[count], only_captures, &move_made);
            
            if (!move_made) {
                ply--;
                continue;
            }
            
            int score = -quiescence(-beta, -alpha, new_rep);
            
            ply--;
            
            // beta cut off
            if (score >= beta)
                return beta;
            
            if (score > alpha)
                alpha = score;
        }
        
        return alpha;
    }
    
    int negamax(int alpha, int beta, int depth, BoardRepresentation const & rep) {
        pv_length[ply] = ply;
        
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
        
        int legal_moves = 0;
        
        Moves move_list = rep.generate_moves();
        if (follow_pv) enable_pv_sorting(move_list);
        sort_moves(move_list, rep);
        
        for (int count = 0; count < move_list.count; count++) {
            ply++;
            
            int move_made;
            BoardRepresentation new_rep = rep.copy_and_move(move_list.moves[count], all_moves, &move_made);
            
            if (!move_made) {
                ply--;
                continue;
            }
            
            legal_moves++;
            
            int score = -negamax(-beta, -alpha, depth - 1, new_rep);
            
            ply--;
            
            
            // beta cutoff
            if (score >= beta) {
                // store killer moves
                if (!move_list.moves[count].get_move_capture()) {
                    killer_moves[1][ply] = killer_moves[0][ply];
                    killer_moves[0][ply] = move_list.moves[count];
                }
                
                return beta;
            }
            
            // better alpha
            if (score > alpha) {
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
            }
        }
        
        if (!legal_moves) {
            // checkmate
            if (in_check)
                return -49000 + ply;
            
            else // stalemate
                return 0;
        }
        
        // node (move) fails low
        return alpha;
    }
    
};

inline void search_position(int depth, BoardRepresentation const & rep) {
    Search search;
    
    int score = 0;
    
    // iterative deepening
    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        search.nodes = 0;
        search.follow_pv = true;
        
        score = search.negamax(-50000, 50000, current_depth, rep);
        
        std::cout << "info score cp " << score << " depth " << current_depth << " nodes " << search.nodes << " pv ";
        // loop over the moves within a PV line
        for (int count = 0; count < search.pv_length[0]; count++) {
            search.pv_table[0][count].print_move_nonewline();
            std::cout << " ";
        }
        
        std::cout << "\n";
    }
    
    
    std::cout << "bestmove ";
    search.pv_table[0][0].print_move_nonewline();
    std::cout << "\n";
}


#endif /* search_hpp */
