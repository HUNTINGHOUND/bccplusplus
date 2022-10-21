// system headers
#include <iostream>
#include <getopt.h>

// local headers
#include "board.hpp"
#include "attacktables.hpp"
#include "util.hpp"
#include "magic.hpp"
#include "move.hpp"
#include "uci.hpp"
#include "search.hpp"


long nodes = 0;

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
    
    Moves move_list = rep.generate_moves();
    
    for (int move_count = 0; move_count < move_list.count; move_count++) {
        int made_move;
        BoardRepresentation rep_copy = rep.copy_and_move(move_list.moves[move_count], all_moves, &made_move);
        
        if(!made_move) continue;
        
        perft_driver(depth - 1, rep_copy);
    }
}

void perft_test(int depth, BoardRepresentation const & rep) {
    std::cout << "\n     Performance test\n\n";
    
    Moves move_list = rep.generate_moves();
    
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
    
    if (options.init_magic) init_magic_numbers();
}

int main(int argc, char **argv){
    set_options(argc, argv);
    init_all();
    
    BoardRepresentation rep;
    if (options.debug) {
        auto start = get_time_point();
        
        rep.parse_fen(TRICKY_POSITION);
        rep.print_board();
        search_position(9, rep);
        std::cout << "Time: " << get_time_diff(start, get_time_point()) << " ms\n";
    } else
        uci_loop(rep);
    
    
    return 0;
}
