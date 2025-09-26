#ifndef lacpp_sorted_list_cgl_tatas_hpp
#define lacpp_sorted_list_cgl_tatas_hpp lacpp_sorted_list_cgl_tatas_hpp

#include <atomic>
#include <iostream>

template<typename T>
class sorted_list_cgl_tatas {
    node<T>* first = nullptr;
    std::atomic<bool> atomic_b{false};

    void tatas_lock()
    {
        while(true)
        {
            while(atomic_b.load());
            if(!atomic_b.exchange(true))
                return;
        }
    }

    void tatas_unlock()
    {
        atomic_b.exchange(false);
    }

    public:
        sorted_list_cgl_tatas() = default;
	    sorted_list_cgl_tatas(const sorted_list_cgl_tatas<T>& other) = default;
	    sorted_list_cgl_tatas(sorted_list_cgl_tatas<T>&& other) = default;
	    sorted_list_cgl_tatas<T>& operator=(const sorted_list_cgl_tatas<T>& other) = default;
	    sorted_list_cgl_tatas<T>& operator=(sorted_list_cgl_tatas<T>&& other) = default;
	    ~sorted_list_cgl_tatas() {
	    	while(first != nullptr) {
	    		remove(first->value);
	    	}
	    }

        void insert(T v) {
            /* first find position */
            tatas_lock();
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
            tatas_unlock();
        }

        void remove(T v) {
            /* first find position */
            tatas_lock();
			node<T>* pred = nullptr;
			node<T>* current = first;
			while(current != nullptr && current->value < v) {
				pred = current;
				current = current->next;
			}
			if(current == nullptr || current->value != v) {
				/* v not found */
                tatas_unlock();
				return;
			}
			/* remove current */
			if(pred == nullptr) {
				first = current->next;
			} else {
				pred->next = current->next;
			}
			delete current;
            tatas_unlock();
        }

        std::size_t count(T v) {
            tatas_lock();
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
            tatas_unlock();
			return cnt;
        }
};

#endif
