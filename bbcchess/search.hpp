#ifndef search_hpp
#define search_hpp

// local headers
#include "board.hpp"
#include "move.hpp"

class Search {
public:
    
    int nodes = 0;
    int ply = 0;
    Move best_move;
    
    int quiescence(int alpha, int beta, BoardRepresentation const & rep) {
        // evaluate position
        int evaluation = rep.evalutate();
        
        // beta cut off
        if (evaluation >= beta)
            return beta;
        
        if (evaluation > alpha)
            alpha = evaluation;
        
        Moves move_list = rep.generate_moves();
        
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
        if (depth == 0)
            return quiescence(alpha, beta, rep);
        
        nodes++;
        
        bool in_check = rep.is_square_attacked(BitBoardSquare(rep.side == white ? get_ls1b_index(rep.bitboards[BoardPiece::K]) :
                                                                                               get_ls1b_index(rep.bitboards[BoardPiece::k])),
                                               TurnColor(rep.side ^ 1));
        int legal_moves = 0;
        
        Move best_sofar;
        int old_alpha = alpha;
        
        Moves move_list = rep.generate_moves();
        
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
            if (score >= beta) return beta;
            
            // better alpha
            if (score > alpha) {
                alpha = score;
                
                // record best move for root node
                if (ply == 0)
                    best_sofar = move_list.moves[count];
            }
        }
        
        if (!legal_moves) {
            if (in_check)
                return -49000 + ply;
            
            else
                return 0;
        }
        
        if (old_alpha != alpha)
            best_move = best_sofar;
        
        // node (move) fails low
        return alpha;
    }
    
};

inline void search_position(int depth, BoardRepresentation const & rep) {
    Search search;
    int score = search.negamax(-50000, 50000, depth, rep);
    
    if (search.best_move) {
        std::cout << "info score cp " << score << " depth " << depth << " node " << search.nodes << "\n";
        
        std::cout << "bestmove ";
        search.best_move.print_move();
        std::cout << "\n";
    }
}


#endif /* search_hpp */
