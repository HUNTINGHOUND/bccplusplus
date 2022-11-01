// system headers
#include <sstream>

// local headers
#include "uci.hpp"
#include "pieces.hpp"
#include "definitions.hpp"
#include "time_control.hpp"
#include "search.hpp"

Move parse_move(std::string const & move_string, BoardRepresentation const & rep, size_t move_idx) {
    Moves move_list = rep.generate_moves(false);
    
    BitBoardSquare source_square = BitBoardSquare((move_string[move_idx] - 'a') + (8 - (move_string[move_idx + 1] - '0')) * 8);
    BitBoardSquare target_square = BitBoardSquare((move_string[move_idx + 2] - 'a') + (8 - (move_string[move_idx + 3] - '0')) * 8);
    
    for (int move_count = 0; move_count < move_list.count; move_count++) {
        Move& move = move_list.moves[move_count];
        
        if (source_square == move.get_move_source() && target_square == move.get_move_target()) {
            BoardPiece::Pieces promoted_piece = BoardPiece::Pieces(move.get_move_promoted());
            
            if (promoted_piece) {
                // promoted to queen
                if ((promoted_piece == BoardPiece::Q || promoted_piece == BoardPiece::q) && move_string[move_idx + 4] == 'q')
                    return move;
                
                // promoted to rook
                if ((promoted_piece == BoardPiece::R || promoted_piece == BoardPiece::r) && move_string[move_idx + 4] == 'r')
                    return move;
                
                // promoted to bishop
                if ((promoted_piece == BoardPiece::B || promoted_piece == BoardPiece::b) && move_string[move_idx + 4] == 'b')
                    return move;
                
                // promoted to knight
                if ((promoted_piece == BoardPiece::N || promoted_piece == BoardPiece::n) && move_string[move_idx + 4] == 'n')
                    return move;
                
                // continue the loop on possible wrong promotion
                continue;
            }
            
            return move;
        }
    }
    
    return 0;
}

void parse_position(std::string const & command, Solver & solver) {
    size_t current_char = 9;
    
    // parse UCI "startpos" command
    if (command.compare(current_char, 8, "startpos") == 0)
        solver.parse_fen(START_POSITION);
    else {
        current_char = command.find("fen", current_char);
        
        // no fen in command
        if (current_char == std::string::npos)
            solver.parse_fen(START_POSITION);
        else {
            current_char += 4;
            solver.parse_fen(command, current_char);
        }
    }
    
    // parse moves after position
    current_char = command.find("moves");
    
    if (current_char != std::string::npos) {
        current_char += 6;
        
        while(current_char < command.size()) {
            Move move = parse_move(command, solver.rep, current_char);
            
            if (!move.move)
                break;
            
            solver.repetition_index++;
            solver.repetition_table[solver.repetition_index] = solver.rep.hash_key;
            
            solver.rep.make_move(move, all_moves);
            
            while (current_char < command.size() && command[current_char] != ' ') current_char++;
            
            current_char++;
        }
        
    }
}

void parse_go(std::string const & command, Solver & solver) {
    reset_time_control();
    
    int depth = -1;
    
    size_t argument = 0;
    
    // infinite search
    if ((argument = command.find("infinite")) != std::string::npos) {};
    
    if ((argument = command.find("binc")) != std::string::npos && solver.rep.side == black)
        time_control.inc = std::stoi(command.substr(argument + 5));
    
    if ((argument = command.find("winc")) != std::string::npos && solver.rep.side == white)
        time_control.inc = std::stoi(command.substr(argument + 5));
    
    if ((argument = command.find("wtime")) != std::string::npos && solver.rep.side == white)
        time_control.time = std::stoi(command.substr(argument + 6));
    
    if ((argument = command.find("btime")) != std::string::npos && solver.rep.side == black)
        time_control.time = std::stoi(command.substr(argument + 6));
    
    if ((argument = command.find("movestogo")) != std::string::npos)
        time_control.movestogo = std::stoi(command.substr(argument + 10));
    
    if ((argument = command.find("movetime")) != std::string::npos)
        time_control.movetime = std::stoi(command.substr(argument + 9));
    
    if ((argument = command.find("depth")) != std::string::npos)
        depth = std::stoi(command.substr(argument + 6));
    
    if (time_control.movetime != -1) {
        time_control.time = time_control.movetime;
        time_control.movestogo = 1;
    }
    
    time_control.start_time = get_time_point();
    
    if (time_control.time != -1) {
        time_control.timeset = true;
        
        time_control.time /= time_control.movestogo;
        
        time_control.time -= 50;
        if (time_control.time < 0) {
            time_control.time = 0;
            
            time_control.inc -= 50;
            
            if (time_control.inc < 0) time_control.inc = 1;
        }
                
        // init stop time
        time_control.stop_time = time_control.start_time + std::chrono::milliseconds(time_control.time) + std::chrono::milliseconds(time_control.inc);
    }
    
    if (depth == -1)
        depth = 64;
    
    std::cout << "time:" << time_control.time << " inc: " << time_control.inc
              << " start:" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(time_control.start_time).time_since_epoch()).count()
              << " stop:" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(time_control.stop_time).time_since_epoch()).count()
              << " depth:" << depth << " timeset:" << time_control.timeset << "\n";
    
    solver.search_position(depth);
}

void uci_loop(Solver & solver) {
    std::string input;
    
    while(true) {
        if (!std::getline(std::cin, input))
            continue;
        
        if (input[0] == '\n')
            continue;
        
        if (input.compare(0, 7, "isready") == 0) {
            std::cout << "readyok\n";
            continue;
        } else if(input.compare(0, 8, "position") == 0) {
            parse_position(input, solver);
        } else if (input.compare(0, 10, "ucinewgame") == 0) {
            parse_position("position startpos", solver);
        } else if (input.compare(0, 2, "go") == 0)
            parse_go(input, solver);
        else if (input.compare(0, 3, "uci") == 0) {
            std::cout << "id name BBC++\n";
            std::cout << "id author Morgan\n";
            std::cout << "option name Hash type spin default 64 min 4 max " << MAX_HASH << "\n";
            std::cout << "uciok\n";
        } else if (input.compare(0, 26, "setoption name Hash value ") == 0) {
            std::stringstream ss(input);
            
            std::string g;
            int mb;
            ss >> g >> g >> g >> g >> mb;
            
            if (mb < 4) mb = 4;
            if (mb > MAX_HASH) mb = MAX_HASH;
            
            std::cout << "    Set transposition and evaluation table size to " << mb << "MB\n";
            solver.hash_table = TranspositionTable(mb);
            solver.eval_table = EvaluationTable(mb);
        } else if (input.compare("d") == 0)
            solver.rep.print_board();
        else if (input.compare(0, 4, "quit") == 0)
            break;
    }
}
