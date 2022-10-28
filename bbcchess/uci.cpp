#include "uci.hpp"
#include "search.hpp"
#include <unistd.h>
# ifdef WIN64
#include <windows.h>
#else
#include <sys/time.h>
#endif

TimeControl time_control;

int input_waiting() {
#ifndef WIN32
    fd_set readfds;
    struct timeval tv;
    FD_ZERO (&readfds);
    FD_SET (fileno(stdin), &readfds);
    tv.tv_sec=0; tv.tv_usec=0;
    select(16, &readfds, 0, 0, &tv);
    return (FD_ISSET(fileno(stdin), &readfds));
#else
    static int init = 0, pipe;
    static HANDLE inh;
    DWORD dw;

    if (!init) {
        init = 1;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe) {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }
   
    if (pipe) {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
        return dw;
    } else {
      GetNumberOfConsoleInputEvents(inh, &dw);
      return dw <= 1 ? 0 : dw;
   }
#endif
}

void read_input() {
    // bytes to read holder
    int bytes;
    
    char input[256] = "", * endc;
    // listen to STDIN
    if (input_waiting()) {
        time_control.stopped = true;
        
        
        // loop to read bytes from STDIN
        do {
            bytes = read(fileno(stdin), input, 256);
        } while (bytes < 0);
        
        // search for newline
        endc = strchr(input, '\n');
        
        // if found ne line set value at point to null
        if (endc) *endc = 0;
        
        if (strlen(input) > 0) {
            if (!strncmp(input, "quit", 4)) {
                time_control.quit = true;
            } else if(!strncmp(input, "stop", 4)) {
                time_control.quit = true;
            }
        }
    }
}

void communicate() {
    // if time is up break hre
    if (time_control.timeset && get_time_point() > time_control.stop_time) {
        time_control.stopped = true;
    }
    
    read_input();
}

Move parse_move(std::string const & move_string, BoardRepresentation const & rep, size_t move_idx = 0) {
    Moves move_list = rep.generate_moves();
    
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

void parse_position(std::string const & command, BoardRepresentation & rep) {
    size_t current_char = 9;
    
    // parse UCI "startpos" command
    if (command.compare(current_char, 8, "startpos") == 0)
        rep.parse_fen(START_POSITION);
    else {
        current_char = command.find("fen", current_char);
        
        // no fen in command
        if (current_char == std::string::npos)
            rep.parse_fen(START_POSITION);
        else {
            current_char += 4;
            rep.parse_fen(command, current_char);
        }
    }
    
    // parse moves after position
    current_char = command.find("moves");
    
    if (current_char != std::string::npos) {
        current_char += 6;
        
        while(current_char < command.size()) {
            Move move = parse_move(command, rep, current_char);
            
            if (!move.move)
                break;
            
            repetition_index++;
            repetition_table[repetition_index] = rep.hash_key;
            
            rep.make_move(move, all_moves);
            
            while (current_char < command.size() && command[current_char] != ' ') current_char++;
            
            current_char++;
        }
        
    }
    
    // rep.print_board();
}

void parse_go(std::string const & command, BoardRepresentation & rep) {
    reset_time_control();
    
    int depth = -1;
    
    size_t argument = 0;
    
    // infinite search
    if ((argument = command.find("infinite")) != std::string::npos) {};
    
    if ((argument = command.find("binc")) != std::string::npos && rep.side == black)
        time_control.inc = std::stoi(command.substr(argument + 5));
    
    if ((argument = command.find("winc")) != std::string::npos && rep.side == white)
        time_control.inc = std::stoi(command.substr(argument + 5));
    
    if ((argument = command.find("wtime")) != std::string::npos && rep.side == white)
        time_control.time = std::stoi(command.substr(argument + 6));
    
    if ((argument = command.find("btime")) != std::string::npos && rep.side == black)
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
        
        if (time_control.time > 1500) time_control.time -= 50;
        
        // init stop time
        time_control.stop_time = time_control.start_time + std::chrono::milliseconds(time_control.time) + std::chrono::milliseconds(time_control.inc);
        
        // treat increment as seconds per move when time is almost up
        if (time_control.time < 1500 && time_control.inc && depth == 64) time_control.stop_time = time_control.start_time + std::chrono::milliseconds(time_control.inc - 50);
    }
    
    if (depth == -1)
        depth = 64;
    
    std::cout << "time:" << time_control.time
              << " start:" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(time_control.start_time).time_since_epoch()).count()
              << " stop:" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(time_control.stop_time).time_since_epoch()).count()
              << " depth:" << depth << " timeset:" << time_control.timeset << "\n"; 
    
    search_position(depth, rep);
}

void uci_loop(BoardRepresentation & rep) {
    std::string input;
    
    std::cout << "id name BBC++\n";
    std::cout << "id author Morgan\n";
    std::cout << "uciok\n";
    
    while(true) {
        if (!std::getline(std::cin, input))
            continue;
        
        if (input[0] == '\n')
            continue;
        
        if (input.compare(0, 7, "isready") == 0) {
            std::cout << "readyok\n";
            continue;
        } else if(input.compare(0, 8, "position") == 0) {
            parse_position(input, rep);
            
            clear_hash_table();
        } else if (input.compare(0, 10, "ucinewgame") == 0) {
            parse_position("position startpos", rep);
            
            clear_hash_table();
        } else if (input.compare(0, 2, "go") == 0)
            parse_go(input, rep);
        else if (input.compare(0, 3, "uci") == 0) {
            std::cout << "id name BBC++\n";
            std::cout << "id author Morgan\n";
            std::cout << "uciok\n";
        }
        else if (input.compare(0, 4, "quit") == 0)
            break;
    }
}

void reset_time_control() {
    time_control.quit = false;
    time_control.movestogo = 30;
    time_control.movetime = -1;
    time_control.time = -1;
    time_control.inc = 0;
    time_control.start_time = std::chrono::system_clock::now();
    time_control.stop_time = std::chrono::system_clock::now();
    time_control.timeset = false;
    time_control.stopped = false;
}
