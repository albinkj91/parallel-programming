#ifndef lacpp_sorted_list_mcs_hpp
#define lacpp_sorted_list_mcs_hpp lacpp_sorted_list_mcs_hpp

#include <mutex>
#include <atomic>

struct qnode {
	bool locked = false;
	qnode* next = nullptr;
};

struct mcs_lock {
	std::atomic<qnode*> tail;

	void lock() {
		// std::cout << "begin locking\n";
		// this->qn = new qnode();
		qnode* pred = tail.exchange(&local_qn, std::memory_order_acquire);
		if (pred != nullptr) {
			local_qn.locked = true;
			pred->next = &local_qn;
			while (local_qn.locked) {}			
		}
		// std::cout << "done locking\n";
	}

	void unlock() {
		// std::cout << "begin unlocking\n";
		// std::cout << qn->next << std::endl;
		if (local_qn.next == nullptr) {
			qnode* temp = &local_qn;
			if (tail.compare_exchange_strong(temp, nullptr, std::memory_order_release, std::memory_order_relaxed)) {
				// std::cout << "done unlocking no successor\n";
				return;
			}
			while (local_qn.next == nullptr) {}
		}
		local_qn.next->locked = false;
		local_qn.next = nullptr;
		// std::cout << "done unlocking\n";
	}

	static thread_local qnode local_qn;
};

thread_local qnode mcs_lock::local_qn = qnode{};

template<typename T>
struct locked_node_mcs {
	T value;
	locked_node_mcs<T>* next;
	mcs_lock lock;
};

template<typename T>
class sorted_list_mcs {
    std::mutex head_guard;
	public: locked_node_mcs<T>* first = nullptr;

	public:
		sorted_list_mcs() = default;
		sorted_list_mcs(const sorted_list_mcs<T>& other) = default;
		sorted_list_mcs(sorted_list_mcs<T>&& other) = default;
		sorted_list_mcs<T>& operator=(const sorted_list_mcs<T>& other) = default;
		sorted_list_mcs<T>& operator=(sorted_list_mcs<T>&& other) = default;
		~sorted_list_mcs() {
			while(first != nullptr) {
				remove(first->value);
			}
		}

		void insert(T v) {
			/* first find position */
            head_guard.lock();
			locked_node_mcs<T>* pred = nullptr;
			locked_node_mcs<T>* succ = first;
            if (first) first->lock.lock();
            if (succ != nullptr && succ->value < v) head_guard.unlock();
			while(succ != nullptr && succ->value < v) {
                if (pred) pred->lock.unlock();
				pred = succ;
				succ = succ->next;
                if (succ) succ->lock.lock();
			}
			
			/* construct new node */
			locked_node_mcs<T>* current = new locked_node_mcs<T>();
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
			locked_node_mcs<T>* pred = nullptr;
			locked_node_mcs<T>* current = first;
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
            locked_node_mcs<T>* prev = nullptr;
			locked_node_mcs<T>* current = first;
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

#endif // lacpp_sorted_list_mcs_hpp
