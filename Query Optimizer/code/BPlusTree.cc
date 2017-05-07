#include "BPlusTree.h"

BNode::BNode(int size) {
	keyCount = 0;
	key = new int[size];
}

BNode::~BNode() {
	delete[] key;
}

internalNode::internalNode(int size) : BNode(size) {
	pageNum = new int[size + 1];
	children = new BNode*[size + 1];
	type = INTERNAL;
	parent = NULL;
	childrenCount = 0;
}

internalNode::~internalNode() {
	delete[] pageNum;
	for (int i = 0; i < childrenCount; ++i) {
		delete children[i];
	}
	delete[] children;
}

leafNode::RecordInfo::RecordInfo(int size) {
	pageNum = new int[size];
	recordNum = new int[size];
}

leafNode::RecordInfo::~RecordInfo() {
	delete[] pageNum;
	delete[] recordNum;
}

leafNode::leafNode(int size) : BNode(size) {
	info = new RecordInfo(size);
	next = NULL;
	parent = NULL;
	type = LEAF;
}

leafNode::~leafNode() {
	delete info;
}

BPlusTree::BPlusTree(int numKey) {
	this->numKey = numKey;
	pageCount = 0;
	root = new leafNode(numKey);
}
BPlusTree::~BPlusTree() {
	delete root;
}

int BPlusTree::Insert(int key, int pageNum, int recordNum) {
	// Handle the beginning case where the root also acts as a leafNode
	// We automatically insert a leafNode into the children and does maintenance from there
	if (root->type == LEAF) {
		if (root->keyCount < numKey) {

		}
	}
	else if (root->type == INTERNAL) {

	}
	return 1;
}



int BPlusTree::Find(int key, leafNode& _leaf) {

}

leafNode* BPlusTree::createLeafNode() {

}

internalNode* BPlusTree::createInternalNode() {

}
