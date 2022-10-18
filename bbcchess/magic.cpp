// system header
#include <iostream>

// local header
#include "magic.hpp"
#include "util.hpp"
#include "attacktables.hpp"

// generate magic number candidate
U64 generate_magic_number()
{
    return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
}

// find appropriate magic number
U64 find_magic_number(BitBoardSquare square, int relevant_bits, BoardPiece::RorB rorb) {
    // init occupancies
    U64 occupancies[4096];
    
    // init attack tables
    U64 attacks[4096];
    
    // init used attacks
    U64 used_attacks[4096];
    
    // init attack mask for a current
    U64 attack_mask = rorb == BoardPiece::bishop ? AttackTables::Bishop::mask_bishop_attacks(square) : AttackTables::Rook::mask_rook_attacks(square);
    
    // init occupancy indicies
    int occupancy_indicies = 1 << relevant_bits;
    
    // init occupancy indicies
    for (int index = 0; index < occupancy_indicies; index++) {
        // init occupancies
        occupancies[index] = AttackTables::set_occupancy(index, relevant_bits, attack_mask);
        
        // init attacks
        attacks[index] = rorb == BoardPiece::bishop ? AttackTables::Bishop::bishop_attacks_on_the_fly(square, occupancies[index]) :
        AttackTables::Rook::rook_attacks_on_the_fly(square, occupancies[index]);
    }
    
    // test magic numbers loop
    for (int random_count = 0; random_count < 100000000; random_count++) {
        // generate magic number candidate
        U64 magic_number = generate_magic_number();
        
        // skip inappropriate magic numbers
        if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;
        
        // init used attacks
        memset(used_attacks, 0, 4096 * sizeof(U64));
        
        // init index & fail flag
        int index;
        bool fail;
        
        // test magic index loop
        for (index = 0, fail = false; !fail && index < occupancy_indicies; index++) {
            // init magic index
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));
            
            // if magic index works
            if (used_attacks[magic_index] == 0ULL)
                // init used attacks
                used_attacks[magic_index] = attacks[index];
            
            // otherwise
            else if(used_attacks[magic_index] != attacks[index])
                // magic index doesn't work
                fail = true;
        }
        
        // if magic number works
        if (!fail)
            return magic_number;
    }
    
    // if magic number doesn't work
    std::cout << "  Magic number fails!\n";
    return 0ULL;
}

void init_magic_numbers() {
    // loop over 64 board squares
    for (int square = 0; square < 64; square++) {
        // init rook magic numbers
        AttackTables::Rook::rook_magic_numbers[square] = find_magic_number(BitBoardSquare(square), AttackTables::Rook::rook_relevant_bits[square], BoardPiece::rook);
        
        // init bishop magic numbers
        AttackTables::Bishop::bishop_magic_numbers[square] = find_magic_number(BitBoardSquare(square), AttackTables::Bishop::bishop_relevant_bits[square], BoardPiece::bishop);
    }
    
}
