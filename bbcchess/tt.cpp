// local headers
#include "tt.hpp"
#include "definitions.hpp"

void TranspositionTable::clear_hash_table() {
    hash_table.clear();
}

int TranspositionTable::read_hash_entry(int alpha, int beta, int depth, int ply, BoardRepresentation const & rep) {
    if (!hash_table.count(rep.hash_key)) return no_hash_entry;
    
    TT hash_entry = hash_table[rep.hash_key];
    
    if (hash_entry.depth >= depth) {
        int score = hash_entry.score;
        
        // retrieve score independent from the actual path
        // from root node (position) to current node (position)
        
        if (score < -MATE_SCORE) score += ply;
        if (score > MATE_SCORE) score -= ply;
        
        if (hash_entry.flag == hash_flag_exact)
            return score;
       
        if ((hash_entry.flag == hash_flag_alpha) && (score <= alpha))
            return alpha;
        
        if ((hash_entry.flag == hash_flag_beta) && (score >= beta))
            return beta;
    }
    
    return no_hash_entry;
}

void TranspositionTable::write_hash_entry(int score, int depth, int hash_flag, int ply, BoardRepresentation const & rep) {
    if (score < -MATE_SCORE) score -= ply;
    if (score > MATE_SCORE) score += ply;
    
    hash_table[rep.hash_key] = {depth, hash_flag, score};
}
