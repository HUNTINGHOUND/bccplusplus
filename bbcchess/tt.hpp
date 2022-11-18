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

template <class KEYTYPE, class STORETYPE>
class HashTable {
public:
    phmap::parallel_flat_hash_map<KEYTYPE, STORETYPE> hash_table;
    size_t hash_entries;
    
    HashTable(int mb) {
        int hash_size = 0x100000 * mb;
        hash_entries = hash_size / sizeof(STORETYPE);
        hash_table.reserve(hash_entries);
    }
    
    virtual void clear_hash_table() {
        hash_table.clear();
    }
};

class TranspositionTable : private HashTable<U64, TT> {
public:
    TranspositionTable(int mb);

    int read_hash_entry(int alpha, int beta, Move* best_move, int depth, int ply, BoardRepresentation const & rep);

    void write_hash_entry(int score, Move best_move, int depth, int hash_flag, int ply, BoardRepresentation const & rep);
    
    using HashTable<U64, TT>::clear_hash_table;
};

class EvaluationTable : private HashTable<U64, int> {
public:
    EvaluationTable(int mb);

    int read_hash_entry(BoardRepresentation const & rep);

    void write_hash_entry(int score, BoardRepresentation const & rep);
    
    using HashTable<U64, int>::clear_hash_table;
};
#endif /* tt_hpp */
