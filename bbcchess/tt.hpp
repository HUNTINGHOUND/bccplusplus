#ifndef tt_hpp
#define tt_hpp

// local headers
#include "parallel_hashmap/phmap.h"
#include "types.hpp"
#include "board.hpp"
#include "move.hpp"

const int hash_flag_exact = 0;
const int hash_flag_alpha = 1;
const int hash_flag_beta = 2;

const int no_hash_entry = 100000;


struct TT {
    int depth=0; // current search depth
    int flag=0;  // flag the type of node
    int score=0; // score (alpha / beta / PV)
    Move best_move;
};

class TranspositionTable {
    phmap::parallel_flat_hash_map<U64, TT> hash_table;
    
public:
    void clear_hash_table();

    int read_hash_entry(int alpha, int beta, Move* best_move, int depth, int ply, BoardRepresentation const & rep);

    void write_hash_entry(int score, Move best_move, int depth, int hash_flag, int ply, BoardRepresentation const & rep);
};
#endif /* tt_hpp */
