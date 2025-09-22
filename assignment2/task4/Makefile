PROG=numerical_integration

all: $(PROG).cpp
	g++ -std=c++11 -Wall -pthread $(PROG).cpp -o $(PROG)

list:
	g++ -std=c++11 -Wall -pthread benchmark_example.cpp -o benchmark_example

test:
	g++ -std=c++11 -Wall -pthread testMain.cpp -o testMain

task_3:
	g++ -std=c++11 -Wall -pthread task3.cpp -o task3

clean:
	$(RM) $(PROG)