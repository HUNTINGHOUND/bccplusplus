CC=g++
CFLAGS=-std=c++20 -Ofast -march=native -pipe -fno-exceptions -flto

attacktables.o: attacktables.cpp attacktables.hpp util.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

bitboard.o: bitboard.cpp bitboard.hpp 
	$(CC) -c -o $@ $< $(CFLAGS)

board.o: board.cpp util.hpp zorbist.hpp repetition.hpp attacktables.hpp evaluation.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

evaluation.o: evaluation.cpp evaluation.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

magic.o: magic.cpp magic.hpp util.hpp attacktables.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

main.o: main.cpp board.hpp attacktables.hpp util.hpp magic.hpp move.hpp uci.hpp search.hpp zorbist.hpp evaluation.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

move.o: move.cpp move.hpp bitboard.hpp pieces.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

repetition.o: repetition.cpp repetition.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

search.o: search.cpp search.hpp repetition.hpp pieces.hpp time_control.hpp evaluation.hpp tt.hpp zorbist.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

time_control.o: time_control.cpp time_control.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

tt.o: tt.cpp tt.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

uci.o: uci.cpp uci.hpp pieces.hpp definitions.hpp repetition.hpp time_control.hpp search.hpp tt.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

util.o: util.cpp util.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

zorbist.o: zorbist.cpp zorbist.hpp
	$(CC) -c -o $@ $< $(CFLAGS)



bbc++: attacktables.o bitboard.o board.o evaluation.o magic.o main.o move.o repetition.o search.o time_control.o tt.o uci.o util.o zorbist.o
	$(CC) -o bbc++ $^ $(CFLAGS)

clean:
	rm -f *.o bbc++