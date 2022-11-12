// system headers
#include <iostream>
#include <getopt.h>

// local headers
#include "types.hpp"
#include "board.hpp"
#include "time_control.hpp"
#include "attacktables.hpp"
#include "pieces.hpp"
#include "evaluation.hpp"
#include "zorbist.hpp"
#include "definitions.hpp"
#include "uci.hpp"
#include "magic.hpp"
#include "solver.hpp"
#include "nnue_eval.hpp"

U64 nodes = 0;

/*
 ==================================
 
               Perft
 
 ==================================
 */

void perft_driver(int depth, BoardRepresentation const & rep) {
    if (depth == 0) {
        nodes++;
        return;
    }
    
    Moves move_list = rep.generate_moves(false);
    
    for (int move_count = 0; move_count < move_list.count; move_count++) {
        int made_move;
        BoardRepresentation rep_copy = rep.copy_and_move(move_list.moves[move_count], all_moves, &made_move);
        
        if(!made_move) continue;
        
        perft_driver(depth - 1, rep_copy);
    }
}

void perft_test(int depth, BoardRepresentation const & rep) {
    std::cout << "\n     Performance test\n\n";
    
    Moves move_list = rep.generate_moves(false);
    
    auto start = get_time_point();
    
    for (int move_count = 0; move_count < move_list.count; move_count++) {
        int move_made;
        BoardRepresentation rep_copy = rep.copy_and_move(move_list.moves[move_count], all_moves, &move_made);
        
        if(!move_made) continue;
        
        long cummulative_modes = nodes;
        
        perft_driver(depth - 1, rep_copy);
        
        long old_nodes = nodes - cummulative_modes;
        
        std::cout << "     move: ";
        move_list.moves[move_count].print_move_nonewline();
        std::cout << "  nodes: " << old_nodes << "\n";
    }
    
    std::cout << "\n    Depth: " << depth << "\n";
    std::cout << "    Nodes: " << nodes << "\n";
    std::cout << "     Time: " << get_time_diff(start, get_time_point()) << " ms\n\n";
}


/*
 ==================================
 
             Main driver
 
 ==================================
*/

struct {
    int init_magic = 0;
    int debug = 0;
} options;

option long_options[] = {
    {"init_magic", no_argument, &options.init_magic, 1},
    {"debug", no_argument, &options.debug, 1}
};

void set_options(int argc, char **argv) {
    int opt;
    while ((opt = getopt_long(argc, argv, "md", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'm':
                options.init_magic = 1;
                break;
                
            case 'd':
                options.debug = 1;
                break;
                
            default:
                break;
        }
    }
}

void init_all() {
    AttackTables::init_leapers_attacks();
    
    AttackTables::init_sliders_attacks(BoardPiece::bishop);
    AttackTables::init_sliders_attacks(BoardPiece::rook);
    
    Evaluation::init_evaluation_masks();
    
    if (options.init_magic) init_magic_numbers();
    Zorbist::init_random_keys();
    
    init_nnue("/Users/morgan/Desktop/bbcchess/bbcchess/nnue/nn-9931db908a9b.nnue");
}

int main(int argc, char **argv){
    set_options(argc, argv);
    init_all();
    
    Solver solver(DEFAULT_TT_MB, DEFAULT_EVAL_HASH_MB);
    if (options.debug) {
        auto start = get_time_point();
        
        solver.parse_fen(TRICKY_POSITION);
        solver.rep.print_board();
        
        solver.search_position(4);
        
        std::cout << "Evaluation: " << solver.rep.evaluate() << "\n";
        std::cout << "Time: " << get_time_diff(start, get_time_point()) << " ms\n";
    } else
        uci_loop(solver);
    
    
    return 0;
}
