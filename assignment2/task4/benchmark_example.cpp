#include <cstdlib>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <cstring>

#include "benchmark.hpp"
#include "sorted_list.hpp"
#include "sorted_list_coarse_grained_locking.hpp"
#include "sorted_list_fine_grained_locking.hpp"
#include "sorted_list_coarse_grained_tatas.hpp"
//#include "sorted_list_fine_grained_tatas.hpp"
#include "sorted_list_mcs.hpp"

static const int DATA_VALUE_RANGE_MIN = 0;
static const int DATA_VALUE_RANGE_MAX = 256;
static const int DATA_PREFILL = 512;

void usage(char *program)
{
  std::cout << "Usage: " << program << " T [<number of trapezes>]" << std::endl;
  std::cout << std::endl;
  std::cout << "  T: number of threads" << std::endl;
  std::cout << "Optional: list version, defaults to non-thread-safe version if not specified or invalid" << std::endl;
  exit(1);
}

template<typename List>
void read(List& l, int random) {
	/* read operations: 100% count */
	l.count(random % DATA_VALUE_RANGE_MAX);
}

template<typename List>
void update(List& l, int random) {
	/* update operations: 50% insert, 50% remove */
	auto choice = (random % (2*DATA_VALUE_RANGE_MAX))/DATA_VALUE_RANGE_MAX;
	if(choice == 0) {
		l.insert(random % DATA_VALUE_RANGE_MAX);
	} else {
		l.remove(random % DATA_VALUE_RANGE_MAX);
	}
}

template<typename List>
void mixed(List& l, int random) {
	/* mixed operations: 6.25% update, 93.75% count */
	auto choice = (random % (32*DATA_VALUE_RANGE_MAX))/DATA_VALUE_RANGE_MAX;
	if(choice == 0) {
		l.insert(random % DATA_VALUE_RANGE_MAX);
	} else if(choice == 1) {
		l.remove(random % DATA_VALUE_RANGE_MAX);
	} else {
		l.count(random % DATA_VALUE_RANGE_MAX);
	}
}

template<typename List>
void run_benchmark(List& l1, List& l2, int threadcnt, std::uniform_int_distribution<int> uniform_dist, std::mt19937 engine) {
	/* example use of benchmarking */
	{
		/* prefill list with 1024 elements */
		for(int i = 0; i < DATA_PREFILL; i++) {
			l1.insert(uniform_dist(engine));
		}
		benchmark(threadcnt, u8"read", [&l1](int random){
			read(l1, random);
		});
		benchmark(threadcnt, u8"update", [&l1](int random){
			update(l1, random);
		});
	}

	{
		/* start with fresh list: update test left list in random size */
		/* prefill list with 1024 elements */
		for(int i = 0; i < DATA_PREFILL; i++) {
			l2.insert(uniform_dist(engine));
		}
		benchmark(threadcnt, u8"mixed", [&l2](int random){
			mixed(l2, random);
		});
	}
}

int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i) {
    	if (strcmp(argv[i], "-h") == 0) {
    	  // DEBUGGING
    	  // std::cout << "-h detected" << std::endl;
    	  usage(argv[0]);
		}
	}
	int version = 0;
	/* get number of threads from command line */
	if(argc < 2) {
		std::cerr << u8"Please specify number of worker threads: " << argv[0] << u8" <number>\n";
		std::exit(EXIT_FAILURE);
	}
	std::istringstream ss(argv[1]);
	int threadcnt;
	if (!(ss >> threadcnt)) {
		std::cerr << u8"Invalid number of threads '" << argv[1] << u8"'\n";
		std::exit(EXIT_FAILURE);
	}
	if (argc == 3) {
		version = std::stoi(argv[2]);
		if (version > 5 || version < 0) {
			std::cout << "Default version non-thread-safe selected" << std::endl;
		} else {
			std::cout << "Version selected: " << version << std::endl;
		}
	} else {
		std::cout << "Default version non-thread-safe selected" << std::endl;
	}
	/* set up random number generator */
	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN, DATA_VALUE_RANGE_MAX);

	switch (version) {
		case 1: {
			sorted_list_cgl<int> l1;
			sorted_list_cgl<int> l2;
			run_benchmark(l1, l2, threadcnt, uniform_dist, engine);
			break;
		}
		case 2: {
			sorted_list_fgl<int> l1;
			sorted_list_fgl<int> l2;
			run_benchmark(l1, l2, threadcnt, uniform_dist, engine);
			break;
		}
		case 3: {
			sorted_list_cgl_tatas<int> l1;
			sorted_list_cgl_tatas<int> l2;
			run_benchmark(l1, l2, threadcnt, uniform_dist, engine);
			break;
		}
		//case 4: {
		//	sorted_list_fgl_tatas<int> l1;
		//	sorted_list_fgl_tatas<int> l2;
		//	run_benchmark(l1, l2, threadcnt, uniform_dist, engine);
		//	break;
		//}
		case 5: {
			sorted_list_mcs<int> l1;
			sorted_list_mcs<int> l2;
			run_benchmark(l1, l2, threadcnt, uniform_dist, engine);
			break;
		}
		default:
			sorted_list<int> l1;
			sorted_list<int> l2;
			run_benchmark(l1, l2, threadcnt, uniform_dist, engine);
			break;
	}

	return EXIT_SUCCESS;
}
