#include "evaluation.hpp"

std::array<U64, 64> Evaluation::file_masks = {};
std::array<U64, 64> Evaluation::rank_masks = {};
std::array<U64, 64> Evaluation::isolated_masks = {};
std::array<std::array<U64, 64>, 2> Evaluation::passed_masks = {};
