NUM=task1/numerical_integration
FLAKY=task3/flaky
BENCH=task4/benchmark_example
TEST=testMain

all: task_1 task_3 task_4

task_1: $(NUM).cpp
	g++ -std=c++11 -Wall -pthread $(NUM).cpp -o $(NUM)

test:
	g++ -std=c++11 -Wall -pthread $(TEST).cpp -o $(TEST)

task_3:
	g++ -std=c++11 -Wall -pthread $(FLAKY).cpp -o $(FLAKY)

task_4:
	g++ -std=c++11 -Wall -pthread $(BENCH).cpp -o $(BENCH)

clean:
	$(RM) $(NUM) $(BENCH) $(TEST) $(FLAKY)