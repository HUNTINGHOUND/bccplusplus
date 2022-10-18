#ifndef pieces_hpp
#define pieces_hpp

namespace BoardPiece {

const int material_score[12] = {
    100,
    300,
    350,
    500,
    1000,
    10000,
    -100,
    -300,
    -350,
    -500,
    -1000,
    -10000
};

enum RorB {rook, bishop};

// encode pieces
enum Pieces { P, N, B, R, Q, K, p, n, b, r, q, k };

const char ascii_pieces[12] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};
const char* const unicode_pieces[12] = {"♙", "♘", "♗", "♖", "♕", "♔", "♟︎", "♞", "♝", "♜", "♛", "♚"};

// convert ASCII character pieces to encoded constants
const Pieces char_pieces[] = {
    ['P'] = P,
    ['N'] = N,
    ['B'] = B,
    ['R'] = R,
    ['Q'] = Q,
    ['K'] = K,
    ['p'] = p,
    ['n'] = n,
    ['b'] = b,
    ['r'] = r,
    ['q'] = q,
    ['k'] = k
};
}

const char promoted_pieces[] = {
    [0] = ' ',
    [BoardPiece::Q] = 'q',
    [BoardPiece::R] = 'r',
    [BoardPiece::B] = 'b',
    [BoardPiece::N] = 'n',
    [BoardPiece::q] = 'q',
    [BoardPiece::r] = 'r',
    [BoardPiece::b] = 'b',
    [BoardPiece::n] = 'n',
};

#endif /* pieces_hpp */
