#include <iostream>
#include <vector>
#include <getopt.h>
#include <string>
#include <fstream>
#include "P2random.h"
#include "poorman_priority_queue.h"
#include "sorted_priority_queue.h"
#include "heap_priority_queue.h"
#include "pairing_priority_queue.h"
#include "eecs281_priority_queue.h"
#include <deque> 

using namespace std;

struct tileStruct {
	int row;
	int column;
	int *value;
};

struct statsStruct {
	int row;
	int column;
	int value;
};

struct tileCompare {
	bool operator() (const tileStruct *x, const tileStruct *y) const {
		if(*x->value == *y->value) {
			if(x->column == y->column) {
				return y->row < x->row; 
			} else {
				return y->column < x->column; 
			}
		} else if (*x->value == -1) {
			return false;
		} else if (*y->value == -1) {
			return true;
		} else {
			return *y->value < *x->value; 
		}
	}	
};

struct tileCompare2 {
	bool operator() (const statsStruct *x, const statsStruct *y) const {
		if(x->value == y->value) {
			if(x->column == y->column) {
				return x->row < y->row; 
			} else {
				return x->column < y->column; 
			}
		} else if (x->value == -1) {
			return true;
		} else if (y->value == -1) {
			return false;
		} else {
			return x->value < y->value; 
		}
	}
};

struct tileCompare3 {
	bool operator() (const statsStruct *x, const statsStruct *y) const {
		if(x->value == y->value) {
			if(x->column == y->column) {
				return y->row < x->row; 
			} else {
				return y->column < x->column; 
			}
		} else if (x->value == -1) {
			return false;
		} else if (y->value == -1) {
			return true;
		} else {
			return y->value < x->value; 
		}
	}
};

void dynamiteFound(vector<vector<bool> > &visitedLocations, 
	vector<vector<bool> > &dynamiteVisited, 
	eecs281_priority_queue <tileStruct *, tileCompare> *dynamiteQueue,
	vector<vector<int> > &inputMap, 
	const int &size, deque<statsStruct*> &firstTiles,
	eecs281_priority_queue <tileStruct *, tileCompare> *pqueue);

void newLocations(eecs281_priority_queue<tileStruct *, tileCompare> *pqueue, 
	vector<vector<bool> > &visitedLocations,
	vector<vector<int> > &inputMap,
	tileStruct *tile, const int &largestSize);

bool exitFound(const tileStruct *tile, const int &size);



int main(int argc, char *argv[]) {
	//THIS IS A TEST VERSION. WITH ONLY POOR MANS!
	int opt = 0, index = 0; 
	bool stats = false; 
	int statsNumberOfTiles = -1; 
	string priorityQueueType; 
	struct option longOpts[] = {
		{"help", no_argument, NULL, 'h'},
		{"container", required_argument, NULL, 'c'},
		{"stats", required_argument, NULL, 's'},
	};

	opterr = false; 
	//cout << getopt_long(argc, argv, "hc:s:", longOpts, &index) << endl;
	
	//Opterr? Do i want to set or not? Error message will print currently
	while((opt = getopt_long(argc, argv, "hc:s:", longOpts, &index)) != -1) {
		switch(opt) {
			case 'h':
				cout << "There are 2 flags you can use, c and s" << endl;
				cout << "-c specifies the priority queue implemented" << endl;
				cout << "-s specifies that statistics print out" << endl;
				exit(0);
				break;
			#pragma mark make sure this ends up becoming a required argument. 
			case 'c':
				if(!(*optarg)) {
					cerr << "Enter required argument" << endl;
					exit(1);
				}
				//Check to see if I can just convert optarg to a string
				priorityQueueType = string(optarg);
				break;
			case 's':
				stats = true;
				statsNumberOfTiles = atoi(string(optarg).c_str()); 
				break;
			default:
				cerr << "Flag unrecognized. Use -h for help" << endl;
				exit(1);
				break;
		}
	}
	ifstream fin;
	string firstOne; 

	fin.open(argv[argc-1]); 

	//Make sure the final file opens. 
	if(!fin.is_open()) {
		cerr << "Make sure you include an input file and that it \n";
		cerr << "can be opened" << endl;
		exit(1); 
	}
	fin >> firstOne;


	//Declare variables!
	int rowStart = 0;
	int colStart = 0; 
	int size = 0; 
	int tilesCleared = 0; 
	int rubbleCleared = 0; 
	string line;
	string sizeString; 

	//Have a throw away string for unnecessary items
	string throwAway;
	fin >> throwAway; 
	fin >> size;

	fin >> throwAway; 
	fin >> rowStart;
	fin >> colStart;

	//Create the map. Maybe use mapmake function
	vector<vector<int> > inputMap(size, vector<int>(size, 0)); 

	if(firstOne == "M") {
		string value;
		for(int i = 0; i < size; i++) {
			for(int j = 0; j < size; j++) {
				fin >> value;
				inputMap.at(i).at(j) = atoi(value.c_str());
			}
		}
	} else if (firstOne == "R") {
		fin >> throwAway;
		int seed = 0;
		fin >> seed; 

		fin >> throwAway; 
		int max_rubble = 0;
		fin >> max_rubble; 

		fin >> throwAway; 
		int TNT = 0;
		fin >> TNT; 

		P2random::seed_mt(seed);
		for(int i = 0; i < size; i++) {
			for(int j = 0; j < size; j++) {
				inputMap.at(i).at(j) 
					= P2random::generate_tile(max_rubble, TNT);
			}
		}
	}

	//Create a bool map of visited locations
	vector<vector<bool> > visitedLocations(size, vector<bool>(size, 0));
	vector<vector<bool> > dynamiteLocations(size, vector<bool>(size, 0));
	visitedLocations[rowStart][colStart] = 1; 

	//Pqueues
	eecs281_priority_queue<tileStruct *, tileCompare> *pqueue; 
	poorman_priority_queue<tileStruct *, tileCompare> poorman;
	sorted_priority_queue<tileStruct *, tileCompare> sorted;
	heap_priority_queue<tileStruct *, tileCompare> heap; 
	//pairing_priority_queue<tileStruct *, tileCompare> pairing; 

	//Dynamite Queues
	eecs281_priority_queue<tileStruct *, tileCompare> *dynamiteQueue; 
	poorman_priority_queue<tileStruct*, tileCompare> dynamitePM; 
	sorted_priority_queue<tileStruct *, tileCompare> dynamiteS;
	heap_priority_queue<tileStruct *, tileCompare>dynamiteH;
	//pairing_priority_queue<tileStruct *, tileCompare>dynamiteP; 

	if(priorityQueueType == "POOR_MAN") {
		pqueue = &poorman;
		dynamiteQueue = &dynamitePM;
	} else if (priorityQueueType == "SORTED"){
		pqueue = &sorted; 
		dynamiteQueue = &dynamiteS;
	} else if (priorityQueueType == "BINARY") {
		pqueue = &heap;
		dynamiteQueue = &dynamiteH;
	} //else if (priority == "PAIRING") {
	// 	pqueue = &pairing;
	// 	dynamiteQueue = &dynamiteP; 
	// }

	deque <statsStruct*> firstTiles;  
	tileStruct *tile = new tileStruct;
	tile->row = rowStart;
	tile->column = colStart;
	tile->value = &inputMap[rowStart][colStart];

	//Put the first tile into the priority queue
	pqueue->push(tile); 
	visitedLocations[tile->row][tile->column] = 1;
	while(!pqueue->empty()) {
		tileStruct *tile = pqueue->top();
		pqueue->pop(); 
		if(exitFound(tile, size)) {
			break;
		}
		if(*tile->value == -1) {
			//Do dynamite things
			dynamiteQueue->push(tile);
			dynamiteFound(visitedLocations,
				dynamiteLocations, dynamiteQueue,
				inputMap, size, firstTiles, pqueue);
			pqueue->fix();
			newLocations(pqueue, visitedLocations, inputMap, tile, size); 
			visitedLocations[tile->row][tile->column] = 1; 
		} else {
			newLocations(pqueue, visitedLocations, inputMap, tile, size);
			if(*tile->value != 0) {
				statsStruct *first = new statsStruct;
				first->row = tile->row;
				first->column = tile->column;
				first->value = *tile->value;
				firstTiles.push_back(first);
				*tile->value = 0; 
			}
		}
		if(exitFound(tile, size)) {
			break;
		}
	}

	deque<statsStruct*>firstTiles2; 
	deque<statsStruct*>lastTiles; 
	heap_priority_queue<statsStruct *, tileCompare3> easiest;
	heap_priority_queue<statsStruct *, tileCompare2> hardest; 
	
	while(!firstTiles.empty()) {
		statsStruct *tile = firstTiles.front();
		if(tile->value != -1) {
			tilesCleared++;
			rubbleCleared+= tile->value;
		}
		firstTiles.pop_front();
		firstTiles2.push_back(tile); 
		lastTiles.push_front(tile); 
		easiest.push(tile);
		hardest.push(tile); 
	}
	cout << "Cleared " << tilesCleared << " tiles containing ";
	cout << rubbleCleared << " rubble and escaped.\n";

	if(statsNumberOfTiles != -1) {
		cout << "First tiles cleared:\n";
		for(int i = 0; i < statsNumberOfTiles; i++) {
			if(firstTiles2.empty()) {
				break;
			}
			statsStruct *front = firstTiles2.front(); 
			firstTiles2.pop_front(); 
			if(front->value == -1) {
				cout << "TNT at [" << front->row; 
			} else {
				cout << front->value << " at [" << front->row;
			}
			cout << "," << front->column << "]" << endl;
		}

		cout << "Last tiles cleared:\n";
		for(int i = 0; i < statsNumberOfTiles; i++) {
			if(lastTiles.empty()) {
				break;
			}
			statsStruct *front = lastTiles.front();
			lastTiles.pop_front();
			if(front->value == -1) {
				cout << "TNT at [" << front->row; 
			} else {
				cout << front->value << " at [" << front->row;
			}
			cout << "," << front->column << "]" << endl;
		}

		cout << "Easiest tiles cleared:\n";
		for(int i = 0; i < statsNumberOfTiles; i++) {
			if(easiest.empty()) {
				break;
			}
			statsStruct *front = easiest.top();
			easiest.pop();
			if(front->value == -1) {
				cout << "TNT at [" << front->row; 
			} else {
				cout << front->value << " at [" << front->row;
			}
			cout << "," << front->column << "]" << endl;
		}

		cout << "Hardest tiles cleared:\n";
		for(int i = 0; i < statsNumberOfTiles; i++) {
			if(hardest.empty()) {
				break;
			}
			statsStruct *front = hardest.top();
			hardest.pop();
			if(front->value == -1) {
				cout << "TNT at [" << front->row; 
			} else {
				cout << front->value << " at [" << front->row;
			}
			cout << "," << front->column << "]" << endl;
			delete front; 
		}
	}
	while(!hardest.empty()) {
		statsStruct *tile = hardest.top();
		hardest.pop(); 
		delete tile; 
		tile = 0; 
	}
}



void dynamiteFound(vector<vector<bool> > &visitedLocations, 
	vector<vector<bool> > &dynamiteVisited, 
	eecs281_priority_queue <tileStruct *, tileCompare> *dynamiteQueue,
	vector<vector<int> > &inputMap, 
	const int &size, deque<statsStruct*> &firstTiles,
	eecs281_priority_queue <tileStruct *, tileCompare> *pqueue) {

	while(!dynamiteQueue->empty()) {
		tileStruct *tile = dynamiteQueue->top();
		dynamiteQueue->pop();
		dynamiteVisited[tile->row][tile->column] = 1; 
		if(*tile->value != 0) {
			statsStruct *first = new statsStruct;
			first->row = tile->row;
			first->column = tile->column;
			first->value = *tile->value;
			firstTiles.push_back(first); 
		}

		if(*tile->value == -1) {
			newLocations(dynamiteQueue, dynamiteVisited, 
				inputMap, tile, size); 
			*tile->value = 0; 
		} else {
			*tile->value = 0; 
		}
	}
}

void newLocations(eecs281_priority_queue<tileStruct *, tileCompare> *pqueue, 
	vector<vector<bool> > &visitedLocations,
	vector<vector<int> > &inputMap,
	tileStruct *tile, const int &largestSize) {

	int left = tile->column - 1; 
	int right = tile->column + 1;
	int up = tile->row - 1;
	int down = tile->row + 1; 

	
	//left
	if(left >= 0) {
		if(!visitedLocations[tile->row][left]) {
			visitedLocations[tile->row][left] = 1;
			tileStruct *tileLeft = new tileStruct;
			tileLeft->row = tile->row;
			tileLeft->column = left;
			tileLeft->value = &inputMap[tile->row][left];
			pqueue->push(tileLeft);
		}
	}

	//right
	if(right < largestSize) {
		if(!visitedLocations[tile->row][right]) {
			visitedLocations[tile->row][right] = 1;
			tileStruct *tileRight = new tileStruct;
			tileRight->row = tile->row;
			tileRight->column = right;
			tileRight->value = &inputMap[tile->row][right];
			pqueue->push(tileRight);
		}
	}

	//up
	if(up >= 0) {
		if(!visitedLocations[up][tile->column]) {
			visitedLocations[up][tile->column] = 1;
			tileStruct *tileUp = new tileStruct;
			tileUp->row = up;
			tileUp->column = tile->column;
			tileUp->value = &inputMap[up][tile->column];
			pqueue->push(tileUp);
		}
	}

	//down
	if(down < largestSize) {
		if(!visitedLocations[down][tile->column]) {
			visitedLocations[down][tile->column] = 1; 
			tileStruct *tileDown = new tileStruct;
			tileDown->row = down;
			tileDown->column = tile->column;
			tileDown->value = &inputMap[down][tile->column];
			pqueue->push(tileDown);
		}
	}
	return;
}

bool exitFound(const tileStruct *tile, const int &size) {
	if(*tile->value == 0) {
		if(tile->row == 0) {
			return true;
		}
		if(tile->column == 0) {
			return true;
		}
		if(tile->row == size-1) {
			return true;
		}
		if(tile->column == size -1) {
			return true;
		}
	}
	return false; 
}