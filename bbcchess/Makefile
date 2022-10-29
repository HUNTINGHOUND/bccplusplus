CC=g++
CFLAGS=-std=c++20 -Ofast -march=native -pipe -fno-exceptions -flto -DUSE_SSE41 -msse4.1 -DUSE_SSSE3 -mssse3 -DUSE_SSE2 -msse2 -DUSE_SSE -msse

nnue/misc.o: nnue/misc.cpp nnue/misc.h
	$(CC) -c -o $@ $< $(CFLAGS)

nnue/nnue.o: nnue/nnue.cpp nnue/nnue.h nnue/misc.h
	$(CC) -c -o $@ $< $(CFLAGS)

attacktables.o: attacktables.cpp attacktables.hpp util.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

bitboard.o: bitboard.cpp bitboard.hpp 
	$(CC) -c -o $@ $< $(CFLAGS)

board.o: board.cpp util.hpp zorbist.hpp attacktables.hpp evaluation.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

evaluation.o: evaluation.cpp evaluation.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

magic.o: magic.cpp magic.hpp util.hpp attacktables.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

main.o: main.cpp board.hpp attacktables.hpp util.hpp magic.hpp move.hpp uci.hpp search.hpp zorbist.hpp evaluation.hpp solver.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

move.o: move.cpp move.hpp bitboard.hpp pieces.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

nnue_eval.o: nnue_eval.cpp nnue_eval.hpp nnue/nnue.h
	$(CC) -c -o $@ $< $(CFLAGS)

search.o: search.cpp search.hpp pieces.hpp time_control.hpp evaluation.hpp zorbist.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

solver.o: solver.cpp solver.hpp time_control.hpp definitions.hpp search.hpp pieces.hpp tt.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

time_control.o: time_control.cpp time_control.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

tt.o: tt.cpp tt.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

uci.o: uci.cpp uci.hpp pieces.hpp definitions.hpp time_control.hpp search.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

util.o: util.cpp util.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

zorbist.o: zorbist.cpp zorbist.hpp
	$(CC) -c -o $@ $< $(CFLAGS)



bbc++: attacktables.o bitboard.o board.o evaluation.o magic.o main.o move.o search.o solver.o time_control.o tt.o uci.o util.o zorbist.o nnue/nnue.o nnue/misc.o nnue_eval.o
	$(CC) -o bbc++ $^ $(CFLAGS)

clean:
	rm -f *.o nnue/*.o nnue/*.o bbc++