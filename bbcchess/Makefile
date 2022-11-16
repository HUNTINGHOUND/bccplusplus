CC=g++
CFLAGS=-std=c++20 -Ofast -march=native -pipe -fno-exceptions -flto -DUSE_SSE41 -msse4.1 -DUSE_SSSE3 -mssse3 -DUSE_SSE2 -msse2 -DUSE_SSE -msse -Wall

INCLUDES = nnue/misc.h nnue/nnue.h attacktables.hpp bitboard.hpp board.hpp boardinfo.hpp definitions.hpp evaluation.hpp magic.hpp move.hpp nnue_eval.hpp pieces.hpp search.hpp time_control.hpp tt.hpp types.hpp uci.hpp util.hpp zorbist.hpp syzygy/tbconfig.h syzygy/stdendian.h syzygy/tbprobe.h

nnue/misc.o: nnue/misc.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

nnue/nnue.o: nnue/nnue.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

attacktables.o: attacktables.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

bitboard.o: bitboard.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

board.o: board.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

evaluation.o: evaluation.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

magic.o: magic.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

main.o: main.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

move.o: move.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

nnue_eval.o: nnue_eval.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

search.o: search.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

solver.o: solver.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

time_control.o: time_control.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

tt.o: tt.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

uci.o: uci.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

util.o: util.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

zorbist.o: zorbist.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

syzygy/tbprobe.o: syzygy/tbprobe.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)
	
syzygy/tbchess.o: syzygy/tbchess.cpp $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)



all: attacktables.o bitboard.o board.o evaluation.o magic.o main.o move.o search.o solver.o time_control.o tt.o uci.o util.o zorbist.o nnue/nnue.o nnue/misc.o nnue_eval.o syzygy/tbchess.o syzygy/tbprobe.o
	$(CC) -o bbc++ $^ $(CFLAGS)

clean:
	rm -f *.o nnue/*.o syzygy/*.o bbc++
