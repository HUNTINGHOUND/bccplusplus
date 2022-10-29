#ifndef solver_hpp
#define solver_hpp

// system headers
#include <array>

// local headers
#include "types.hpp"
#include "board.hpp"
#include "tt.hpp"

class Solver {
public:
    // positions repetition table
    std::array<U64, 1000> repetition_table;
    
    TranspositionTable hash_table;
    EvaluationTable eval_table;

    // repetition index
    int repetition_index;
    
    BoardRepresentation rep;

    void search_position(int depth);
    
    void parse_fen(std::string const & fen, size_t fen_idx=0);
};

#endif /* solver_hpp */
