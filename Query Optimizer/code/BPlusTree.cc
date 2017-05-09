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
	cout << "Key Count is " << keyCount << endl;
	cout << "Keys:      ";
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
	BNode::print();
	for (int i = 0; i < childrenCount; ++i) {
		cout << "Internal Node Child: " << i << " pageNum: " <<pageNum[i] << " Type: ";
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
leafNode* BPlusTree::Find(int key, BNode* node, int& keyIndex) {
	//cout << "node type is " << ((node->type == INTERNAL) ? "INTERNAL " : "LEAF ") << endl;
	// Recursive case
	if (node->type == INTERNAL) {
		internalNode* temp = (internalNode*)node;
		// Needs to figure out which child to use for next recursion
		// Handles the case that key is less/equal then existing key in the node
		for (int i = 0; i < temp->keyCount; ++i) {
			if (key <= temp->key[i]) {
				keyIndex = i;
				return Find(key, temp->children[i], keyIndex);
			}
		}
		// Handle the case that key is greater then existing key
		keyIndex = temp->keyCount;
		return Find(key, temp->children[temp->keyCount], keyIndex);
	}
	// Base case
	else if (node->type == LEAF) {
		return (leafNode*) node;
	}
}
int BPlusTree::Insert(int key, int pageNum, int recordNum) {
	// Handle the beginning case where the root also acts as a leafNode
	// We automatically insert a leafNode into the children and does maintenance from there
	if (root->type == LEAF) {
		// Leaf is not full
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
			internalNode* temp = new internalNode(numKey);
			leafNode* newLeaf = new leafNode(numKey);
			leafNode* rootTemp = (leafNode*)root;
			// Calculate how many Node to steal and steal Node from rootTemp
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
			temp->pageNum[0] = pageCount++;
			temp->pageNum[1] = pageCount++;
			temp->childrenCount = 2;

			temp->key[0] = newLeaf->key[0];
			temp->keyCount++;

			rootTemp->parent = temp;
			newLeaf->parent = temp;
			rootTemp->next = newLeaf;
			root = temp;
		} 
	}
	else if (root->type == INTERNAL) {
		// Check which leafNode to insert to
		//BNode *temp = root;
		cout << "root is internal" << endl;
		int toInsertIndex = -1;
		leafNode* toInsert = Find(key, root, toInsertIndex);
		//cout << "child Number " << toInsertIndex << endl;
		//cout << "Key is " << key << endl;
		//cout << "Printing to Insert" << endl;
		//toInsert->print();
		// Safe to Insert
		if (toInsert->keyCount < numKey) {
			return Insert(toInsert, key, pageNum, recordNum);
		}
		// Need to split
		else {
			//cout << "Need to split" << endl;
			internalNode* parent = (internalNode*) toInsert->parent;
			leafNode* newLeaf = new leafNode(numKey);
			newLeaf->parent = parent;
			toInsert->next = newLeaf;

			// Calculate how many Node to steal and steal Node from toInsert
			int minCap = numKey / 2;
			for (int i = 0; i < minCap; ++i) {
				newLeaf->key[i] = toInsert->key[i + minCap];
				newLeaf->info->pageNum[i] = toInsert->info->pageNum[i + minCap];
				newLeaf->info->recordNum[i] = toInsert->info->recordNum[i + minCap];
				newLeaf->keyCount += 1;
			}
			toInsert->keyCount -= minCap;
			//cout << "Finish splitting leaf" << endl;
			// Key belongs to newLeaf
			if (key >= newLeaf->key[0]) {
				Insert(newLeaf, key, pageNum, recordNum);
			}
			// Key belongs to toInsert
			else {
				Insert(toInsert, key, pageNum, recordNum);
			}
			//cout << "Finish inserting key into leaf" << endl;
			// Update internal Node
			// Safe to update
			//cout << parent << endl;
			if (parent->keyCount < numKey) {
				// Insert 
				parent->children[parent->childrenCount] = newLeaf;
				parent->pageNum[parent->childrenCount] = pageCount++;
				parent->key[parent->keyCount] = newLeaf->key[0];

				// Need to make sure that the keys are sorted
				// Insertion Sort Time
				
				for (int i = parent->keyCount; i >= 0; --i) {
					if (parent->key[i] < parent->key[i - 1]) {
						// Swap
						int temp1;
						BNode* temp2;
						// Keys
						temp1 = parent->key[i];
						parent->key[i] = parent->key[i - 1];
						parent->key[i - 1] = temp1;
						// Children Pointers
						temp2 = parent->children[i + 1];
						parent->children[i + 1] = parent->children[i];
						parent->children[i] = temp2;
					}
					else {
						// Sorted Do Not Need To Continue
						break;
					}
				}
				parent->childrenCount += 1;
				parent->keyCount += 1;

			}
			// Need to split parent
			else {
				cout << "Parent is Full" << endl;
				internalNode* temp = parent;
				internalNode* newParent = new internalNode(numKey);
				bool split = true;
				//while (split) {
					// need to split parent
					int minCap = (numKey / 2);

					int newKey = parent->key[minCap];
					newParent->pageNum[0] = parent->pageNum[minCap + 1];
					newParent->children[0] = parent->children[minCap + 1];
					for (int i = 0; i < minCap; ++i) {
						newParent->key[i] = parent->key[i + minCap + 1];
						newParent->pageNum[i + 1] = parent->pageNum[i + minCap + 2];
						newParent->children[i + 1] = parent->children[i + minCap + 2];
					}
					newParent->childrenCount = minCap;
					newParent->keyCount = minCap - 1;
					parent->childrenCount -= minCap;
					parent->keyCount -= minCap;
		
					// Insert Left
					leafNode *toInsert;
					if (key < parent->key[0]) {
						int indexNo;
						toInsert = Find(key, parent, indexNo);
						Insert(toInsert, key, pageNum, recordNum);
					}
					// Insert Right
					else {
						int indexNo;
						toInsert = Find(key, newParent, indexNo);
						Insert(toInsert, key, pageNum, recordNum);
					}
					/*
					cout << "Printing parent" << endl;
					parent->print();
					cout << "Printing new Parent" << endl;
					newParent->print();
					cout << newParent->keyCount << endl;
					*/
					// Connect them
					// Check if parent exist
					if (parent->parent != NULL) {
						internalNode* grandParent = 
					}
				//}
			}

		}
	}
	return 1;
}
/*	Insert functions currently allows duplicate key to exist
	To prevent duplicate key from existing, In the Loop of // Figure out where to put it
	add an if statement that checks if the key is the same and don't insert if it does exist
*/
int BPlusTree::Insert(leafNode* leaf, int key, int pageNum, int recordNum) {
	// Figure out where to put it
	int index = -1;
	for (int i = 0; i < leaf->keyCount; ++i) {
		if (leaf->key[i] <= key) {
			index = i;
		}
		else {
			break; 
		}
	}
	// Insert to Right
	if (index == leaf->keyCount - 1) {
		leaf->key[leaf->keyCount] = key;
		leaf->info->pageNum[leaf->keyCount] = pageNum;
		leaf->info->recordNum[leaf->keyCount] = recordNum;
		leaf->keyCount += 1;
	}
	// Insert to Left
	else {
		// Insert new stuff to the end of array
		leaf->key[leaf->keyCount] = key;
		leaf->info->pageNum[leaf->keyCount] = pageNum;
		leaf->info->recordNum[leaf->keyCount] = recordNum;
		// Shift last element using insertion sort
		for (int i = leaf->keyCount; i > index + 1; --i) {
			int temp;
			// Keys
			if (leaf->key[i] < leaf->key[i - 1]) {
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
			else {
				break;
			}
		}
		leaf->keyCount += 1;
	}
	return 1;
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

	int i = 0;

	while (i < _leaf.keyCount && key >= _leaf.key[i]) {
		i++;
	}
	if (key == _leaf.key[i]) {
		return 1;
	}
	if (_leaf.type == LEAF) {
		return 0;
	}
	
}

void BPlusTree::print() {
	cout << "Printing from Root" << endl;
	cout << "Number of Key " << numKey << endl;
	root->print();
}/*
int BPlusTree::writeToDisk(DBFile* file, Schema iNode, Schema lNode)
{
	int lastType, lastParent;
	print();
	traverseAndWrite(file, iNode, lNode, root, -1, lastType, lastParent);
	file->AppendLastIndex(lastType, lastParent);
	file->Close();
}

template <typename T>
string convert(T x)
{
	ostringstream convert;   			// stream used for the conversion
	convert << x;		      			// insert the textual representation of 'Number' in the characters in the stream
	return convert.str(); 				// set 'Result' to the contents of the stream
}

int BPlusTree::traverseAndWrite(DBFile* file, Schema iNode, Schema lNode, BNode * node, int parent, int& lastType, int& lastParent)
{
	// traverse the node
	if (node->type == LEAF) {
		// Write
		for (int i = 0; i < node->keyCount; ++i) {
			int ikey = node->key[i];
			int dpnum = ((leafNode*)node)->info->pageNum[i];
			int recnum = ((leafNode*)node)->info->recordNum[i];
			string str = convert(ikey) + convert('|') + convert(dpnum) + convert('|') + convert(recnum) + convert('|');
			char* text = new char[str.length() + 1];
			strcpy(text, str.c_str());
			FILE* fp;
			fp = fmemopen(text, str.length() * sizeof(char), "r");
			Record recTemp;
			recTemp.ExtractNextRecord(lNode, *fp);
			/*cout << "LEAF: "; recTemp.print(cout, lNode);
			cout << endl;*//*
			fclose(fp);
			delete text;
			int type = 1;	// means Leaf
			file->AppendRecordIndex(recTemp, type, parent);
		}
		lastType = LEAF;
		lastParent = parent;
		return 1;
	}
	else if (node->type == INTERNAL) {
		// Write
		internalNode* temp = (internalNode*)node;
		for (int i = 0; i < node->keyCount; ++i)
		{
			int ikey = node->key[i];
			int cpnum = temp->pageNum[i];
			string str = convert(ikey) + convert('|') + convert(cpnum) + convert('|');
			char* text = new char[str.length() + 1];
			strcpy(text, str.c_str());
			FILE* fp;
			fp = fmemopen(text, str.length() * sizeof(char), "r");
			Record recTemp;
			recTemp.ExtractNextRecord(iNode, *fp);
			fclose(fp);
			/*cout << "INTERNAL: ";  recTemp.print(cout, iNode);
			cout << endl;*//*
			delete text;
			int type = 0;	// means Leaf
			file->AppendRecordIndex(recTemp, type, parent);
		}
		//temp->recordNum;
		for (int i = 0; i < temp->childrenCount; ++i) {
			traverseAndWrite(file, iNode, lNode, (temp->children)[i], (temp->pageNum)[i], lastType, lastParent);
			//return 1;
		}
		lastType = INTERNAL;
		lastParent = parent;
	}
}*/