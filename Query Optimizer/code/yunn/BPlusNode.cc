#include "BPlusNode.h"

BNode::BNode() {}

leafNodeData::leafNodeData(int key, int pageNum, int recordNum) {
	update(key, pageNum, recordNum);
}

void leafNodeData::update(int key, int pageNum, int recordNum) {
	this->key = key;
	this->pageNum = pageNum;
	this->recordNum = recordNum;
	next = NULL;
	parent = NULL;
}

void leafNodeData::Swap(leafNodeData* toSwap) {
	SWAP(this->key, toSwap->key);
	SWAP(this->pageNum, toSwap->pageNum);
	SWAP(this->recordNum, toSwap->recordNum);
	SWAP(this->next, toSwap->next);
	SWAP(this->parent, toSwap->parent);
}

void leafNodeData::insert(leafNodeData* toInsert) {
	// Case of only 1 Node in the list
	if (this->next == NULL) {
		if (toInsert->key > this->key) {
			//cout << "LeafNodeData Insert Function cause temp->key > this->key error" << endl;
			//exit(0);
			this->Swap(toInsert);
			this->next = toInsert;
			toInsert->parent = this;
		}
		else {
			this->next = toInsert;
			toInsert->parent = this;
		}
	}
	// Generic Case
	else {
		leafNodeData* temp = this->next;
		if (toInsert->key < this->key) {
			this->Swap(toInsert);
			this->next = toInsert;
			toInsert->parent = this;
		}
		else {
			while (true) {
				if (toInsert->key < temp->key) {
					// Linking
					(temp->parent)->next = toInsert;
					toInsert->parent = (temp->parent);
					toInsert->next = temp;
					temp->parent = toInsert;
					break;
				}
				else {
					if (temp->next != NULL) {
						temp = temp->next;
					}
					else {
						temp->next = toInsert;
						toInsert->parent = temp;
						break;
					}
				}
			}
		}
	}
}

leafNodeData* leafNodeData::steal(int keyCount, int stealCount)
{
	// iterate to correct location
	int i = 1;
	leafNodeData* returnMe = next;
	while (i < keyCount - stealCount)
	{
		returnMe = returnMe->next;
		++i;
	}

	// break links
	returnMe->parent->next = NULL;
	returnMe->parent = NULL;

	// return the node to steal
	return returnMe;
}

leafNode::leafNode() {
	type = LEAF;
	next = NULL;
	keyCount = 0;
	parent = NULL;
}
leafNode::~leafNode() {
	leafNodeData* temp1,* temp2;
	temp1 = data;
	while (temp1 != NULL) {
		temp2 = temp1;
		temp1 = temp1->next;
		delete temp2;
	}
}

void leafNode::print() {
	std::cout << "Printing leafNode with keyCount" << keyCount << std::endl;
	if (data == NULL) {
		std::cout << "leafNode is empty" << std::endl;
		return;
	}
	std::cout << "Pair Data (key, pageNum, recordNum): ";
	leafNodeData* temp = data;
	while (temp != NULL) {
		std::cout << "(" << temp->key << ", " << temp->pageNum << ", " << temp->recordNum << "), ";
		temp = temp->next;
	}
	std::cout << std::endl;
}

void leafNode::insert(int key, int pageNum, int recordNum) {
	if (data != NULL) {
		leafNodeData* insertMe = new leafNodeData(key, pageNum, recordNum);
		data->insert(insertMe);
		++keyCount;
	}
	else {
		data = new leafNodeData(key, pageNum, recordNum);
	}
}

leafNode* leafNode::split() {
	int stealCount = keyCount / 2;
	leafNode* returnMe = new leafNode();

	// update keyCount for both new leafNode and old one
	keyCount -= stealCount;
	returnMe->keyCount = stealCount;

	// steal data from old one
	returnMe->data = data->steal(keyCount, stealCount);

	// return
	return returnMe;
}

