#ifndef definitions_hpp
#define definitions_hpp

// FEN dedug positions
#define EMPTY_POSITION "8/8/8/8/8/8/8/8 w - - "
#define START_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define TRICKY_POSITION "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define KILLER_POSITION "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define CMK_POSITION "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "
#define REPETITIONS "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40 "

#define MAX_PLY 64
#define WINDOW_VAL 50

/* These are the score bounds for the range of the mating scores
   [-infinity, -mate_value ... -mate_score, ... score ... mate_score ... mate_value, infinity]
*/
#define INFINITY_SCORE 50000
#define MATE_VALUE 49000
#define MATE_SCORE 48000

#define DEFAULT_TT_MB 64
#define DEFAULT_EVAL_HASH_MB 64
#define MAX_HASH 128

#endif /* definitions_hpp */
