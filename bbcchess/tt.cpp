// system headers
#include <iostream>

// local headers
#include "tt.hpp"
#include "definitions.hpp"

TranspositionTable::TranspositionTable(int mb) {
    int hash_size = 0x100000 * mb;

    hash_entries = hash_size / sizeof(TT);
    
    hash_table.reserve(hash_entries);
    
    std::cout << "Transposition table is initialized with " << hash_entries << " entires\n";
}

void TranspositionTable::clear_hash_table() {
    hash_table.clear();
}

int TranspositionTable::read_hash_entry(int alpha, int beta, Move* best_move, int depth, int ply, BoardRepresentation const & rep) {
    size_t hash_pos = rep.hash_key % hash_entries;
    if (!hash_table.count(hash_pos)) return no_hash_entry;
    
    TT hash_entry = hash_table[hash_pos];
    
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
    
    *best_move = hash_entry.best_move;
    
    return no_hash_entry;
}

void TranspositionTable::write_hash_entry(int score, Move best_move, int depth, int hash_flag, int ply, BoardRepresentation const & rep) {
    if (score < -MATE_SCORE) score -= ply;
    if (score > MATE_SCORE) score += ply;
    
    hash_table[rep.hash_key % hash_entries] = {depth, hash_flag, score, best_move};
}

EvaluationTable::EvaluationTable(int mb) {
    int hash_size = 0x100000 * mb;

    hash_entries = hash_size / sizeof(int);
    
    hash_table.reserve(hash_entries);
    
    std::cout << "Evaluation hash table is initialized with " << hash_entries << " entires\n";
}


void EvaluationTable::clear_hash_table() {
    hash_table.clear();
}

int EvaluationTable::read_hash_entry(BoardRepresentation const & rep) {
    size_t hash_pos = rep.hash_key % hash_entries;
    if (!hash_table.count(hash_pos)) return no_hash_entry;
    return hash_table[hash_pos];
}

void EvaluationTable::write_hash_entry(int score, BoardRepresentation const & rep) {
    hash_table[rep.hash_key % hash_entries] = score;
}
