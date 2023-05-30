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

class SingleUnBoundedFIFO {
	// based on list
	std::list<int> data;
	atomic<uint32_t> number_of_elements;
public:
	SingleUnBoundedFIFO() :data(), number_of_elements(0) {}

	bool pop(int& value) // returns false if queue is empty
	{
		if (number_of_elements > 0)
		{
			value = data.front();
			data.pop_front();
			number_of_elements--;
			return true;
		}
		return false;
	}
	void push(int val) // v - the new integer to push into the queue.
	{
		data.push_back(val);
		number_of_elements++;
	}
};

SingleUnBoundedFIFO unbounded_single_fifo;

void singleUnBoundedConsumer()
{
	int val;
	for (uint32_t i = 0; i < NUM_ELEM; i++)
	{
		while (!unbounded_single_fifo.pop(val)) {
			std::cout << "empty" << std::endl;
		};
		std::cout << "  " << val << std::endl;
	}
}

void singleUnBoundedProducer()
{
	for (uint32_t i = 0; i < NUM_ELEM; i++)
	{
		unbounded_single_fifo.push(i);
	}
}


void checkUSBF()
{
	std::cout << "Starts printing numbers poped from SINGLETHREADED IO BOUNDED FIFO:" << std::endl;
	std::thread unbound_cons = std::thread(singleUnBoundedConsumer);

	std::thread unbound_prod = std::thread(singleUnBoundedProducer);

	unbound_cons.join();
	unbound_prod.join();

	std::cout << "All itemes were poped from the FIFO-queue. done printing " << std::endl;
}


int main()
{

	//SingleUnBoundedFIFO check

	checkUSBF();

}