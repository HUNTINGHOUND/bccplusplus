#ifndef uci_hpp
#define uci_hpp
// system headers
#include <string>

// local headers
#include "board.hpp"

// parse user/GUI move string input (e.g. e7e8q)
Move parse_move(std::string const & move_string, BoardRepresentation const & rep, size_t move_idx = 0);

/*
 Example UCI commands to init position on chess board
 
 init start postions:
 position startpos
 
 init start position and make the moves on chess board:
 position startpos moves e2e4 e7e5
 
 init position from FEN string:
 position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
 
 init position from FEN string and make moves on chess board:
 position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 moves e2a6 e8g8
 */
void parse_position(std::string const & command, BoardRepresentation & rep);

/*
 Example UCI command to make engine search for the best move
 
 fixed depth search:
 go depth 64
 */
void parse_go(std::string const & command, BoardRepresentation & rep);

void uci_loop(BoardRepresentation & rep);
#endif /* uci_hpp */
