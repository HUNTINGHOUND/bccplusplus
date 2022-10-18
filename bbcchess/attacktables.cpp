// local headers
#include "attacktables.hpp"
#include "util.hpp"

// extern definition
U64 AttackTables::Pawn::pawn_attacks[2][64];
U64 AttackTables::Knight::knight_attacks[64];
U64 AttackTables::King::king_attacks[64];
U64 AttackTables::Rook::rook_magic_numbers[64] = {
    0x200108021020040ULL,
    0x1100110080400020ULL,
    0x80200080100008ULL,
    0x100100009000420ULL,
    0x200081002002004ULL,
    0x900040008820100ULL,
    0x4200011824008200ULL,
    0x1180004063000080ULL,
    0x10802080004002ULL,
    0x8100c00150002000ULL,
    0xa100802000100080ULL,
    0x28808010000800ULL,
    0x40800400080080ULL,
    0xc4011004002008ULL,
    0x2003000200040300ULL,
    0x2082000401008062ULL,
    0x9280004020004000ULL,
    0x42040c010002000ULL,
    0x10e410010200102ULL,
    0x828008010000881ULL,
    0x4808004000800ULL,
    0x200a008004008002ULL,
    0x80000400020110c8ULL,
    0x100200004411a3ULL,
    0x8080004440002004ULL,
    0xa000200080400082ULL,
    0x5840100080200080ULL,
    0x100080080081ULL,
    0x22004600282010ULL,
    0x440040801201040ULL,
    0x4900400010248ULL,
    0x70410600004084ULL,
    0x1414400020800480ULL,
    0x90002000404001ULL,
    0x100080802000ULL,
    0x6000800800801000ULL,
    0x501c041101000800ULL,
    0x480040080800200ULL,
    0x100104004288ULL,
    0x208004092000924ULL,
    0x8090800c41010020ULL,
    0x3100020004c4000ULL,
    0x1020408022020012ULL,
    0x70100008008080ULL,
    0xc080004008080ULL,
    0x8040040002008080ULL,
    0x1000040200010100ULL,
    0x544004100a20004ULL,
    0x210290080084100ULL,
    0x1804000200280ULL,
    0x200801000a00280ULL,
    0x200100009002100ULL,
    0x80024008280ULL,
    0x142200440900801ULL,
    0x3902000824016200ULL,
    0x50044081040e00ULL,
    0x1001824800041ULL,
    0x228e002080421102ULL,
    0x80102008804202ULL,
    0x1000820100055ULL,
    0x1000210480005ULL,
    0x2002004401108802ULL,
    0x8a8009001080204ULL,
    0x1201000124520081ULL
};
U64 AttackTables::Bishop::bishop_magic_numbers[64] = {
    0x20024204440281ULL,
    0x21a404008c110203ULL,
    0x2010040040440058ULL,
    0x808060042302088ULL,
    0x110400020440cULL,
    0x804922020004000ULL,
    0x8040840108400210ULL,
    0x8002020082011080ULL,
    0x100020a084410040ULL,
    0xa08100102041040ULL,
    0x4822101210a0400ULL,
    0x1040420804000ULL,
    0xc0000c5040200010ULL,
    0x86b0008210400110ULL,
    0x1a00905c10045000ULL,
    0x4900020242280d00ULL,
    0x404801060021c08ULL,
    0x110000210822480ULL,
    0xa040800101010100ULL,
    0x404001240148016ULL,
    0x8101101401400ULL,
    0x208048c8044013ULL,
    0x1004003120821008ULL,
    0x1010480104048484ULL,
    0x50401012040100ULL,
    0x108040002240840ULL,
    0x404020010002040ULL,
    0x84080004010410ULL,
    0x204080404010400ULL,
    0x8828308080400ULL,
    0x8048801240144ULL,
    0x5002020442210120ULL,
    0x208a00800041881ULL,
    0x18013000043400ULL,
    0x8042003002020086ULL,
    0x820400808a08200ULL,
    0x4200204a0080ULL,
    0x2a420200004800ULL,
    0x8204010202d408a0ULL,
    0x808821080021188ULL,
    0x110a2a7040102420ULL,
    0x5210520301040ULL,
    0x12010401000200ULL,
    0x1024012009040ULL,
    0x400420202020410ULL,
    0x2090200088200500ULL,
    0x10410840911100ULL,
    0x4090340468800b43ULL,
    0x1040121480000ULL,
    0x4004221802080202ULL,
    0x8012104404640100ULL,
    0x524000020a020001ULL,
    0x3200008210101c0ULL,
    0x112001010020ULL,
    0x8420a809210000ULL,
    0x802001a901010000ULL,
    0x2080402090082005ULL,
    0x4820101015000ULL,
    0x1890004024041200ULL,
    0x1010800000208800ULL,
    0x4000000020204110ULL,
    0x602002020216ULL,
    0x408a0200210812cULL,
    0x4542200840810440ULL
};
U64 AttackTables::Bishop::bishop_masks[64];
U64 AttackTables::Rook::rook_masks[64];
U64 AttackTables::Bishop::bishop_attacks[64][512];
U64 AttackTables::Rook::rook_attacks[64][4096];

void AttackTables::init_leapers_attacks() {
    // loop over 64 board squares
    for (int square = a8; square <= h1; square++) {
        // init pawn attacks
        Pawn::pawn_attacks[white][square] = Pawn::mask_pawn_attacks(white, static_cast<BitBoardSquare>(square));
        Pawn::pawn_attacks[black][square] = Pawn::mask_pawn_attacks(black, static_cast<BitBoardSquare>(square));
        
        // init knight attacks
        Knight::knight_attacks[square] = Knight::mask_knight_attacks(static_cast<BitBoardSquare>(square));
        
        // init king attacks
        King::king_attacks[square] = King::mask_king_attacks(static_cast<BitBoardSquare>(square));
    }
}

void AttackTables::init_sliders_attacks(BoardPiece::RorB rorb) {
    // loop over 64 board sqaures
    for (int square = a8; square <= h1; square++) {
        // init bishop & rook masks
        Bishop::bishop_masks[square] = Bishop::mask_bishop_attacks(static_cast<BitBoardSquare>(square));
        Rook::rook_masks[square] = Rook::mask_rook_attacks(static_cast<BitBoardSquare>(square));
        
        // init current mask
        U64 attack_mask = rorb == BoardPiece::bishop ? Bishop::bishop_masks[square] : Rook::rook_masks[square];
        
        // init relevant occupany bit cout
        int relevant_bits_count = count_bits(attack_mask);
        
        // init occupancy indicies
        int occupancy_indicides (1 << relevant_bits_count);
        
        // loop over occupancy indicies
        for (int index = 0; index < occupancy_indicides; index++) {
            // bishop
            if (rorb == BoardPiece::bishop) {
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
                
                // init magic index
                int magic_index = (occupancy * Bishop::bishop_magic_numbers[square]) >> (64 - Bishop::bishop_relevant_bits[square]);
                
                Bishop::bishop_attacks[square][magic_index] = Bishop::bishop_attacks_on_the_fly(static_cast<BitBoardSquare>(square), occupancy);
            } else { // rook
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
                
                // init magic index
                int magic_index = (occupancy * Rook::rook_magic_numbers[square]) >> (64 - Rook::rook_relevant_bits[square]);
                
                // init bishop attacks
                Rook::rook_attacks[square][magic_index] = Rook::rook_attacks_on_the_fly(static_cast<BitBoardSquare>(square), occupancy);
            }
        }
    }
}

U64 AttackTables::set_occupancy(int index, int bits_in_mask, BitBoard attack_mask) {
    // occupancy map
    U64 occupancy = 0ULL;
    
    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++) {
        // get LS1B index of attacks mask
        int square = get_ls1b_index(attack_mask);
        
        // pop LS1B in attack map
        attack_mask.pop_bit(square);
        
        // make sure occupany is on board
        if (index & (1 << count))
            // populate occupancy map
            occupancy |= (1ULL << square);
    }
    
    return occupancy;
}


U64 AttackTables::Pawn::mask_pawn_attacks(TurnColor side, BitBoardSquare square) {
    // result attacks bitboard
    U64 attacks = 0ULL;
    
    // pice bitboard
    BitBoard bitboard(0ULL);
    
    // set piece on board
    bitboard.set_bit(square);
    
    // white pawn
    if (side == white) {
        if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
    } else { // black pawns
        if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
    }
    
    return attacks;
}

U64 AttackTables::Knight::mask_knight_attacks(BitBoardSquare square) {
    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard
    BitBoard bitboard(0ULL);
    
    // set piece on board
    bitboard.set_bit(square);
    
    // generate knight attacks
    if ((bitboard >> 17) & not_h_file) attacks |= (bitboard >> 17);
    if ((bitboard >> 15) & not_a_file) attacks |= (bitboard >> 15);
    if ((bitboard >> 10) & not_hg_file) attacks |= (bitboard >> 10);
    if ((bitboard >> 6) & not_ab_file) attacks |= (bitboard >> 6);
    if ((bitboard << 17) & not_a_file) attacks |= (bitboard << 17);
    if ((bitboard << 15) & not_h_file) attacks |= (bitboard << 15);
    if ((bitboard << 10) & not_ab_file) attacks |= (bitboard << 10);
    if ((bitboard << 6) & not_hg_file) attacks |= (bitboard << 6);

    // return attack map
    return attacks;
}

U64 AttackTables::King::mask_king_attacks(BitBoardSquare square) {
    // result attacks bitboard
    U64 attacks = 0ULL;

    // piece bitboard
    BitBoard bitboard(0ULL);
    
    // set piece on board
    bitboard.set_bit(square);
    
    // generate king attacks
    if (bitboard >> 8) attacks |= (bitboard >> 8);
    if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
    if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
    if ((bitboard >> 1) & not_h_file) attacks |= (bitboard >> 1);
    if (bitboard << 8) attacks |= (bitboard << 8);
    if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
    if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
    if ((bitboard << 1) & not_a_file) attacks |= (bitboard << 1);
    
    // return attack map
    return attacks;
}

U64 AttackTables::Bishop::mask_bishop_attacks(BitBoardSquare square) {
    // result attacks bitboard
    U64 attacks = 0ULL;
    
    // init ranks & files
    int r, f;
    
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    
    // mask relevant bishop occupancy bits
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));
    
    // return attack map
    return attacks;
}

U64 AttackTables::Bishop::bishop_attacks_on_the_fly(BitBoardSquare square, U64 block) {
    // result attacks bitboard
    U64 attacks = 0ULL;
    
    // init ranks & files
    int r, f;
    
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    
    // generate bishop atacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    
    // return attack map
    return attacks;
}

U64 AttackTables::Rook::mask_rook_attacks(BitBoardSquare square) {
    // result attacks bitboard
    U64 attacks = 0ULL;
    
    // init ranks & files
    int r, f;
    
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    
    // mask relevant bishop occupancy bits
    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));
    
    // return attack map
    return attacks;
}

U64 AttackTables::Rook::rook_attacks_on_the_fly(BitBoardSquare square, U64 block) {
    // result attacks bitboard
    U64 attacks = 0ULL;
    
    // init ranks & files
    int r, f;
    
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    
    // generate rook attacks
    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }
    
    for (r = tr - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }
    
    for (f = tf + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }
    
    for (f = tf - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }
    
    // return attack map
    return attacks;
}


