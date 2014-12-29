#ifndef MEDIAN_H
#define MEDIAN_H

#include <queue>
#include <iostream> 

struct greaterOperator {
	bool operator() (int a, int b) {
		return a > b; 
	}
};

class medianDataStructure {
	public: 
		medianDataStructure () : leftSize(0), rightSize(0) {}


		void insertElement(int a) { 
			//Case for the left subtree when empty
			if(smallerHeap.empty()) {
				smallerHeap.push(a);
				leftSize++; 
				return;
			} 
			if(a < smallerHeap.top()) {
				smallerHeap.push(a); 
				leftSize++;
			} else {
				biggerHeap.push(a); 
				rightSize++; 
			}

			if(leftSize - rightSize > 1) {
				int top = smallerHeap.top();
				smallerHeap.pop();
				biggerHeap.push(top); 
				leftSize--;
				rightSize++; 
			} else if (rightSize - leftSize > 0) {
				int top = biggerHeap.top();
				biggerHeap.pop();
				smallerHeap.push(top); 
				leftSize++;
				rightSize--; 
			}


		}

		int getMedian() {
			if ((leftSize + rightSize) % 2) {
				return smallerHeap.top();
			} else {
				return (smallerHeap.top() + biggerHeap.top()) / 2;
			}
		}

		int getSize() {
			return leftSize + rightSize; 
		}

	private:
		int leftSize;
		int rightSize;
		std::priority_queue<int, std::vector<int>, std::less<int> > smallerHeap;
		std::priority_queue<int, std::vector<int>, greaterOperator> biggerHeap;

};

#endif