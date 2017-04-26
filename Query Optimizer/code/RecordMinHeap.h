#include "Record.h"
#include "Comparison.h"
#include <vector>

using namespace std;

struct Node {
	Record data;
	int index;
};
class Heap
{
private:
	OrderMaker x;
	vector<Node*> y;

private:

	void minHeapify(int i)
	{
		int l = 2 * i;
		int r = 2 * i + 1;
		int minimum; 
		if (l < y.size() && x.Run(y[l]->data, y[i]->data) == -1)
			minimum = l;
		else
			minimum = i;

		if (r < y.size() && x.Run(y[r]->data, y[minimum]->data) == -1)
			minimum = r;

		if (minimum != i)
		{
			std::swap(y[i], y[minimum]);
			MinHeapify(minimum);
		}
	}

	void buildHeap()
	{
		for (int i = (y.size() - 1) / 2; i >= 0; i--)
			minHeapify(i);
	}

public:
	Heap(OrderMaker order)
	{
		x.Swap(order);
	}
	void insert(Record rec, int ind)
	{
		Node* n;
		n->data = rec;
		n->index = ind;
		y.push_back(n);
	}
	Node* extractMin()
	{
		buildHeap();
		Node* temp = y[0];
		y.erase(y.begin());
		return temp;
	}
}