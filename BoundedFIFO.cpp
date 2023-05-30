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

class BoundedFIFO {
	uint32_t size;
	uint32_t head;
	uint32_t tail;
	vector<int> data;
	std::mutex wr_mutex;
	std::condition_variable c_in;
	std::condition_variable c_out;

	bool empty() { return (head == tail); }
	bool full() { return(head - tail == size); }

	// make two locks and synch loads stores with atomic head and tail.
public:

	BoundedFIFO(uint32_t set_size)
	{
		size = set_size;
		data = vector<int>(set_size);
		head = 0;
		tail = 0;
	}
	// Pop the next element from the queue.
	// If the buffer is empty, the calling thread waits until being notified of
	// new elements in the queue
	int pop()
	{
		std::unique_lock<std::mutex> lk(wr_mutex);
		while (empty())
		{
			c_out.wait(lk);
		}
		int data_out = data[tail % size];
		tail++;
		lk.unlock();
		c_in.notify_one();
		return data_out;
	}
	// Push a new integer to the queue.
	// If the buffer is full, the calling thread should wait until being notified 
	// that the queue is not full anymore
	// v - the new integer to push into the queue.
	void push(int v)
	{
		std::unique_lock<std::mutex> lk(wr_mutex);
		while (full())
		{
			c_in.wait(lk);
		}
		data[head % size] = v;
		head++;
		lk.unlock();
		c_out.notify_one();
	}
};

BoundedFIFO bounded_fifo(FIFO_SIZE);

void boundedConsumer()
{
	int val;
	for (uint32_t i = 0; i < NUM_ELEM; i++)
	{
		val = bounded_fifo.pop();
		std::cout << "  thread" << std::this_thread::get_id() << " poped the item: " << val << std::endl;
	}
}

void boundedProducer()
{
	for (uint32_t i = 0; i < NUM_ELEM; i++)
	{
		bounded_fifo.push(i);
	}

}

void checkBF()
{
	std::cout << "Starts printing numbers poped from MULTITHREADED IO BOUNDED FIFO:" << std::endl;
	std::thread cons1 = std::thread(boundedConsumer);
	std::thread cons2 = std::thread(boundedConsumer);

	std::thread prod1 = std::thread(boundedProducer);
	std::thread prod2 = std::thread(boundedProducer);

	cons1.join();
	cons2.join();
	prod1.join();
	prod2.join();

	std::cout << "All itemes were poped from the FIFO-queue. done printing " << std::endl;
}


int main()
{
	//BoundedFIFO check
	checkBF();
}