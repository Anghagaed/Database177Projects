/*	B+ Tree Data Structure
 *	Handles Basic B+ Tree functionality
 *  Only supports Key of type int and float as defined by c++ 
 */
#include <cstddef>

enum BKeyType {INT = 0, FLOAT};
enum nodeType {INTERNAL = 0, LEAF};

/* Base B+ Tree Node
 * internalNode and leafNode inherits from this base Node
 * How filled = keyCount / numKey (size of the array)
 */
struct BNode {
	// Generic Key array, size of the array is numKey
	void* key;
	// count of how many keys are inserted into key array. Initialize to 0 at start
	int keyCount;
	// Parent Pointer
	BNode* parent;
};

// B+ Tree Representation of Internal Node
struct internalNode : public BNode {
	// Type of Node
	nodeType type;
	// Array of Child Page Number. Size is Base Class Member variable size + 1	
	int* pageNum;
	/* Array of pointers to BNode since children can either be internalNode or leafNode
	 * Size of array is Base Class Member variable size + 1	
	*/
	BNode* children;
	// count of how many chilren are inserted into chilren array. Initialize to 0 at start
	int childrenCount;
};

// B+ Tree Representation of Leaf Node
struct leafNode : public BNode {
	// Array of Data Page Number. Size is Base Class Member variable size + 1	
	int* pageNum;
	// Array of Record Number. Size is Base Class Member variable size + 1	
	int* recordNum;
	// Type of Node
	nodeType type;
	// Next Sibling
	leafNode* next;
};

class BPlusTree {
private:
	internalNode root;
	// Type of the B+ Tree key
	BKeyType type;
	// N for the B+ Tree
	int numKey;
	// Page Number Counter, Root always get 0 as its page number
	int pageCount;
private:
	/* Create an Empty Leaf Node with start intialization*/
	leafNode* createLeafNode();
	/* Create an Empty Internal Node with start intialization*/
	internalNode* createInternalNode();
public:
	BPlusTree(BKeyType type, int numKey);
	~BPlusTree();

	// Insert (Key, pageNum, recordNum) into the B+ tree as a single node
	// Return 1 if successful and 0 if fails
	int Insert(void* key, int pageNum, int recordNum);
	// Put the first leaf Node found that has matching into _leaf
	// Return 1 if successful and 0 if fails
	int Find(void* key, leafNode& _leaf);

	int writeToDisk(DBFile* file, string fileName);
};