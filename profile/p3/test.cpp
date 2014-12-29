#include "medianDataStructure.h"
#include <iostream>

using namespace std;

int main() {
	medianDataStructure data; 
	for(int i = 0; i < 20; i++) {
		data.insertElement(i); 
	}
	cout << data.getMedian() << endl;
}