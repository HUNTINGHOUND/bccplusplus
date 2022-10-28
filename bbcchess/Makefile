CC=g++
CFLAGS=-std=c++20 -Ofast -march=native -pipe -fno-exceptions

attacktables.o: attacktables.cpp attacktables.hpp util.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

evaluation.o: evaluation.cpp evaluation.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

magic.o: magic.cpp magic.hpp util.hpp attacktables.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

main.o: main.cpp board.hpp attacktables.hpp util.hpp magic.hpp move.hpp uci.hpp search.hpp zorbist.hpp evaluation.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

repetition.o: repetition.cpp repetition.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

tt.o: tt.cpp tt.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

uci.o: uci.cpp search.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

util.o: util.cpp util.hpp
	$(CC) -c -o $@ $< $(CFLAGS)

zorbist.o: zorbist.cpp zorbist.hpp
	$(CC) -c -o $@ $< $(CFLAGS)



bbc++: attacktables.o evaluation.o magic.o main.o repetition.o tt.o uci.o util.o zorbist.o
	$(CC) -o bbc++ $^ $(CFLAGS)

clean:
	rm -f *.o bbc++