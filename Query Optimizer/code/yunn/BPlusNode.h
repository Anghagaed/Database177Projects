#ifndef _BPLUSNODE_H
#define _BPLUSNODE_H
/*	B+ Tree Data Structure
*	Handles Basic B+ Tree functionality
*  Only supports Key of type int and float as defined by c++
*/
#include <cstddef>
#include <iostream>
#include "Utility.h"

enum nodeType { INTERNAL = 0, LEAF };

struct BNode {

	int keyCount;
	BNode* parent;
	nodeType type;

	BNode();
	virtual void print() = 0;
};

struct leafNodeData {
	int key;
	int pageNum;
	int recordNum;

	leafNodeData* next, *parent;

	leafNodeData(int key, int pageNum, int recordNum);
	void update(int key, int pageNum, int recordNum);
	void insert(leafNodeData* toInsert);
	void Swap(leafNodeData* toSwap);
	leafNodeData* steal(int keyCount, int stealCount);
};

// B+ Tree Representation of Leaf Node
struct leafNode : public BNode {
	leafNodeData* data;
	leafNode* next;

	leafNode();
	~leafNode();
	virtual void print();
	void insert(int key, int pageNum, int recordNum);
	leafNode* split();
};
struct internNodeData {
	leafNode* child;
	int pageNum;
	internNodeData* next, *parent;

	internNodeData(int pageNum, leafNode* child);
	void insert(internNodeData* head);

};

// B+ Tree Representation of Internal Node
struct internalNode : public BNode {

	int *key;
	internNodeData* data;

	internalNode(int size);
	~internalNode();
	virtual void print();
	void insert(int key, int pageNum, leafNode* child);
	internalNode* split();
};
#endif