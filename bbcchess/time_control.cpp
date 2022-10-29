#include "time_control.hpp"

TimeControl time_control;

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
