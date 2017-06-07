#include "BPlusTree.h"
using namespace std;

int main() {
	leafNode test;
	/*
	test.insert(1, 1, 1);
	test.print();
	test.insert(2, 2, 2);
	test.print();
	test.insert(3, 3, 3);
	test.print();
	*/
	for (int i = 0; i < 10; i+=2) {
		test.insert(i, i, i);
		test.print();
	}
	for (int i = 1; i < 10; i += 2) {
		test.insert(i, i, i);
		test.print();
	}
	return 0;
}