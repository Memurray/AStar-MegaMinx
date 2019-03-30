/*
Author: "please do not put your name in the files"
Help: Modified my heuristic to be 4 turns for opposide face edge pieces based on discussion board communication with Adrian Wyllie.
Date: 9/15/18

Run instructions: run Make from command line. Creates executable AStarMegaMinx
Execute with ./AStarMegaMinx
Below will be 3 user options
1. q to quit
2. k<randomize steps> ex. k10 will solve a megaMinx with 10 random rotations applied
3. f<randomize steps> ex.f10 will solve 5 different megaMinxs with for each k value between 3 and 10 rotations applied.
*/

#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <math.h> 
#include <vector>
#include <queue>

using namespace std;

// For making creating multiple instances of the megaMinx option for the next programming assignment
// I decided I should just make the megaMinx an object. So I just wrapped up my functions inside of a class.
class megaMinx
{

public:
	uint8_t mm[12][15];  //megaMinx core data Structure
	uint8_t heuristic = 100;  // initialized to something unrealistically large
	uint8_t gScore = 0;  //depth = 0;
	uint8_t createdTurn = 100;  //initialized to an invalid rotation ([0-23] = valid)
	
	// If no arguments, megaMinx initialized to default solved state
	megaMinx() {
		uint8_t baseFaces[12][5] = //manually setting all the adjacent faces in clockwise order
		{
			{ 1,5,4,3,2 },
			{ 5,0,2,9,10 },
			{ 1,0,3,8,9 },
			{ 2,0,4,7,8 },
			{ 3,0,5,11,7 },
			{ 0,1,10,11,4 },
			{ 7,11,10,9,8 },
			{ 11,6,8,3,4 },
			{ 7,6,9,2,3 },
			{ 8,6,10,1,2 },
			{ 9,6,11,5,1 },
			{ 10,6,7,4,5 }
		};

		for (int i = 0; i < 12; i++)
			for (int j = 0; j < 15; j++)
				if (j<10)	//fill current row with row number up to column 9
					mm[i][j] = i;
				else		//fill remaining columns with adjacent side info
					mm[i][j] = baseFaces[i][j - 10];
	}

	// UNUSED FUNCTION
	// If input array, megaMinx initialized to match config of input 2D array
	megaMinx(uint8_t inputArray[][15]) {
		for (int i = 0; i < 12; i++)
			for (int j = 0; j < 15; j++)
				mm[i][j] = inputArray[i][j];
	}

	// Sets the data structure to input pointer to megaMinx object
	// Element wise copy contents
	void set(megaMinx * old) {
		for (int i = 0; i < 12; i++)
			for (int j = 0; j < 15; j++)
				this->mm[i][j] = old->mm[i][j];
	}

	//prints out the gui using single letters representing colors,since the sides are stored as int, char command is used plus ascii offset to print the desired letter.
	void guiPaint(int face) {
		cout << "        " << char(mm[face][10] + 65) << "\n\n";
		cout << " " << char(mm[face][14] + 65) << "    " << char(mm[face][0] + 65) << " " << char(mm[face][1] + 65) << " " << char(mm[face][2] + 65) << "    " << char(mm[face][11] + 65) << "\n";
		cout << "     " << char(mm[face][9] + 65) << "     " << char(mm[face][3] + 65) << " \n";
		cout << "    " << char(mm[face][8] + 65) << "   " << char(face + 65) << "   " << char(mm[face][4] + 65) << " \n";
		cout << "      " << char(mm[face][7] + 65) << "   " << char(mm[face][5] + 65) << " \n";
		cout << "        " << char(mm[face][6] + 65) << " \n";
		cout << "  " << char(mm[face][13] + 65) << "           " << char(mm[face][12] + 65) << "\n";
	}

	//GUI variant with only numbers, not used in main, was helpful for some testing
	void guiPaintNum(int face) {
		cout << "        " << mm[face][10] + 1 << "\n\n";
		cout << " " << mm[face][14] + 1 << "    " << mm[face][0] + 1 << " " << mm[face][1] + 1 << " " << mm[face][2] + 1 << "    " << mm[face][11] + 1 << "\n";
		cout << "     " << mm[face][9] + 1 << "     " << mm[face][3] + 1 << " \n";
		cout << "    " << mm[face][8] + 1 << "   " << face + 1 << "   " << mm[face][4] + 1 << " \n";
		cout << "      " << mm[face][7] + 1 << "   " << mm[face][5] + 1 << " \n";
		cout << "        " << mm[face][6] + 1 << " \n";
		cout << "  " << mm[face][13] + 1 << "           " << mm[face][12] + 1 << "\n";
	}

	//rotates assigned face with arguements
	//baseFace = face to rotate, direction: 1 = CW	 0 = CCW
	void faceRotate(int baseFace, int direction) {
		if (direction == 0 || direction == 1) {
			int targetStart[5] = { -1,-1,-1,-1,-1 };
			int targetFace[5] = { -1,-1,-1,-1,-1 };
			int old[15];
			for (int i = 0; i < 5; i++) {
				targetFace[i] = mm[baseFace][10 + i]; //captures the faces adjacent to the face chosen for rotation
				for (int j = 0; j < 5; j++) {
					if (mm[targetFace[i]][10 + j] == baseFace) {  //searches for the rotation face inside of the adjacent faces adjacencies
						targetStart[i] = j * 2;  //based on its position in the adjacency, the location of the color information can be derived
						break;
					}
				}
			}
			//saves the 15 elements color that ring the rotated face to make moving them easier
			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 3; j++)
					old[i * 3 + j] = mm[targetFace[i]][(targetStart[i] + j) % 10];//modulo 10 is important because element 10 is actually element 0 as the color data is a ring

																				  // using 15 element ring information, overwrites the 3 elements on each of the 5 adjacent faces to the one being rotated.
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 3; j++) {
					if (direction)  //overwrite equation is different depending on the direction of rotation
						mm[targetFace[i]][(targetStart[i] + j) % 10] = old[(i * 3 + j + 12) % 15];
					else
						mm[targetFace[i]][(targetStart[i] + j) % 10] = old[(i * 3 + j + 3) % 15];
				}
			}

			// changes the face adjacencies on the rotated face, direction of changed based on direction of rotation. 
			//I think of it as face rotation relative to rest matches CW or CCW rotation but in physical space, the rotated face remains in the same place and the rest of the megaminx rotates. I felt this was easier for me to handle.
			for (int i = 0; i < 5; i++) {
				if (direction)
					mm[baseFace][10 + i] = targetFace[(i + 1) % 5];
				else
					mm[baseFace][10 + i] = targetFace[(i + 4) % 5];
			}
		}
		else {
			cout << "faceRotate call error, direction not 0 or 1.\n";
		}
	}

	//takes a number of moves and runs a number of faceRotate commands equal to this number, generating a new face to rotate and direction with each move.
	void randomize(int moves) {
		int lastFace = -1;
		int lastDirection = -1;
		for (int i = 0; i < moves; i++) {
			int face = rand() % 12;
			int direction = rand() % 2;
			while (face == lastFace && direction != lastDirection) {
				face = rand() % 12;
				direction = rand() % 2;
			}
			faceRotate(face, direction);
			lastFace = face;
			lastDirection = direction;
		}
	}

	// sum of all stickers for equation stickers*turnDisplacement, all divided by 15.
	uint8_t testHeuristic() {
		double hReturn = 0.0;  //division required before return, felt safest to use a floating point value for tracking.
		uint8_t opposite[12] = { 6,7,11,10,9,8,0,1,5,4,3,2 };  // mapping of what side is opposite to index side, ex. opposite[0]=6 means sides 0 and sides 6 are opposite of each other
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 10; j++) {
				if (mm[i][j] == i)  //if stick is on right face, turns away is 0 for that sticker
					continue;
				else if (mm[i][j] == opposite[i]){  // if sticker on opposite face then edges are 4 turns away, corners are 3 turns away.
					if(j%2 == 1)
						hReturn = hReturn + 4.0;
					else
						hReturn = hReturn + 3.0;
					}
				else if (mm[i][j] == mm[i][10] || mm[i][j] == mm[i][11] || mm[i][j] == mm[i][12] || mm[i][j] == mm[i][13] || mm[i][j] == mm[i][14])  //if sticker is on a neighbor to the correct side, 1 turn away.
					hReturn = hReturn + 1.0;
				else //tested all sides other than exclusive neighbor of neighbor, 2 turns away.
					hReturn = hReturn + 2.0;
			}
		}
		return ceil(hReturn / 15);
	}
	// combo function to make repetitive actions easier, configures all variables for new child
	void setAll(megaMinx * old, int rotateKey) {
		set(old);  //copy 2-d array values from old to this megaMinx object
		this->createdTurn = rotateKey;  //save rotation used to create this child
		this->faceRotate(floor(rotateKey / 2), rotateKey % 2);  //apply rotation to this object so it differs from the parent
		this->heuristic = testHeuristic();  //generate the heuristic value
		this->gScore = old->gScore+1;  //depth of gScore is 1 more than parent
	}
};

//work around for getting press enter to continue on linux
void printError() {
	cout << "Invalid Input.\n";
	cout << "Press Enter to continue";
	cin.ignore();
	cin.get();
}

// same as print error function just without extra text saying there was an error, exists just to make program pause, linux compat.
void wait() {
	cout << "Press Enter to continue";
	cin.ignore();
	cin.get();
}

// Setting up comparator class to assist priority queue
// m1 > m2 results in top item in queue being the lowest fScore (this value is derived from sum of gScore and heuristic).
struct CompareF {
	bool operator()(megaMinx * m1, megaMinx * m2) {
		// op is true if m1's fScore is greater than m2 fScore (this value is derived from sum of gScore and heuristic).
		return (m1->gScore + m1->heuristic) > (m2->gScore + m2->heuristic);
	}
};


void solve(megaMinx * old) {
	int nodesExpanded = 1;	// makes the assumption solvers isnt given solved megaMinx
	priority_queue<megaMinx*, vector<megaMinx*>, CompareF> mmPQueue;  //priority queue of pointers to megaMinx controlled by comparator class ensuring smalls fScore is at the top of the queue
	for(int i=0; i < 24; i++){  //immediatly expands input megaMinx, checking all 24 possible next moves
		if(floor(old->createdTurn/2) != floor(i / 2) || old->createdTurn % 2 == i%2){  //ensures planned move doesnt undo the previous move, though for first expansion this check really isnt needed.
			megaMinx * p = new megaMinx;  //pointer to new megaMinx on heap
			p -> setAll(old,i);	  //configure this child
			mmPQueue.push(p); // push to queue
		}		
	}
	while(true){  //core loop, ideally loops until megaMinx solved
		megaMinx * testM = mmPQueue.top();  //point to item at top of the queue
		if (testM->heuristic == 0 || nodesExpanded > 4000000) {  //check if this node to be popped is the solution, or we've exceeded a safety threshold (this is set based on my personal computer's ram size)
			if(testM->heuristic == 0) //if node is solution, print information about the run
				cout << "Expanded: " << nodesExpanded << "    Distance: " << (int)testM->gScore <<  endl;
			else { //if too many nodes are expanded, print basic error, let user read it, kill program
				cout << "Too big\nExiting program\n";
				wait();
				exit(EXIT_FAILURE);
			}
			while (!mmPQueue.empty()) {  //for the purposes of successive runs without closing the program I need to clear the queue otherwise ram usages is compounded
				megaMinx * delMM = mmPQueue.top();  //save pointer to top
				mmPQueue.pop();  //removed top from queue (in the process setting next to top)
				delete(delMM);}	// delete old top
			break; //exit "infinite" loop
		}
		else{  //if exit condition not met
			nodesExpanded ++;  //counts as a new expanded node
			for (int i = 0; i < 24; i++) {  //generated all childern for parent
				if (floor(testM->createdTurn / 2) != floor(i / 2) || testM->createdTurn % 2 == i % 2) {  //check to see which one of the 24 children doesnt need to be created (one that undoes parent rotation)
					megaMinx * p = new megaMinx;  //add new mm pointer to heap
					p->setAll(testM, i);  //configure child
					mmPQueue.push(p); //push to queue
				}
			}
			mmPQueue.pop();  //remove parent from queue
			delete(testM);  //delete expanded node
		}
	}
}

int main() {
	srand(time(NULL)); 
	megaMinx mm1;
	string userInput;
	int storedFace = 0;
	//system("CLS");

	// repeats options display until entering q breaks loop and program ends.
	while (true) {
		//mm1.guiPaint(storedFace);
		
		cout << "Type q to quit\nor Type k<x> to solve rand(x) megaMinx ex.[k10] \nor Type f<x> to solve 5 rand megaMinx from 3 to x number of turns: ";
		cin >> userInput;
		if (userInput[0] == 'r' && userInput.length() > 1) {  //check for a user randomization command
			int parsedInput = stoi(userInput.substr(1, userInput.length() - 1));
			if (parsedInput >= 0){
				mm1.randomize(parsedInput);
			}
		}
		else if (userInput[0] == 'k' && userInput.length() > 1) {  //run solve for k number of randomizations
			int parsedInput = stoi(userInput.substr(1, userInput.length() - 1));
			if (parsedInput >= 0) {
				megaMinx mmTest; //make a fresh new megaMinx object
				mmTest.randomize(parsedInput); // randomize with input number of randomizations
				solve(&mmTest);
				//wait();
			}
		}

		else if (userInput[0] == 'f' && userInput.length() > 1) {  //run solve for 3 to k number of randomizations, 5 times each
			int parsedInput = stoi(userInput.substr(1, userInput.length() - 1));
			if (parsedInput >= 3) {
				for (int i = 3; i <= parsedInput; i++) { //for each k from 3 to input
					cout << "K = " << i << " Tests" << endl;
					for (int j = 0; j < 5; j++) {  //run 5 solves
						megaMinx mmTest; //make fresh megaMinx
						mmTest.randomize(i); //randomize it
						solve(&mmTest); //solve it
					}
				}
			}
			//wait();
		}

		else if (userInput[0] == 'r' && userInput.length() == 1) {
			printError();
		}
		else if (userInput[0] == 'q')  // checks for a quit command
			break;
		else if (char(userInput[0]) >= 49 && char(userInput[0]) <= 57) {  //check for a 1-12 input for face to inspect, since cin reads a string, I check ascii codes of first character to ensure it's a 1-9 value
			if (stoi(userInput) >= 1 && stoi(userInput) <= 12){
				storedFace = stoi(userInput) - 1;
				mm1.guiPaint(storedFace);
			}
			else {
				printError();
			}
		}
		else {
			printError();
		}
		//system("CLS");
		cout << endl;
	}
	return 0;
}
