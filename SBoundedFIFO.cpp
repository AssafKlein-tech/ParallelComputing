#include <vector>
#include <mutex>
#include <condition_variable>
#include <list>
#include <thread>
#include <iostream>
#include <atomic>

const uint32_t FIFO_SIZE = 40;
const uint32_t NUM_ELEM = 200;


using std::vector;
using std::atomic;

class SingleBoundedFIFO {
	//right to long variables - don't need to sync. in the creation enter 2^32. all the long is writen as one - only if using MOVDIR64B 
	uint32_t size;
	atomic<uint32_t> head;
	atomic<uint32_t> tail;
	vector<int> data;
	bool empty(const uint32_t relaxed_tail) { return (head.load(std::memory_order_acquire) == relaxed_tail); }
	bool full(const uint32_t relaxed_head) { return(relaxed_head - tail.load(std::memory_order_acquire) == size); }
public:
	SingleBoundedFIFO(uint32_t size) : size(size)
	{
		data = vector<int>(size);
		head = 0;
		tail = 0;
	}
	// Pop the next element (integer value) from the queue.
	// if the buffer is empty, ruturn_false. Otherwise, true.
	bool pop(int& val)
	{
		uint32_t relaxed_tail = tail.load(std::memory_order_relaxed);
		if (empty(relaxed_tail))
			return false;
		val = data[relaxed_tail];
		tail.store(relaxed_tail + 1, std::memory_order_release);
		return true;
	}
	// Push a new integer to the queue.
	// if queue is full, return false. Otherwise, true.
	// v - the new integer to push into the queue.
	bool push(int v)
	{
		uint32_t relaxed_head = head.load(std::memory_order_relaxed);
		if (full(relaxed_head))
			return false;
		data[relaxed_head] = v;
		head.store(relaxed_head + 1, std::memory_order_release);
		return true;
	}
};





SingleBoundedFIFO bounded_single_fifo(FIFO_SIZE);

void boundedSingleConsumer()
{
	int val;
	for (uint32_t i = 0; i < NUM_ELEM; i++)
	{
		while (!bounded_single_fifo.pop(val)) {
			std::cout << "empty" << std::endl;
		};
		std::cout << "  " << val << std::endl;
	}
}

void boundedSingleProducer()
{
	for (uint32_t i = 0; i < NUM_ELEM; i++)
	{
		while (!bounded_single_fifo.push(i)) {
			std::cout << "full (trying to push " << i << ")" << std::endl;
		};
	}
}

void checkSBF()
{
	std::cout << "Starts printing numbers poped from SINGLETHREADED IO BOUNDED FIFO:" << std::endl;
	std::thread bound_cons = std::thread(boundedSingleConsumer);

	std::thread bound_prod = std::thread(boundedSingleProducer);

	bound_cons.join();
	bound_prod.join();

	std::cout << "All itemes were poped from the FIFO-queue. done printing " << std::endl;
}

int main()
{
	//SingleBoundedFIFO check
	checkSBF();

}