NUM=numerical_integration
BENCH=benchmark_example
TEST=testMain
FLAKY=task3

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