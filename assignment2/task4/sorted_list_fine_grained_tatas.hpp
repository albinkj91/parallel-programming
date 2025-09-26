#ifndef lacpp_sorted_list_fgl_tatas_hpp
#define lacpp_sorted_list_fgl_tatas_hpp lacpp_sorted_list_fgl_tatas_hpp

#include <atomic>

template<typename T>
struct atomic_node {
	T value;
	atomic_node<T>* next;
    std::atomic<bool> lock;
};

template<typename T>
class sorted_list_fgl_tatas {
    std::atomic<bool> head_guard;
    atomic_node<T>* first = nullptr;

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
		sorted_list_fgl_tatas() = default;
		sorted_list_fgl_tatas(const sorted_list_fgl_tatas<T>& other) = default;
		sorted_list_fgl_tatas(sorted_list_fgl_tatas<T>&& other) = default;
		sorted_list_fgl_tatas<T>& operator=(const sorted_list_fgl_tatas<T>& other) = default;
		sorted_list_fgl_tatas<T>& operator=(sorted_list_fgl_tatas<T>&& other) = default;
		~sorted_list_fgl_tatas() {
			while(first != nullptr) {
				remove(first->value);
			}
		}

		void insert(T v) {
			/* first find position */
            head_guard.tatas_lock();
			atomic_node<T>* pred = nullptr;
			atomic_node<T>* succ = first;
            if (first) first->lock.lock();
            if (succ != nullptr && succ->value < v) head_guard.tatas_unlock();
			while(succ != nullptr && succ->value < v) {
                if (pred) pred->lock.unlock();
				pred = succ;
				succ = succ->next;
                if (succ) succ->lock.lock();
			}
			
			/* construct new node */
			atomic_node<T>* current = new atomic_node<T>();
			current->value = v;

			/* insert new node between pred and succ */
			current->next = succ;
			if(pred == nullptr) {
				first = current;
                head_guard.tatas_unlock();
			} else {
				pred->next = current;
			}
            if (succ) succ->lock.unlock();
            if (pred) pred->lock.unlock();
		}

		void remove(T v) {
			/* first find position */
            head_guard.tatas_lock();
			atomic_node<T>* pred = nullptr;
			atomic_node<T>* current = first;
            if (current) current->lock.lock();
            if (current->value != v) head_guard.tatas_unlock();
			while(current != nullptr && current->value < v) {
                if (pred) pred->lock.unlock();
				pred = current;
				current = current->next;
                if (current) current->lock.lock();
			}
			if(current == nullptr || current->value != v) {
				/* v not found */
                if (pred) pred->lock.unlock();
                if (current) current->lock.unlock();
				return;
			}
			/* remove current */
			if(pred == nullptr) {
				first = current->next;
                head_guard.tatas_unlock();
			} else {
				pred->next = current->next;
			}
            if (pred) pred->lock.unlock();
            if (current) current->lock.unlock();
			delete current;
		}

		std::size_t count(T v) {
			std::size_t cnt = 0;
			/* first go to value v */
            head_guard.tatas_lock();
            atomic_node<T>* prev = nullptr;
			atomic_node<T>* current = first;
            if (current) current->lock.lock();
            head_guard.tatas_unlock();
			while(current != nullptr && current->value < v) {
                if (current->next) current->next->lock.lock();
                prev = current;
				current = current->next;
                if (prev) prev->lock.unlock();
			}
			/* count elements */
			while(current != nullptr && current->value == v) {
				cnt++;
                if (current->next) current->next->lock.lock();
				prev = current;
				current = current->next;
                if (prev) prev->lock.unlock();
			}
            if (current) current->lock.unlock();
			return cnt;
		}
};

#endif // lacpp_sorted_list_fgl_tatas_hpp
