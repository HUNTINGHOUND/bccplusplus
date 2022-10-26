#ifndef uci_hpp
#define uci_hpp
// system headers
#include <string>
#include <chrono>

// local headers
#include "move.hpp"
#include "board.hpp"
#include "pieces.hpp"

// FEN dedug positions
#define EMPTY_POSITION "8/8/8/8/8/8/8/8 w - - "
#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define TRICKY_POSITION "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define KILLER_POSITION "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define CMK_POSITION "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "
#define REPETITIONS "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40 "

// parse user/GUI move string input (e.g. e7e8q)
Move parse_move(std::string const & move_string, BoardRepresentation & rep);

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

struct TimeControl{
    // exit from engine flag
    bool quit;
    
    // UCI "movestogo" command moves counter
    int movestogo = 30;
    
    // UCI "movetime" commend time counter
    int movetime = -1;
    
    // UCI "time" command holder (ms)
    int time = -1;
    
    // UCI "inc" command's time increment holder
    int inc = 0;
    
    // UCI "starttime" command time holder
    std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
    
    // UCI "stoptime" command time holder
    std::chrono::time_point<std::chrono::system_clock> stop_time = std::chrono::system_clock::now();
    
    // variable to flag time control availability
    bool timeset = 0;
    
    // variable to flag when the time is up
    bool stopped = 0;
    
};

extern TimeControl time_control;


/*
 Function listen to GUI's input during search.
 Its waiting for the user in[put from std::cin.
 */
int input_waiting();

// Read GUI/user input
void read_input();

// a bridge function to interact between search and GUI input
void communicate();
#endif /* uci_hpp */
