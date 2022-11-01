#include "evaluation.hpp"

std::array<U64, 64> Evaluation::file_masks = {};
std::array<U64, 64> Evaluation::rank_masks = {};

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
        }
    }
}

int Evaluation::interpolate_score(int opening_score, int endgame_score, int game_phase_score) {
    return (
            opening_score * game_phase_score +
            endgame_score * (opening_phase_score - game_phase_score)
            ) / opening_phase_score;
}
