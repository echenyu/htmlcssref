#ifndef	CLIENT_C_H
#define CLIENT_C_H

#include <sstream>
#include <vector>
#include <queue>
#include "client_B.h"


class client_C {
public:
	std::vector<coordinates> allCoordinates; 
	std::vector<std::vector <double> > distanceMatrix; 
	std::vector<int> thePath; 

	void calculateDistances(); 
	double findDistance(int indexOne, int indexTwo);
	void permute(std::vector<int> &path, std::deque<int>&unvisited);
	void findNearestNeighbor(int numberOfFacilities); 
	bool promising(const std::vector<int> &path, const std::deque<int> &unvisited); 

	double totalDistance; 
	double lowerBound;
	double upperBound; 
};	

void client_C::calculateDistances() {
	//Calculate the distance matrix
	std::vector<double> temp(allCoordinates.size()); 
	
	for(unsigned int j = 0; j < allCoordinates.size(); j++) {
		distanceMatrix.push_back(temp); 
	}

	for(unsigned int i = 0; i < allCoordinates.size(); i++) {
		for(unsigned int j = 0; j < allCoordinates.size(); j++) {
			distanceMatrix[i][j] = findDistance(i, j); 
		}
	}
}

void client_C::permute(std::vector<int> &path, std::deque<int>&unvisited) {
	
	if(!promising(path, unvisited)) {
		return; 
	}

	if(unvisited.empty()) {
		std::vector<int> tempPath; 
		int distanceIndex = path[0]; 
		double distance = 0; 
		for(unsigned int i = 0; i < path.size(); i++) {
			//std::cout << path[i] << " distance: ";
			distance += distanceMatrix[distanceIndex][path[i]];
			if(distance > upperBound) {
				return; 
			}
			tempPath.push_back(path[i]); 
			//std::cout << distance << "\n";
			distanceIndex = path[i]; 
		}
		distance += distanceMatrix[distanceIndex][path[0]]; 
		//std::cout << 0 << " distance: " << distance << "\n\n";

		if(distance < upperBound) {
			thePath = tempPath; 
			upperBound = distance; 
			totalDistance = distance; 
		}
	}
	for(unsigned int i = 0; i < unvisited.size(); ++i) {
		path.push_back(unvisited.front());
		unvisited.pop_front();
		permute(path, unvisited);
		unvisited.push_back(path.back());
		path.pop_back();
	}
}

double client_C::findDistance(int indexOne, int indexTwo) {
	double xDifference = allCoordinates[indexOne].xCoordinate - allCoordinates[indexTwo].xCoordinate;
	double yDifference = allCoordinates[indexOne].yCoordinate - allCoordinates[indexTwo].yCoordinate;
	
	return sqrt(xDifference * xDifference + yDifference * yDifference);
}

void client_C::findNearestNeighbor(int numberOfFacilities) {
	int index = 0; 
	std::vector<bool> visited(numberOfFacilities); 
	visited[index] = true; 
	upperBound = 0; 

	for(int i = 0; i < numberOfFacilities; i++) {
		double tempDistance = LONG_MAX; 
		int endIndex; 
		for(int j = 0; j < numberOfFacilities; j++) {
			if(!visited[j] && distanceMatrix[index][j] < tempDistance) {
				tempDistance = distanceMatrix[index][j];  
				endIndex = j; 
			}
		}
		thePath.push_back(index); 
		if(tempDistance == LONG_MAX) {
			tempDistance = distanceMatrix[index][0]; 
			index = 0; 
		}
		visited[endIndex] = true;
		index = endIndex; 
		upperBound+=tempDistance; 
	}
	return;
}

bool client_C::promising(const std::vector<int>&path, const std::deque<int>&unvisited) {
	lowerBound = 0; 

	if(path.empty()) {
		return true;
	}

	if(path[0] != 0) {
		return false; 
	}

	int startIndex = path[0]; 
	for(unsigned int i = 0; i < path.size(); i++) {
		lowerBound += distanceMatrix[startIndex][path[i]];
		startIndex = path[i];
	}

	if(unvisited.empty()) {
		if(lowerBound >= upperBound) {
			return false;
		} else {
			return true; 
		} 
	}

	//Find the two closest points from the start and End that haven't been visited
	double lowestDistance = LONG_MAX;
	startIndex = path[0]; 
	for(unsigned int i = 0; i < unvisited.size(); i++) {
		if(distanceMatrix[startIndex][unvisited[i]] < lowestDistance) {
			lowestDistance = distanceMatrix[startIndex][unvisited[i]]; 
		} 
	}

	lowerBound += lowestDistance; 

	lowestDistance = LONG_MAX;
	startIndex = path.back(); 
	for(unsigned int i = 0; i < unvisited.size(); i++) {
		if (distanceMatrix[startIndex][unvisited[i]] < lowestDistance) {
			lowestDistance = distanceMatrix[startIndex][unvisited[i]]; 
		}
	}

	lowerBound += lowestDistance; 

	client_B bClient; 
	for(unsigned int i = 0; i < unvisited.size(); i++) {
		bClient.allCoordinates.push_back(allCoordinates[unvisited[i]]); 
	}

	double weight = bClient.calculatePath2(unvisited.size()); 
	lowerBound += weight; 

	if(lowerBound >= upperBound) {
		return false; 
	}
	return true;
}

#endif