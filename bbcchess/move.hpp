#ifndef move_hpp
#define move_hpp

class Move {
public:
    
    static const int SOURCE_SQUARE_CONSTANT    = 0x3f;
    static const int TARGET_SQUARE_CONSTANT    = 0xfc0;
    static const int PIECE_CONSTANT            = 0xf000;
    static const int PROMOTED_PIECE_CONSTANT   = 0xf0000;
    static const int CAPTURE_FLAG_CONSTANT     = 0x100000;
    static const int DOUBLE_PUSH_FLAG_CONSTANT = 0x200000;
    static const int ENPASSANT_FLAG_CONSTANT   = 0x400000;
    static const int CASTLING_FLAG_CONSTANT    = 0x800000;
    
    int move;
     
    Move() : move(0) {};
    Move(int move) : move(move) {};
    
    operator int() const;
    
    int get_move_source() const;
    
    int get_move_target() const;
    
    int get_move_piece() const;

    int get_move_promoted() const;
    
    bool get_move_capture() const;
    
    bool get_move_double() const;
    
    bool get_move_enpassant() const;
    
    bool get_move_castling() const;
    
    void print_move() const;
    
    void print_move_nonewline() const;
    
    void encode(int source, int target, int piece, int promoted, bool capture, bool double_push, bool enpassant, bool castling);
    
    Move(int source, int target, int piece, int promoted, bool capture, bool double_push, bool enpassant, bool castling);
};

class Moves {
public:
    Move moves[256];
    int count = 0;
    
    bool has_promotion = false;
    
    void add_move(Move move);

    void print_move_list() const;
};

enum MoveFlag { all_moves, only_captures };

#endif /* move_hpp */
