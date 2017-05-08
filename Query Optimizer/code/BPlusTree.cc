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
	cout << "Keys:      ";
	BNode::print();
	cout << "pageNum:   ";
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
leafNode* Find(int key, BNode* node) {
	if (node->type == INTERNAL) {
		// Figure out which child to use for next recursion
		internalNode* temp = (internalNode*)node;
		for (int i = 0; i < temp->childrenCount; ++i) {

		}
	}
	else if (node->type == LEAF) {
		return node;
	}
}
int BPlusTree::Insert(int key, int pageNum, int recordNum) {
	// Handle the beginning case where the root also acts as a leafNode
	// We automatically insert a leafNode into the children and does maintenance from there
	if (root->type == LEAF) {
		// Leaf is not full
		cout << root->keyCount << endl;
		if (root->keyCount < numKey) {
			// Insert First Element has no comparison
			//Insert((leafNode*)root, key, pageNum, recordNum);
			
			if (root->keyCount == 0) {
				root->key[root->keyCount] = key;
				((leafNode*)root)->info->pageNum[root->keyCount] = pageNum;
				((leafNode*)root)->info->recordNum[root->keyCount] = recordNum;
				root->keyCount += 1;
			}
			
			else {
				Insert((leafNode*)root, key, pageNum, recordNum);
			}
		}
		// Leaf is full, Need to split, and change root so its an internalNode
		else {
			cout << "Leaf is full" << endl;
			internalNode* temp = new internalNode(numKey);
			leafNode* newLeaf = new leafNode(numKey);
			leafNode* rootTemp = (leafNode*)root;
			// Calculate how many Node to steal
			int minCap = numKey / 2;
			for (int i = 0; i < minCap; ++i) {
				newLeaf->key[i] = rootTemp->key[i + minCap];
				newLeaf->info->pageNum[i] = rootTemp->info->pageNum[i + minCap];
				newLeaf->info->recordNum[i] = rootTemp->info->recordNum[i + minCap];
				newLeaf->keyCount += 1;
			}
			rootTemp->keyCount -= minCap;

			// Key Belongs to newLeaf
			if (key >= newLeaf->key[0]) {
				Insert(newLeaf, key, pageNum, recordNum);
			}
			// Key belongs to rootTemp
			else {
				Insert(rootTemp, key, pageNum, recordNum);
			}

			// Connect the nodes
			temp->children[0] = rootTemp;
			temp->children[1] = newLeaf;
			temp->childrenCount = 2;
			temp->key[0] = newLeaf->key[0];
			temp->keyCount++;
			rootTemp->parent = temp;
			newLeaf->parent = temp;
			root = temp;
		} 
	}
	else if (root->type == INTERNAL) {
		// Check which leafNode to insert to
		//BNode *temp = root;
		return 0;
	}
	return 1;
}

int BPlusTree::Insert(leafNode* leaf, int key, int pageNum, int recordNum) {
	// Figure out where to put it
	int index = -1;
	for (int i = 0; i < leaf->keyCount; ++i) {
		//cout << "Iteration 0 ";
		if (leaf->key[i] <= key) {
			//cout << "In IF" << endl;
			index = i;
		}
		else {
			//cout << "In ELSE" << endl;
			break; 
		}
	}
	//cout << "Index: " << index << endl;
	// Insert to Right
	if (index == leaf->keyCount - 1) {
		//cout << "Inserting to Right" << endl;
		leaf->key[leaf->keyCount] = key;
		leaf->info->pageNum[leaf->keyCount] = pageNum;
		leaf->info->recordNum[leaf->keyCount] = recordNum;
		leaf->keyCount += 1;
	}
	// Insert to Left
	else {
		//cout << "Inserting to Left" << endl;
		// Insert new stuff to the end of array
		leaf->key[leaf->keyCount] = key;
		leaf->info->pageNum[leaf->keyCount] = pageNum;
		leaf->info->recordNum[leaf->keyCount] = recordNum;
		// Shift last element using insertion sort
		for (int i = leaf->keyCount; i > index + 1; --i) {
			int temp;
			// Keys
			temp = leaf->key[i];
			leaf->key[i] = leaf->key[i - 1];
			leaf->key[i - 1] = temp;
			// pageNum
			temp = leaf->info->pageNum[i];
			leaf->info->pageNum[i] = leaf->info->pageNum[i - 1];
			leaf->info->pageNum[i - 1] = temp;
			// recordNum
			temp = leaf->info->recordNum[i];
			leaf->info->recordNum[i] = leaf->info->recordNum[i - 1];
			leaf->info->recordNum[i - 1] = temp;
		}
		leaf->keyCount += 1;
	}
}
/* FOR HANG DO NOT DELETE
DO NOT DELETE
DO NOT DELETE
int index = -1;
for (int i = 0; i < root->keyCount; ++i) {
if (root->key[i] <= key) {
index = i;
}
}
leafNode* rootTemp = ((leafNode*)root);
// Insert to right
if (index == root->keyCount - 1) {
root->key[root->keyCount] = key;
rootTemp->info->pageNum[root->keyCount] = pageNum;
rootTemp->info->recordNum[root->keyCount] = recordNum;
root->keyCount += 1;
}
// Insert to left
else {
// Insert new stuff to end of array
rootTemp->key[root->keyCount] = key;
rootTemp->info->pageNum[root->keyCount] = pageNum;
rootTemp->info->recordNum[root->keyCount] = recordNum;
// Shift last element using insertion sort
for (int i = rootTemp->keyCount; i > index + 1; --i) {
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

*/
int BPlusTree::Find(int key, leafNode& _leaf) {

}

void BPlusTree::print() {
	cout << "Printing from Root" << endl;
	root->print();
}