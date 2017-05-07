#include "BPlusTree.h"

using namespace std;

BNode::BNode(int size) {
	keyCount = 0;
	key = new int[size];
}

BNode::~BNode() {
	delete[] key;
}

void BNode::print() {
	for (int i = 0; i < keyCount; ++i) {
		cout << key[i] << " ";
	}
	cout << endl;
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

void internalNode::print() {
	cout << "Internal Node" << endl;
	cout << "Internal Node Keys: ";
	BNode::print();
	for (int i = 0; i < childrenCount; ++i) {
		cout << "Internal Node Child " << i << " ";
		children[i]->print();
	}
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

void leafNode::print() {
	cout << "Leaf Node" << endl;
	cout << "Keys ";
	BNode::print();
	cout << "pageNum: ";
	for (int i = 0; i < keyCount; ++i) {
		cout << info->pageNum[i] << " ";
	}
	cout << endl;
	cout << "recordNum: ";
	for (int i = 0; i < keyCount; ++i) {
		cout << info->recordNum[i] << " ";
	}
	cout << endl;
}

BPlusTree::BPlusTree(int numKey) {
	this->numKey = numKey;
	pageCount = 1;
	root = new leafNode(numKey);
}
BPlusTree::~BPlusTree() {
	delete root;
}

int BPlusTree::Insert(int key, int pageNum, int recordNum) {
	// Handle the beginning case where the root also acts as a leafNode
	// We automatically insert a leafNode into the children and does maintenance from there
	if (root->type == LEAF) {
		// Leaf is not full
		if (root->keyCount < numKey) {
			// Insert First Element has no comparison
			if (root->keyCount == 0) {
				root->key[root->keyCount] = key;
				((leafNode*)root)->info->pageNum[root->keyCount] = pageNum;
				((leafNode*)root)->info->recordNum[root->keyCount] = recordNum;
				root->keyCount += 1;
			}
			else {
				// Figure out where to put it 
				int index;
				for (int i = 0; i < root->keyCount; ++i) {
					if (root->key[i] > key) {
						index = i;
					}
				}
				// Insert to right
				if (index == root->keyCount - 1) {
					root->key[root->keyCount] = key;
					((leafNode*)root)->info->pageNum[root->keyCount] = pageNum;
					((leafNode*)root)->info->recordNum[root->keyCount] = recordNum;
					root->keyCount += 1;
				}
				// Insert to left
				else {
					leafNode* rootTemp = ((leafNode*)root);
					// Insert new stuff to end of array
					rootTemp->key[root->keyCount] = key;
					rootTemp->info->pageNum[root->keyCount] = pageNum;
					rootTemp->info->recordNum[root->keyCount] = recordNum;
					// Shift last element using insertion sort
					for (int i = rootTemp->keyCount; i > index; --i) {
						int temp;
						// Keys
						temp = rootTemp->key[i];
						rootTemp->key[i] = root->key[i - 1];
						rootTemp->key[i - 1] = temp;
						// pageNum
						temp = rootTemp->info->pageNum[i];
						rootTemp->info->pageNum[i] = rootTemp->info->pageNum[i - 1];
						rootTemp->info->pageNum[i - 1] = temp;
						// recordNum
						temp = rootTemp->info->recordNum[i];
						rootTemp->info->recordNum[i] = rootTemp->info->recordNum[i - 1];
						rootTemp->info->recordNum[i - 1] = temp;

					}
					rootTemp->keyCount += 1;
				}

			}
		}
		// Leaf is full, Need to split
		else {
			cout << "Full" << endl;
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
