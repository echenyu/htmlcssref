#ifndef	CLIENT_A_H
#define CLIENT_A_H

#include <vector>
#include <queue>
#include <sstream>

struct path {
	int startPoint;
	int endPoint; 
	double distance; 
};

struct coordinates {
	double xCoordinate;
	double yCoordinate;
};

void unionFunction (int *parents, path a); 
bool sameRepresentative(path a, int parents[]);

struct pathCompare {
	bool operator() (const path a, const path b) {
		if(a.distance == b.distance) {
			if(a.startPoint == b.startPoint) {
				return b.endPoint < a.endPoint; 
			} else {
				return b.startPoint < a.startPoint; 
			}
		}
		else {
			return b.distance < a.distance; 
		}
	}
};

class client_A {
public:
	std::vector<coordinates> allCoordinates; 
	std::priority_queue<path, std::vector<path>, pathCompare> allPaths; 
	void calculatePath(int *parents, std::stringstream &ssin);
	
};

void client_A::calculatePath(int *parents, std::stringstream &ssin) {
	//First two edges can always just be calculated and popped off. 
	double totalDistance = 0; 
	int edgesAdded = 0; 
	std::queue <path> allEdges; 

	while(!allPaths.empty()) {
		
		path edgeToCheck = allPaths.top();
		allPaths.pop(); 
		if(!sameRepresentative(edgeToCheck, parents)) {
			unionFunction(parents, edgeToCheck); 
			totalDistance += edgeToCheck.distance; 
			allEdges.push(edgeToCheck); 
			edgesAdded++; 
		}
	}
	
	ssin << totalDistance << "\n"; 
	while(!allEdges.empty()) {
		path edgeToPrint = allEdges.front();
		allEdges.pop();
		ssin << edgeToPrint.startPoint << " " << edgeToPrint.endPoint << "\n";
	}
}

void unionFunction(int *parents, path a) {
	int representativeA = a.startPoint;
	int representativeB = a.endPoint; 
	
	//Can think of an optimization by choosing the smaller set that changes its reps
	while(parents[representativeA] != representativeA) {
		representativeA = parents[representativeA]; 
	}

	while(parents[representativeB] != representativeB) {
		representativeB = parents[representativeB];
	}

	if(representativeA != representativeB) {
		representativeB = a.endPoint; 
		while(parents[representativeB] != representativeB) {
			int currentRep = representativeB;
			representativeB = parents[representativeB];
			parents[currentRep] = representativeA; 
		}
		parents[representativeB] = representativeA;
	}
	
}

bool sameRepresentative(path a, int parents[]) {
	while(parents[a.startPoint] != a.startPoint) {
		a.startPoint = parents[a.startPoint];
	}

	while(parents[a.endPoint] != a.endPoint) {
		a.endPoint = parents[a.endPoint];
	}
	return a.startPoint == a.endPoint; 
}

#endif