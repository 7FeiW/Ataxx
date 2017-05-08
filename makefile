CC = g++
ELEVENFLAG= -Wextra -O3 -std=c++11
CCFLAGS= -Wall

all: Ataxx

Ataxx: Main.o Ataxx.o AtaxxState.o TTEntry.o
	$(CC) $(ELEVENFLAG) $(CCFLAGS) $^ -o $@

%.o : %.cpp
	$(CC) $(ELEVENFLAG) $(CCFLAGS) -c $<

Main.o: Ataxx.h AtaxxState.h

Ataxx.o: Ataxx.h TTEntry.h Utility.h

AtaxxState.o: AtaxxState.h TTEntry.h Utility.h

TTEntry.o: TTEntry.h

clean:
	rm -f *.o *.gcov *.gcda *gcno
	rm -f *~ *# .#*

clean-all : clean
	rm -f Ataxx
