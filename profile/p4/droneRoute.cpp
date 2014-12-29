#include <iostream>
#include <iomanip>
#include <string>
#include <getopt.h>
#include <vector>
#include <cmath>
#include <sstream>
#include <stdlib.h>
#include "client_A.h"
#include "client_B.h"
#include "client_C.h"

using namespace std;
void permute(vector<int> &path, queue<int> unvisited);

int main(int argc, char *argv[]) {
	std::ios_base::sync_with_stdio(false); 


	struct option longOpts[] = {
		{"clientType", required_argument, NULL, 'c'},
		{"help", no_argument, NULL, 'h'},
	};

	int opt = 0, index = 0; 
	char clientType; 
	stringstream ssin; 

	ssin << setprecision(2);
	ssin << fixed;
	//getopt
	while((opt = getopt_long(argc, argv, "c:h", longOpts, &index)) != -1) {
		switch(opt) {
			case 'c': 
				clientType = *optarg;
				break; 
			case 'h':
				cout << "This program a drone route\n";
				cout << "There are 2 different flags to be used\n";
				cout << "clientType/c: choose the client type.";
				cout << "There's a required argument that can be A, B, or C\n";
				cout << "help/h: Print out a help message to the screen\n"; 
				cout.flush(); 
				exit(0); 
		}
	}

	//Make sure the correct clientType is chosen
	if(clientType != 'A' && clientType != 'B' && clientType != 'C') {
		cerr << "You need to choose either clientType A, B, or C\n"; 
		cout.flush();
		exit(1); 
	}

	string trash; 
	vector<coordinates> allCoordinates;
	client_A aClient;
	client_B bClient; 
	client_C cClient;


	if(clientType == 'A') {
		cin >> trash;
		int numberOfFacilities;
		cin >> numberOfFacilities; 

		for(int i = 0; i < numberOfFacilities; i++) {
			coordinates temp;
			cin >> temp.xCoordinate;
			cin >> temp.yCoordinate; 
			aClient.allCoordinates.push_back(temp); 
		}

		cin >> trash;
		int numberOfPaths;
		cin >> numberOfPaths;

		//Use this for loop to find all the distances between the points
		for(int i = 0; i < numberOfPaths; i++) {
			int startIndex;
			int endIndex;
			double pathDistance; 

			cin >> startIndex; 
			cin >> endIndex; 

			double xDifference = aClient.allCoordinates.at(startIndex).xCoordinate - aClient.allCoordinates.at(endIndex).xCoordinate;
			double yDifference = aClient.allCoordinates.at(startIndex).yCoordinate - aClient.allCoordinates.at(endIndex).yCoordinate;

			pathDistance = sqrt(xDifference * xDifference + yDifference * yDifference);
			
			path temp;
			temp.startPoint = startIndex;
			temp.endPoint = endIndex;
			temp.distance = pathDistance; 

			aClient.allPaths.push(temp); 
		}

		int parentArray [numberOfFacilities]; 
		//Set up the parent array for Union Find
		for(int i = 0; i < numberOfFacilities; i++) {
			parentArray[i] = i; 
		}	
		aClient.calculatePath(parentArray, ssin); 

	} else if (clientType == 'B') {
		cin >> trash;
		int numberOfFacilities;
		cin >> numberOfFacilities;

		for(int i = 0; i < numberOfFacilities; i++) {
			coordinates temp;
			cin >> temp.xCoordinate;
			cin >> temp.yCoordinate; 
			bClient.allCoordinates.push_back(temp); 
		}
		bClient.calculatePath(ssin, numberOfFacilities); 

	} else if (clientType == 'C') {
		cin >> trash;
		int numberOfFacilities;
		cin >> numberOfFacilities; 

		for(int i = 0; i < numberOfFacilities; i++) {
			coordinates temp;
			cin >> temp.xCoordinate;
			cin >> temp.yCoordinate;
			cClient.allCoordinates.push_back(temp); 
		}

		cClient.calculateDistances(); 
		cClient.findNearestNeighbor(numberOfFacilities); 

		vector<int> pathToUse; 
		deque<int> unvisited; 
		for(int i = 0; i < numberOfFacilities; i++) {
			unvisited.push_back(i); 
		}

		cClient.permute(pathToUse, unvisited);

		ssin << cClient.totalDistance << "\n";
		for(int i = 0; i < numberOfFacilities; i++) {
			ssin << cClient.thePath[i] << " ";
		}
		ssin << "\n";
	}

	cout << ssin.str();
	cout.flush(); 
	return 0; 
}
