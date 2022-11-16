#ifndef pieces_hpp
#define pieces_hpp

// #include "boardinfo.hpp"

namespace BoardPiece {

enum RorB {rook, bishop};

// encode pieces
enum Pieces { P, N, B, R, Q, K, p, n, b, r, q, k };

enum PieceType {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};

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

const int piece_to_index[] = {
    [BoardPiece::P] = 0,
    [BoardPiece::N] = 1,
    [BoardPiece::B] = 2,
    [BoardPiece::R] = 3,
    [BoardPiece::Q] = 4,
    [BoardPiece::K] = 5,
    [BoardPiece::p] = 0,
    [BoardPiece::n] = 1,
    [BoardPiece::b] = 2,
    [BoardPiece::r] = 3,
    [BoardPiece::q] = 4,
    [BoardPiece::k] = 5,
};

#endif /* pieces_hpp */
