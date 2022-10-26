#ifndef tt_hpp
#define tt_hpp

#include "parallel_hashmap/phmap.h"
#include "board.hpp"


using U64 = unsigned long long;

const int hash_flag_exact = 0;
const int hash_flag_alpha = 1;
const int hash_flag_beta = 2;

const int no_hash_entry = 100000;

/* These are the score bounds for the range of the mating scores
   [-infinity, -mate_value ... -mate_score, ... score ... mate_score ... mate_value, infinity]
*/
#define INFINITY 50000
#define MATE_VALUE 49000
#define MATE_SCORE 48000


struct TT {
    int depth=0; // current search depth
    int flag=0;  // flag the type of node
    int score=0; // score (alpha / beta / PV)
};

extern phmap::parallel_flat_hash_map<U64, TT> hash_table;

inline void clear_hash_table() {
    hash_table.clear();
}

inline int read_hash_entry(int alpha, int beta, int depth, int ply, BoardRepresentation const & rep) {
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

inline void write_hash_entry(int score, int depth, int hash_flag, int ply, BoardRepresentation const & rep) {
    if (score < -MATE_SCORE) score -= ply;
    if (score > MATE_SCORE) score += ply;
    
    hash_table[rep.hash_key] = {depth, hash_flag, score};
}

#endif /* tt_hpp */
