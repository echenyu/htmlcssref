#ifndef	CLIENT_B_H
#define CLIENT_B_H

#include <vector>
#include <climits>
#include <queue>

int indexToUse(bool *visited, double *distances, int numberOfFacilities);

struct pathA{
	int startIndex;
	int endIndex; 
};

class client_B {
public:
	std::vector<coordinates> allCoordinates; 
	void calculatePath(std::stringstream &ssin, int numberOfFacilities);
	double calculatePath2(int numberOfFacilities); 
	double findDistance(const int &indexOne, const int &indexTwo);
};


void client_B::calculatePath(std::stringstream &ssin, int numberOfFacilities) {
	double totalDistance = 0; 
	bool visited[numberOfFacilities]; 
	double distances[numberOfFacilities];
	int parent[numberOfFacilities]; 

	for(int i = 0; i < numberOfFacilities; i++) {
		visited[i] = false;
		distances[i] = LONG_MAX; 
	}
	
	//Set some variables for the first coordinate because
	//we will always start our MST there
	std::deque <pathA> mstPath; 
	visited[0] = true; 
	distances[0] = 0; 
	parent[0] = -1; 
	int minIndex = 0; 
	

	//Loop through all the neighbors/points that can be visited
	for(int i = 0; i < numberOfFacilities-1; i++) {
		pathA shortestPath; 
		double minimum = LONG_MAX; 
		int index; 
		for(int j = 0; j < numberOfFacilities; j++) {
			if(!visited[j]) {
				double distance = findDistance(minIndex, j); 
				if(distance < distances[j]) {
					distances[j] = distance; 
					parent[j] = minIndex; 
				}	
				if(distances[j] < minimum) {
					minimum = distances[j]; 
					index = j; 
				}
			}
		}
		
		minIndex = index; 
		int parentValue = parent[index]; 
		if(parentValue < minIndex) {
			shortestPath.startIndex = parentValue;
			shortestPath.endIndex = minIndex;
		} else {
			shortestPath.startIndex = minIndex;
			shortestPath.endIndex = parentValue;
		}
		visited[minIndex] = true; 
		totalDistance += sqrt(minimum); 
		mstPath.push_back(shortestPath); 
	}
	
	ssin << totalDistance << "\n"; 
	while(!mstPath.empty()) {
		pathA pathToPrint = mstPath.front();
		mstPath.pop_front(); 
		ssin << pathToPrint.startIndex << " " << pathToPrint.endIndex << "\n"; 
	}
}

double client_B::calculatePath2(int numberOfFacilities) {
	double totalDistance = 0; 
	bool visited[numberOfFacilities]; 
	double distances[numberOfFacilities];
	int parent[numberOfFacilities]; 

	for(int i = 0; i < numberOfFacilities; i++) {
		visited[i] = false;
		distances[i] = LONG_MAX; 
	}
	
	//Set some variables for the first coordinate because
	//we will always start our MST there
	std::deque <pathA> mstPath; 
	visited[0] = true; 
	distances[0] = 0; 
	parent[0] = -1; 
	int minIndex = 0; 
	

	//Loop through all the neighbors/points that can be visited
	for(int i = 0; i < numberOfFacilities-1; i++) {
		pathA shortestPath; 
		double minimum = LONG_MAX; 
		int index; 
		for(int j = 0; j < numberOfFacilities; j++) {
			if(!visited[j]) {
				double distance = findDistance(minIndex, j); 
				if(distance < distances[j]) {
					distances[j] = distance; 
					parent[j] = minIndex; 
				}	
				if(distances[j] < minimum) {
					minimum = distances[j]; 
					index = j; 
				}
			}
		}
		
		minIndex = index; 
		int parentValue = parent[index]; 
		if(parentValue < minIndex) {
			shortestPath.startIndex = parentValue;
			shortestPath.endIndex = minIndex;
		} else {
			shortestPath.startIndex = minIndex;
			shortestPath.endIndex = parentValue;
		}
		visited[minIndex] = true; 
		totalDistance += sqrt(minimum); 
		mstPath.push_back(shortestPath); 
	}

	return totalDistance; 
}	

double client_B::findDistance(const int &indexOne, const int &indexTwo) {

	double xDifference = allCoordinates[indexOne].xCoordinate - allCoordinates[indexTwo].xCoordinate;
	double yDifference = allCoordinates[indexOne].yCoordinate - allCoordinates[indexTwo].yCoordinate;

	return xDifference * xDifference + yDifference * yDifference;;
}

#endif