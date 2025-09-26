NUM=task1/numerical_integration
SIEVE=task2/sieve
SIEVE_SINGLE_THREAD=task2/sieve_single_thread
FLAKY=task3/flaky
BENCH=task4/benchmark_example
TEST=testMain

all: task_1 task_2 task_2_single task_3 task_4

task_1: $(NUM).cpp
	g++ -std=c++11 -Wall -pthread $(NUM).cpp -o $(NUM)

task_2: $(SIEVE).cpp
	g++ -std=c++11 -Wall -pthread $(SIEVE).cpp -o $(SIEVE)

task_2_single: $(SIEVE_SINGLE_THREAD).cpp
	g++ -std=c++11 -Wall -pthread $(SIEVE_SINGLE_THREAD).cpp -o $(SIEVE_SINGLE_THREAD)

task_3:
	g++ -std=c++11 -Wall -pthread $(FLAKY).cpp -o $(FLAKY)

task_4:
	g++ -std=c++11 -Wall -pthread $(BENCH).cpp -o $(BENCH)

test:
	g++ -std=c++11 -Wall -pthread -g $(TEST).cpp -o $(TEST)

clean:
	$(RM) $(NUM) $(SIEVE) $(SIEVE_SINGLE_THREAD) $(BENCH) $(TEST) $(FLAKY)
