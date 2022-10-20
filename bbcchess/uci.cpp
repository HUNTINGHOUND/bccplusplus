#include "uci.hpp"
#include "search.hpp"

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
            
            rep.make_move(move, all_moves);
            
            while (current_char < command.size() && command[current_char] != ' ') current_char++;
            
            current_char++;
        }
        
    }
    
    // rep.print_board();
}

void parse_go(std::string const & command, BoardRepresentation & rep) {
    int depth = -1;
    
    size_t current_depth = 0;
    
    if ((current_depth = command.find("depth")) != std::string::npos)
        depth = std::stoi(command.substr(current_depth + 6));
    else depth = 9;
    
    search_position(depth, rep);
}

void uci_loop(BoardRepresentation & rep) {
    std::string input;
    
    std::cout << "id name BBC++\n";
    std::cout << "id name Morgan\n";
    std::cout << "uciok\n";
    
    while(true) {
        if (!std::getline(std::cin, input))
            continue;
        
        if (input[0] == '\n')
            continue;
        
        if (input.compare(0, 7, "isready") == 0) {
            std::cout << "readyok\n";
            continue;
        } else if(input.compare(0, 8, "position") == 0)
            parse_position(input, rep);
        else if (input.compare(0, 10, "ucinewgame") == 0)
            parse_position("position startpos", rep);
        else if (input.compare(0, 2, "go") == 0)
            parse_go(input, rep);
        else if (input.compare(0, 3, "uci") == 0) {
            std::cout << "id name BBC++\n";
            std::cout << "uciok\n";
        }
        else if (input.compare(0, 4, "quit") == 0)
            break;
    }
}
