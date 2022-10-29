#ifndef time_control_hpp
#define time_control_hpp

// system headers
#include <chrono>
#include <unistd.h>
#include <cstdio>
#include <cstring>
# ifdef WIN64
#include <windows.h>
#else
#include <sys/time.h>
#endif

// local headers
#include "util.hpp"

struct TimeControl{
    // exit from engine flag
    bool quit;
    
    // UCI "movestogo" command moves counter
    int movestogo = 30;
    
    // UCI "movetime" commend time counter
    int movetime = -1;
    
    // UCI "time" command holder (ms)
    int time = -1;
    
    // UCI "inc" command's time increment holder
    int inc = 0;
    
    // UCI "starttime" command time holder
    std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
    
    // UCI "stoptime" command time holder
    std::chrono::time_point<std::chrono::system_clock> stop_time = std::chrono::system_clock::now();
    
    // variable to flag time control availability
    bool timeset = 0;
    
    // variable to flag when the time is up
    bool stopped = 0;
    
};

extern TimeControl time_control;

void reset_time_control();

/*
 Function listen to GUI's input during search.
 Its waiting for the user in[put from std::cin.
 */
int input_waiting();

// Read GUI/user input
void read_input();

// a bridge function to interact between search and GUI input
void communicate();
#endif /* time_control_hpp */
