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

	void minHeapify(int i);
	void buildHeap();

public:
	MinHeap(OrderMaker order);
	void insert(Record& rec, int ind);
	HeapNode* extractMin();
};
#endif