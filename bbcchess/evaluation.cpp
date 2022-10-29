#include "evaluation.hpp"

std::array<U64, 64> Evaluation::file_masks = {};
std::array<U64, 64> Evaluation::rank_masks = {};
std::array<U64, 64> Evaluation::isolated_masks = {};
std::array<std::array<U64, 64>, 2> Evaluation::passed_masks = {};

U64 Evaluation::set_file_rank_mask(int file_number, int rank_number) {
    // file or rank mask
    U64 mask = 0ULL;
    
    // loop over ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;
            
            if (file_number != -1)
            {
                // on file match
                if (file == file_number)
                    // set bit on mask
                    BitBoard::set_bit(mask, square);
            }
            
            else if (rank_number != -1)
            {
                // on rank match
                if (rank == rank_number)
                    // set bit on mask
                    BitBoard::set_bit(mask, square);
            }
        }
    }
    
    // return mask
    return mask;
}

void Evaluation::init_evaluation_masks() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            
            file_masks[square] |= set_file_rank_mask(file, -1);
            rank_masks[square] |= set_file_rank_mask(-1, rank);
            
            isolated_masks[square] |= set_file_rank_mask(file - 1, -1) | set_file_rank_mask(file + 1, -1);
        }
    }
    
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            
            passed_masks[white][square] |= set_file_rank_mask(file - 1, -1) | set_file_rank_mask(file, -1) | set_file_rank_mask(file + 1, -1);
            passed_masks[black][square] |= set_file_rank_mask(file - 1, -1) | set_file_rank_mask(file, -1) | set_file_rank_mask(file + 1, -1);
            
            for (int i = 0; i < (8 - rank); i++) passed_masks[white][square] &= ~rank_masks[(7 - i) * 8 + file];
            for (int i = 0; i < rank + 1; i++) passed_masks[black][square] &= ~rank_masks[i * 8 + file];
        }
    }
}

int Evaluation::interpolate_score(int opening_score, int endgame_score, int game_phase_score) {
    return (
            opening_score * game_phase_score +
            endgame_score * (opening_phase_score - game_phase_score)
            ) / opening_phase_score;
}

int Evaluation::eval_interpolation_material(int piece, int game_phase_score) {
    return (
            material_score[opening][piece] * game_phase_score +
            material_score[endgame][piece] * (opening_phase_score - game_phase_score)
            ) / opening_phase_score;
}

int Evaluation::eval_interpolation_position(int piece, int square, int game_phase_score) {
    return (
            positional_score[opening][piece][square] * game_phase_score +
            positional_score[endgame][piece][square] * (opening_phase_score - game_phase_score)
            ) / opening_phase_score;
}
