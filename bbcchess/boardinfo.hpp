#ifndef boardinfo_hpp
#define boardinfo_hpp

enum TurnColor {white, black, both};


/*
 white king side castle  = 0001
 white queen side castle = 0010
 black king side castle  = 0100
 black queen side castle = 1000
 */
enum CastleRight {wk = 1, wq = 2, bk = 4, bq = 8};

/*
                            in          in
                            binary      decimal
 king & rook didn't move:   1111        15
 
        white king moved:   1100        12
   white king rook moved:   1110        14
  white queen rook moved:   1101        13
 
        black king moved:   0011        3
   black king rook moved:   1011        11
  black queen rook moved:   0111        7
 */

const int castling_rights[64] = {
    7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};

enum GamePhase {opening, endgame, middlegame};


#endif /* boardinfo_hpp */
