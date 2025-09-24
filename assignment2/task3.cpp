#include <iostream>
#include <thread>
#include <mutex>

bool lock_printing;

struct FlakyLock
{
    std::thread::id turn;
    bool busy;

    public:
        void lock(int id) {
            auto me = std::this_thread::get_id();
            do {
                do {
                    turn = me;
                    if (lock_printing)
                        std::cout << std::to_string(id) + " waiting for lock\n";
                } while(busy);
                busy = true;
            } while (turn != me);
        }

        void unlock(int id) {
            busy = false;
        }
};


int counter;
FlakyLock lock;
std::mutex mutex_lock;

void thread_loop(int id) {
    for (int i = 0; i < 10000; ++i) {
        lock.lock(id);
        ++counter;
        lock.unlock(id);
    }
    std::cout << "Thread " + std::to_string(id) + " finished\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Please select subproblem 1 or 3: " << argv[0] << "<subproblem>\n";
        std::exit(EXIT_FAILURE);
    }
    
    int subproblem;
    try {
        subproblem = std::stoi(argv[1]);
    } catch (const std::exception&) {
        std::cerr << "Please select subproblem 1 or 3: " << argv[0] << "<subproblem>\n";
        std::exit(EXIT_FAILURE);
    }

    switch (subproblem) {
        case 1: {
            lock_printing = false;
            auto t0 = std::thread(thread_loop, 0);
            auto t1 = std::thread(thread_loop, 1);
            t0.join();
            t1.join();
            break;
        }
        case 3: {
            lock_printing = true;
            auto t0 = std::thread(thread_loop, 0);
            auto t1 = std::thread(thread_loop, 1);
            auto t2 = std::thread(thread_loop, 2);
            auto t3 = std::thread(thread_loop, 3);
            t0.join();
            t1.join();
            t2.join();
            t3.join();
        }
        default:
            std::cerr << "Please select subproblem 1 or 3: " << argv[0] << "<subproblem>\n";
            std::exit(EXIT_FAILURE);
            break;
    }

    // auto t0 = std::thread(thread_loop, 0);
    // auto t1 = std::thread(thread_loop, 1);
    // auto t2 = std::thread(thread_loop, 2);
    // auto t3 = std::thread(thread_loop, 3);

    // t0.join();
    // t1.join();
    // t2.join();
    // t3.join();

    std::cout << counter << std::endl;
}