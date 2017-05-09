/*	B+ Tree Data Structure
 *	Handles Basic B+ Tree functionality
 *  Only supports Key of type int and float as defined by c++ 
 */
#ifndef _BPLUSTREE_H
#define _BPLUSTREE_H
#include <cstddef>
#include <iostream>
enum nodeType {INTERNAL = 0, LEAF};

/* Base B+ Tree Node
 * internalNode and leafNode inherits from this base Node
 * How filled = keyCount / numKey (size of the array)
 */
struct BNode {
	// Generic Key array, size of the array is numKey
	int* key;
	/* count of how many keys are inserted into key array. Initialize to 0 at start
	 * Max of keycount is numKey - 1
	 */
	int keyCount;
	// Parent Pointer
	BNode* parent;
	// Type of Node for Down Casting
	nodeType type;

	BNode(int size);
	~BNode();
	virtual void print();
};

// B+ Tree Representation of Internal Node
struct internalNode : public BNode {
	// Array of Child Page Number. Size is Base Class Member variable size + 1	
	int* pageNum;
	/* Array of pointers to BNode since children can either be internalNode or leafNode
	 * Size of array is Base Class Member variable size + 1	
	*/
	BNode** children;
	// count of how many chilren are inserted into chilren array. Initialize to 0 at start
	int childrenCount;

	internalNode(int size);
	~internalNode();
	virtual void print();
};

// B+ Tree Representation of Leaf Node
struct leafNode : public BNode {
	// Structure to hold Record Info associated with the key at the same index
	struct RecordInfo {
		// Array of Data Page Number. Size is Base Class Member variable size + 1	
		int* pageNum;
		// Array of Record Number. Size is Base Class Member variable size + 1	
		int* recordNum;

		RecordInfo(int size);
		~RecordInfo();
	};
	RecordInfo* info;
	// Next Sibling
	leafNode* next;

	leafNode(int size);
	~leafNode();
	virtual void print();
};



class BPlusTree {
private:
	BNode* root;
	// N for the B+ Tree
	int numKey;
	// Page Number Counter, Root always get 0 as its page number
	int pageCount;
	// height of the tree
	int height;
	// number of Node in the tree
	int numNode;
private:
	// Insert (key, pageNum, recordNum) into leaf
	int Insert(leafNode* leaf, int key, int pageNum, int recordNum);
	/* Return a leafNode (even if it is full) to insert the key into. keyIndex is the index to leaf Node from parent Node
		Set keyIndex to -1 before calling function and if still -1 then node has no parent
	*/
	leafNode* Find(int key, BNode* node, int& keyIndex);
	/* Checks if the node parent key is correct for the current node. 
	*/
	void updateKey(BNode* node, int parentIndex);
public:
	BPlusTree(int numKey);
	~BPlusTree();

	// Insert (Key, pageNum, recordNum) into the B+ tree as a single node
	// Return 1 if successful and 0 if fails
	int Insert(int key, int pageNum, int recordNum);
	// Put the first leaf Node found that has matching into _leaf
	// Return 1 if successful and 0 if fails
	int Find(int key, leafNode& _leaf);

	//int writeToDisk(DBFile* file, string fileName);
	void print();
};
#endif //_BPLUSTREE_H