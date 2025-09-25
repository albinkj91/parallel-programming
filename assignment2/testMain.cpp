#include <cstdlib>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "task4/benchmark.hpp"
#include "task4/sorted_list.hpp"
#include "task4/sorted_list_coarse_grained_locking.hpp"
#include "task4/sorted_list_fine_grained_locking.hpp"
#include "task4/sorted_list_mcs.hpp"

void insertLoop(sorted_list_fgl<int>& list, int id) {
    for (int i = 999; i >= 0; --i) {
        // list.insert(i + 1000 * id, id);
    }
}

void removeLoop(sorted_list_fgl<int>& list, int id) {
    for (int i = id; i < 4000; i += 4) {
        // list.remove(i, id);
    }

    std::cout << "Thread " + std::to_string(id) + " done\n";
}

int main() {
    sorted_list_mcs<int> list;

    list.insert(1);
    list.insert(2);

    // for (int i = 0; i < 10000; i++) {
    //     list.insert(i);
    // }

    // auto t1 = std::thread(insertLoop, std::ref(list), 0);
    // auto t2 = std::thread(insertLoop, std::ref(list), 1);
    // auto t3 = std::thread(insertLoop, std::ref(list), 2);
    // auto t4 = std::thread(insertLoop, std::ref(list), 3);
    // t1.join();
    // t2.join();
    // t3.join();
    // t4.join();

    // for (locked_node<int>* current = list.first; current != nullptr; current = current->next) {
    //     std::cout << current->value << " ";
    // }
    // std::cout << std::endl;

    // auto s0 = std::thread(removeLoop, std::ref(list), 0);
    // auto s1 = std::thread(removeLoop, std::ref(list), 1);
    // auto s2 = std::thread(removeLoop, std::ref(list), 2);
    // auto s3 = std::thread(removeLoop, std::ref(list), 3);
    // s0.join();
    // s1.join();
    // s2.join();
    // s3.join();

    // std::cout << "done" << std::endl << "{";

    // for (locked_node<int>* current = list.first; current != nullptr; current = current->next) {
    //     std::cout << current->value << " ";
    // }
    // std::cout << "}" << std::endl;

    return 0;
}