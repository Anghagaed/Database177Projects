#ifndef _RecordMinHeap_H
#define _RecordMinHeap_H

#include "Record.h"
#include "Comparison.h"
#include <vector>

using namespace std;

struct HeapNode {
	Record data;
	int index;
};
class MinHeap
{
private:
	OrderMaker x;
	vector<HeapNode*> y;

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
			minHeapify(minimum);
		}
	}

	void buildHeap()
	{
		for (int i = (y.size() - 1) / 2; i >= 0; i--)
			minHeapify(i);
	}

public:
	MinHeap(OrderMaker order)
	{
		x.Swap(order);
	}
	void insert(Record& rec, int ind)
	{
		HeapNode* n = new HeapNode;
		n->data = rec;
		n->index = ind;
		y.push_back(n);
	}
	HeapNode* extractMin()
	{
		buildHeap();
		HeapNode* temp = y[0];
		y.erase(y.begin());
		return temp;
	}
};
#endif