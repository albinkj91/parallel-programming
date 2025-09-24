#ifndef lacpp_sorted_list_cgl_hpp
#define lacpp_sorted_list_cgl_hpp lacpp_sorted_list_cgl_hpp

#include <mutex>

template<typename T>
class sorted_list_cgl {
    node<T>* first = nullptr;
    std::mutex lock;   

    public:
        sorted_list_cgl() = default;
	    sorted_list_cgl(const sorted_list_cgl<T>& other) = default;
	    sorted_list_cgl(sorted_list_cgl<T>&& other) = default;
	    sorted_list_cgl<T>& operator=(const sorted_list_cgl<T>& other) = default;
	    sorted_list_cgl<T>& operator=(sorted_list_cgl<T>&& other) = default;
	    ~sorted_list_cgl() {
	    	while(first != nullptr) {
	    		remove(first->value);
	    	}
	    }

        void insert(T v) {
            /* first find position */
            lock.lock();
			node<T>* pred = nullptr;
			node<T>* succ = first;
			while(succ != nullptr && succ->value < v) {
				pred = succ;
				succ = succ->next;
			}
			
			/* construct new node */
			node<T>* current = new node<T>();
			current->value = v;

			/* insert new node between pred and succ */
			current->next = succ;
			if(pred == nullptr) {
				first = current;
			} else {
				pred->next = current;
			}
            lock.unlock();
        }

        void remove(T v) {
            /* first find position */
            lock.lock();
			node<T>* pred = nullptr;
			node<T>* current = first;
			while(current != nullptr && current->value < v) {
				pred = current;
				current = current->next;
			}
			if(current == nullptr || current->value != v) {
				/* v not found */
                lock.unlock();
				return;
			}
			/* remove current */
			if(pred == nullptr) {
				first = current->next;
			} else {
				pred->next = current->next;
			}
			delete current;
            lock.unlock();
        }

        std::size_t count(T v) {
            lock.lock();
            std::size_t cnt = 0;
			/* first go to value v */
			node<T>* current = first;
			while(current != nullptr && current->value < v) {
				current = current->next;
			}
			/* count elements */
			while(current != nullptr && current->value == v) {
				cnt++;
				current = current->next;
			}
            lock.unlock();
			return cnt;
        }
};

#endif