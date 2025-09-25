#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <cstdlib>
#include <cstring>

void usage(char *program)
{
  std::cout << "Usage: " << program << " T N [-t <number of trapezes>]" << std::endl;
  std::cout << std::endl;
  std::cout << "  T: number of threads" << std::endl;
  std::cout << "  N: number of trapezes" << std::endl;
  std::cout << "Optional: -t if threads should take a ticket" << std::endl;
  exit(1);
}

std::mutex print;

void thread_function_integral_fixed_segments(double *sums, int index, double startX, int numOfSegments, double stepSize) {
  // FOR DEBUGGING
  // print.lock();
  // std::cout << "Thread " << index << " started" /*after sleeping for " << sleepTime << "s"*/ << std::endl;
  // print.unlock();

  double localSum = 0.0;

  // Extra loop for debugging because each thread finished before the next has even started, so we didn't believe it was parallel.
  // But if using the following extra loop (together with the division of the sum by 1000), we get the correct result and can
  // actually see, that the threads run in parallel. So I guess it's just completely useless to parallelize this

  // for (int j = 0; j < 1000; ++j) {
  double currentX = startX;
  for (int i = 0; i < numOfSegments; ++i) {
    double y1 = 4 / (1 + currentX * currentX);
    currentX += stepSize;
    double y2 = 4 / (1 + currentX * currentX);
    double area = (y1 + y2) * stepSize / 2;
    localSum += area;
  }

  sums[index] = localSum;
  // }
  // data[index] /= 1000;

  // FOR DEBUGGING
  // print.lock();
  // std::cout << "Thread " << index << " finished" << std::endl;
  // print.unlock();
}

double integral_fixed_segments(int numOfThreads, int numOfTrapezes) {
  // allocate and initialize sums[] and threads[]
  double *sums = new double[numOfThreads];
  std::thread *threads = new std::thread[numOfThreads];

  // determine how many trapezes each thread gets
  int minSegPerThread = numOfTrapezes / numOfThreads;
  int numOfThreadsWithOneExtra = numOfTrapezes % numOfThreads;
  double stepSize = 1.0 / numOfTrapezes;

  double startX = 0;
  // the first <numOfThreadsWithOneExtra>-threads get an extra trapez more than the rest 
  for (int i = 0; i < numOfThreadsWithOneExtra; ++i) {
      threads[i] = std::thread(thread_function_integral_fixed_segments, sums, i, startX, minSegPerThread + 1, stepSize);
      startX += stepSize * (minSegPerThread + 1);
  }
  
  // all the other threads get <minSegPerThread> threads
  for (int i = numOfThreadsWithOneExtra; i < numOfThreads; i++) {
      threads[i] = std::thread(thread_function_integral_fixed_segments, sums, i, startX, minSegPerThread, stepSize);
      startX += stepSize * minSegPerThread;
  }

  // summing up all the results when ready
  double totalSum = 0;
  for (int i = 0; i < numOfThreads; ++i) {
      threads[i].join();
      totalSum += sums[i];
  }

  // deallocate memory
  delete[] sums;
  delete[] threads;

  return totalSum;
}


int ticket_machine = 0;
std::mutex ticket_lock;

void thread_function_integral_with_tickets(double *sums, int index, int numOfSegments, double trapezSize, int totalNumOfTrapezes) {
  double localSum = 0.0;
  while (ticket_machine < totalNumOfTrapezes) {
    ticket_lock.lock();
    int startX = ticket_machine;
    ticket_machine += numOfSegments;
    ticket_lock.unlock();

    double currentX = startX * trapezSize;
    for (int i = startX; i < startX + numOfSegments && i < totalNumOfTrapezes; ++i) {
      double y1 = 4 / (1 + currentX * currentX);
      currentX += trapezSize;
      double y2 = 4 / (1 + currentX * currentX);
      double area = (y1 + y2) * trapezSize / 2;
      localSum += area;
    }
  }

  sums[index] = localSum;
}

double integral_with_tickets(int numOfThreads, int numOfTrapezes, int numOfSegments) {
  // allocate and initialize sums[] and threads[]
  double *sums = new double[numOfThreads];
  std::thread *threads = new std::thread[numOfThreads];

  double trapezSize = 1.0 / numOfTrapezes;
  for (int i = 0; i < numOfThreads; ++i) {
    threads[i] = std::thread(thread_function_integral_with_tickets, sums, i, numOfSegments, trapezSize, numOfTrapezes);
  }

  // summing up all the results when ready
  double totalSum = 0;
  for (int i = 0; i < numOfThreads; ++i) {
      threads[i].join();
      totalSum += sums[i];
  }

  // deallocate memory
  delete[] sums;
  delete[] threads;

  return totalSum;
}

int main(int argc, char *argv[])
{
  bool use_ticketing = false;
  int numOfSegments;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-h") == 0) {
      // DEBUGGING
      // std::cout << "-h detected" << std::endl;
      usage(argv[0]);
    } else if (strcmp(argv[i], "-t") == 0) {
      use_ticketing = true;

      if (argc == 5) {
        try {
          numOfSegments = std::stoi(argv[i + 1]);
        } catch (const std::exception&) {
          usage(argv[0]);
        }
      } else {
        usage(argv[0]);
      }
    }
  }
  
  if (argc < 3 || argc > 5) {
    usage(argv[0]);
  }

  // numOfThreads = argv[1]
  int numOfThreads;
  try {
    numOfThreads = std::stoi(argv[1]);
  } catch (const std::exception&) {
    usage(argv[0]);
  }
  
  if (numOfThreads < 1) {
    usage(argv[0]);
  }

  // numberOfTrapezes = argv[2]
  int numberOfTrapezes;
  try {
    numberOfTrapezes = std::stoi(argv[2]);
  } catch (const std::exception&) {
    usage(argv[0]);
  }
  
  if (numberOfTrapezes < 1) {
    usage(argv[0]);
  }

  std::cout << "Computing integral with " << (use_ticketing ? "ticket machine" : "fixed segments per threads") << std::endl;

  auto start_time = std::chrono::system_clock::now();

  double totalSum;
  if (use_ticketing) {
    totalSum = integral_with_tickets(numOfThreads, numberOfTrapezes, numOfSegments);
  } else {
    totalSum = integral_fixed_segments(numOfThreads, numberOfTrapezes);
  }

  auto end_time = std::chrono::system_clock::now();

  std::chrono::duration<double> duration = end_time - start_time;
  std::cout << "Total sum: " << totalSum << " after " << duration.count() << "s" << std::endl;

  return 0;
}