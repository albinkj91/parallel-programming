#
# example Makefile to build an executable named myprog from myprog.cpp
#
PROG=dining

all: $(PROG).cpp
	g++ -std=c++11 -Wall -pthread $(PROG).cpp -o $(PROG) -fsanitize=thread

clean:
	$(RM) $(PROG)
