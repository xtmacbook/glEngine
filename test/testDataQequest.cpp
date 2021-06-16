
#include "dataQueue.h"
#include <iostream>

using namespace base;

struct Tip
{
	Tip(int v) :val(v) {}
	bool isNode_ = false;
	int val = 0;
};

int main()
{
	DataQequest<Tip> queues;

	Tip  nodeArray[] = {0,1,2,3,4,5,6,7,8,9};

	for (int i = 0; i < 9; i++)
	{
		queues.addRequest(&nodeArray[i]);
	}

	while (!queues.empty())
	{
		Tip* tip = queues.firstRequest();

		std::cout << tip->val << "\n";
		queues.removeFistNode();
	}

	return 0;
}