#include <iostream>
#include <vector>
#include "heap_priority_queue.h"

using namespace std;

struct intptr_comp {
	bool operator() (const int *a, const int *b) const {
	return *a < *b;
	}
};

int main() {
	heap_priority_queue<int *, intptr_comp> sheap;
	vector<int> owner = {10, 5, 20, 7};

	for(auto &i : owner) {
		cout << sheap.empty() << " is empty" << endl;
		sheap.push(&i);
	}
	// *sheap.top() = 2; 
	// cout << sheap.top(); 
	cout << sheap.size() << endl;
	while(!sheap.empty()) {
		cout << *sheap.top() << " ";
		sheap.pop();
	}
	cout << endl;
	return 0; 
}