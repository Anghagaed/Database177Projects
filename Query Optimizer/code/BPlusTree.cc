#include "BPlusTree.h"

BPlusTree::BPlusTree(BKeyType type, int numKey) {
	this->pageCount = 1;
	this->numKey = numKey;
	this->type = type;
	// Initialize the parameter of the root node
	this->root.type = INTERNAL;
	this->root.childrenCount = 0;
	this->root.keyCount = 0;
	this->root.children = new BNode[numKey + 1];
	this->root.pageNum = new int[numKey + 1];
	this->root.parent = NULL;
}
BPlusTree::~BPlusTree() {

}

int BPlusTree::Insert(void* key, int pageNum, int recordNum) {
	// Handle the beginning case where the root also acts as a leafNode
	// We automatically insert a leafNode into the children and does maintenance from there
	if (root.childrenCount == 0) {
		// Create a new leaf Node and initialize it to fit the (key, pageNum, recordNum)
		leafNode* temp = createLeafNode();
		if (type == INT) {
			((int *)temp->key)[temp->keyCount] = *(int*)key;
			temp->keyCount += 1;
		}
		else if (type == FLOAT) {
			((float *)temp->key)[temp->keyCount] = *(float*)key;
			temp->keyCount += 1;
		}
		temp->pageNum[numKey] = pageNum;
		temp->recordNum[numKey] = recordNum;

		// Insert the new leaf Node into the root Node
		root.children[root.childrenCount] = temp;
		root.pageNum[root.childrenCount] = pageCount++;
		root.childrenCount += 1;
		temp->parent = root;
		return 1;
	}
	else {
		// Find the node that the key is to be inserted to
		BNode* temp;
		for (int i = 0; i < root.childrenCount; ++i) {
			if (type == INT) {
				if (*(int *)root.key[i] <= *(int *)key) {
					temp = root.children[]
				}
			}
			else if (type == FLOAT) {

			}
		}
	}

}



int BPlusTree::Find(void* key, leafNode& _leaf) {

}

leafNode* BPlusTree::createLeafNode() {
	leafNode* temp = new leafNode();
	// Values in leafNode
	temp->type = LEAF;
	temp->next = NULL;
	temp->pageNum = new int[numKey];
	temp->recordNum = new int[numKey];
	// Values in BNode
	temp->parent = NULL;
	if (type == INT) {
		temp->key = (void *) new int[numKey];
		temp->keyCount = 0;
	}
	else if (type == FLOAT) {
		temp->key = (void *) new float[numKey]; 
		temp->keyCount = 0;
	}
	return temp;
}
internalNode* BPlusTree::createInternalNode() {
	internalNode* temp = new internalNode();
	// Values in internalNode	
	temp->type = INTERNAL;
	temp->pageNum = new int[numKey + 1];
	temp->children = new children*[numKey + 1];
	temp->childrenCount = 0;
	// Values in BNode
	temp->parent = NULL;
	if (type == INT) {
		temp->key = (void *) new int[numKey];
		temp->keyCount = 0;
	}
	else if (type == FLOAT) {
		temp->key = (void *) new float[numKey];
		temp->keyCount = 0;
	}
	return temp;
}
