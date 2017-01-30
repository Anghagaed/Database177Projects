/*
 * Author:	Hang V Liang
 * Purpose:	File for testing purposes
*/

#include "EfficientMap.h"
#include "InefficientMap.h"
#include "Keyify.h"
#include <iostream>

using namespace std;
// Copying from Keyify.h to test how keyify works for various data type
void IntSwap (int& a, int& b) { 
	int temp;
	temp = a;
	a = b;
	b = temp;
}

int IntCheck(int& a, int& b) { 
	return (a == b);
} 

// Test for Efficient Map for type int

class key { 
	int x;	
};

class data { 
	int y;
};
int main(int argc, char** argv) { 
	typedef Keyify < int, IntSwap, IntCheck > keyifiedInt;
	InefficientMap<keyifiedInt, keyifiedInt> foo;
	keyifiedInt bar1, bar2;
	
	bar1 = 12;
	bar2 = 43;
	foo.Insert (bar1, bar2);
	
	return 0;
}
