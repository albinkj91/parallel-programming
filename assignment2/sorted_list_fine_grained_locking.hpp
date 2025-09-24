#ifndef lacpp_sorted_list_fgl_hpp
#define lacpp_sorted_list_fgl_hpp lacpp_sorted_list_fgl_hpp

#include <mutex>

template<typename T>
struct locked_node {
	T value;
	locked_node<T>* next;
	std::mutex lock;
};

template<typename T>
class sorted_list_fgl {
    std::mutex head_guard;
	public: locked_node<T>* first = nullptr;

	public:
		sorted_list_fgl() = default;
		sorted_list_fgl(const sorted_list_fgl<T>& other) = default;
		sorted_list_fgl(sorted_list_fgl<T>&& other) = default;
		sorted_list_fgl<T>& operator=(const sorted_list_fgl<T>& other) = default;
		sorted_list_fgl<T>& operator=(sorted_list_fgl<T>&& other) = default;
		~sorted_list_fgl() {
			while(first != nullptr) {
				remove(first->value);
			}
		}

		void insert(T v) {
			/* first find position */
            head_guard.lock();
			locked_node<T>* pred = nullptr;
			locked_node<T>* succ = first;
            if (first) first->lock.lock();
            if (succ != nullptr && succ->value < v) head_guard.unlock();
			while(succ != nullptr && succ->value < v) {
                if (pred) pred->lock.unlock();
				pred = succ;
				succ = succ->next;
                if (succ) succ->lock.lock();
			}
			
			/* construct new node */
			locked_node<T>* current = new locked_node<T>();
			current->value = v;

			/* insert new node between pred and succ */
			current->next = succ;
			if(pred == nullptr) {
				first = current;
                head_guard.unlock();
			} else {
				pred->next = current;
			}
            if (succ) succ->lock.unlock();
            if (pred) pred->lock.unlock();
		}

		void remove(T v) {
			/* first find position */
            head_guard.lock();
			locked_node<T>* pred = nullptr;
			locked_node<T>* current = first;
            if (current) current->lock.lock();
            if (current->value != v) head_guard.unlock();
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
                head_guard.unlock();
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
            head_guard.lock();
            locked_node<T>* prev = nullptr;
			locked_node<T>* current = first;
            if (current) current->lock.lock();
            head_guard.unlock();
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

#endif // lacpp_sorted_list_fgl_hpp
