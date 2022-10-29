// system headers
#include <iostream>
#include <string>

// local headers
#include "solver.hpp"
#include "time_control.hpp"
#include "definitions.hpp"
#include "search.hpp"
#include "pieces.hpp"

Solver::Solver(int ttmb, int evalmb) : hash_table(ttmb), eval_table(evalmb) {}

void Solver::search_position(int depth) {
    Search search(repetition_table, repetition_index, hash_table, eval_table);
    
    auto start = get_time_point();
    
    int score = 0;
    
    time_control.stopped = false;
    
    // define initial alpha beta bounds
    int alpha = -INFINITY_SCORE;
    int beta = INFINITY_SCORE;
    
    std::array<Move, MAX_PLY> prev_pv;
    
    // iterative deepening
    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        
        search.follow_pv = true;
        
        score = search.negascout(alpha, beta, current_depth, rep, false);
        
        if (time_control.stopped) break;
        
        // fails high or low, reset window
        if (score <= alpha) {
            alpha = -INFINITY_SCORE;
            current_depth--;
            continue;
        }
        if (score >= beta) {
            beta = INFINITY_SCORE;
            current_depth--;
            continue;
        }
        
        alpha = score - WINDOW_VAL;
        beta = score + WINDOW_VAL;
        
        if (search.pv_length[0]) {
            if (score > -MATE_VALUE && score < -MATE_SCORE)
                std::cout << "info score mate " << -(score + MATE_VALUE) / 2 - 1 << " depth " << current_depth << " nodes " << search.nodes << " time " << get_time_diff(start, get_time_point()) << " ms pv ";
            else if (score > MATE_SCORE && score < MATE_VALUE)
                std::cout << "info score mate " << (MATE_VALUE - score) / 2 + 1 << " depth " << current_depth << " nodes " << search.nodes << " time " << get_time_diff(start, get_time_point()) << " ms pv ";
            else
                std::cout << "info score cp " << score << " depth " << current_depth << " nodes " << search.nodes << " time " << get_time_diff(start, get_time_point()) << " ms pv ";
            
            // loop over the moves within a PV line
            for (int count = 0; count < search.pv_length[0]; count++) {
                search.pv_table[0][count].print_move_nonewline();
                std::cout << " ";
            }
            std::cout << "\n";
        }
        
        prev_pv = search.pv_table[0];
    }
    
    if (time_control.stopped) std::cout << "info timeout stopped\n";
    
    
    std::cout << "bestmove ";
    
    if (!time_control.stopped) search.pv_table[0][0].print_move_nonewline();
    else prev_pv[0].print_move_nonewline();
    
    std::cout << "\n";
}

void Solver::parse_fen(std::string const & fen, size_t fen_idx) {
    std::fill(rep.bitboards.begin(), rep.bitboards.end(), 0ULL);
    std::fill(rep.occupancies.begin(), rep.occupancies.end(), 0ULL);
    
    rep.side = white;
    rep.enpassant = no_sq;
    rep.castle = 0;
    
    repetition_index = 0;
    std::fill(repetition_table.begin(), repetition_table.end(), 0);
    
    hash_table.clear_hash_table();
    eval_table.clear_hash_table();
    
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            
            if (isalpha(fen[fen_idx])) {
                BoardPiece::Pieces piece = BoardPiece::char_pieces[fen[fen_idx]];
                BitBoard::set_bit(rep.bitboards[piece], square);
            } else if (isnumber(fen[fen_idx])) {
                int offset = fen[fen_idx] - '0';
                file += offset - 1;
            }
            
            fen_idx++;
        }
        fen_idx++;
    }
    
    
    if (fen[fen_idx] == 'w') rep.side = white;
    else rep.side = black;
    
    fen_idx += 2;
    
    while (fen[fen_idx] != ' ') {
        switch (fen[fen_idx]) {
            case 'K':
                rep.castle |= wk;
                break;
            case 'Q':
                rep.castle |= wq;
                break;
            case 'k':
                rep.castle |= bk;
                break;
            case 'q':
                rep.castle |= bq;
                break;
            case '-':
                break;
                
            default:
                fatal_exit("Invalid fen position\n");
        }
        
        fen_idx++;
    }
    
    fen_idx++;
    
    if (fen[fen_idx] != '-') {
        int file = fen[fen_idx] - 'a';
        int rank = 8 - (fen[fen_idx + 1] - '0');
        
        rep.enpassant = BitBoardSquare(rank * 8 + file);
    } else rep.enpassant = no_sq;
    
    fen_idx++;
    rep.fifty = std::stoi(fen.substr(fen_idx));
    
    for (int piece = BoardPiece::P; piece <= BoardPiece::K; piece++)
        rep.occupancies[white] |= rep.bitboards[piece];
    
    for (int piece = BoardPiece::p; piece <= BoardPiece::k; piece++)
        rep.occupancies[black] |= rep.bitboards[piece];
    
    rep.occupancies[both] |= rep.occupancies[white];
    rep.occupancies[both] |= rep.occupancies[black];
    
    // init hash_key
    rep.hash_key = rep.generate_hash_key();
}
