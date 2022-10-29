#ifndef search_hpp
#define search_hpp
// system headers
#include <array>

// local headers
#include "types.hpp"
#include "move.hpp"
#include "definitions.hpp"
#include "board.hpp"
#include "tt.hpp"

class Search {
    
    std::array<U64, 1000> & repetition_table;
    int repetition_index;
    
    TranspositionTable & hash_table;
    
public:
    
    Search(std::array<U64, 1000> & _repetition_table, int _repetition_index, TranspositionTable & _hash_table) : repetition_table(_repetition_table), repetition_index(_repetition_index), hash_table(_hash_table) {}
    
    const int full_depth_move = 4;
    const int reduction_limit = 3;
    const int R = 2;
    
    U64 nodes = 0;
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
    
    bool is_repetition(BoardRepresentation const & rep);
    
    void enable_pv_sorting(Moves const & move_list);
    
    /*  =======================
             Move ordering
        =======================
        
        1. PV move
        2. Captures in MVV/LVA
        3. 1st killer move
        4. 2nd killer move
        5. History moves
        6. Unsorted moves
    */
    
    int score_move(Move const & move, BoardRepresentation const & rep);
    
    void print_move_scores(Moves const & move_list, BoardRepresentation const & rep);
    
    int sort_moves(Moves & move_list, Move best_move, BoardRepresentation const & rep);
    
    int quiescence(int alpha, int beta, BoardRepresentation const & rep);
    
    bool ok_to_reduce(Move const & move, bool in_check);
    
    // PVS search, rep should not be changed, if rep is change, it should be restored to original form. 
    int negascout(int alpha, int beta, int depth, BoardRepresentation & rep, bool allow_null);
    
};
#endif /* search_hpp */
