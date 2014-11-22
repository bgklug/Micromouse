#include <stdio.h>
#include <math.h>

//----------//----------//DESCRIPTION//----------//----------//
/* The wall pattern of each cell follows a binary form where each bit 
 * is one side.  The compass directions in the binary number are ordered 
 * as follows from most significant to least significant bit:
 * WSEN
 * From this representation, the various wall patterns can be numbered 
 * like shown:                                                       N
 *      _       _       _       _       _       _       _       _  W   E
 *           |   |  _   _   _|  _| |   |   | | | | |_  |_  |_| |_|   S
 *  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
 * This program attempts to explore the possible functions that could 
 * be used to manipulate these numbers in ways that would correspond 
 * to changing a cell's walls.
 */

char wallSet(bool, bool, bool, bool);
bool wallExists(char, char);
char wallChange(char, char, bool);
char wallRotate(char, bool);

void main() {
	// wall existence test
	char identifier = wallSet(0, 0, 1, 1);
	printf("identifier = %d\n", identifier);
	for (char direction = 0; direction < 4; direction++) {
		printf("direction = %d\n", direction);
		printf("T/F? %d\n", wallExists(identifier, direction));
	}
	
	// wall change test (adding walls)
	identifier = wallSet(0, 0, 0, 1);
	printf("\nidentifier = %d\n", identifier);
	for (char direction = 0; direction < 4; direction++) {
		identifier = wallChange(identifier, direction, true);
		printf("identifier = %d\tdirection = %d\n", identifier, direction);
	}

	// wall change test (removing walls)
	identifier = wallSet(0, 1, 0, 1);
	printf("\nidentifier = %d\n", identifier);
	for (char direction = 0; direction < 4; direction++) {
		identifier = wallChange(identifier, direction, false);
		printf("identifier = %d\tdirection = %d\n", identifier, direction);
	}

	// cell rotation test (clockwise)
	identifier = wallSet(1, 0, 0, 1);
	printf("\nidentifier = %d\n", identifier);
	for (int i = 0; i < 4; i++) {
		identifier = wallRotate(identifier, true);
		printf("identifier = %d\trotated clockwise %d degrees\n", identifier, 90*(i + 1));
	}
}

char wallSet(bool north, bool east, bool south, bool west) {
	// Returns the decimal number represented by the inputs as boolean digits
	// The inputs are ordered from least to most significant bit
	return (char)(north + east*2 + south*4 + west*8);
}

bool wallExists(char identifier, char direction) {
	// Evaluates the boolean equivalent of 'identifier' for the digit specified by 'direction'
	// For direction: N = 0, E = 1, S = 2, W = 3
	for (char i = 0; i < direction; i++) {
		identifier = identifier/2;
	}
	if (identifier%2) {
		return true;
	}
	return false;
}

char wallChange(char identifier, char direction, bool addWall) {
	// Adds the value of 'addWall' to the digit 'direction' of the boolean equivalent of 'identifier'
	// For direction: N = 0, E = 1, S = 2, W = 3
	// Includes wall-add/-subtract redundancy checking
	//   e.g. wallChange(8, 3, true) and wallChange(8, 2, false) will both return 8
	if (addWall) {
		return identifier + (char)((double)!wallExists(identifier, direction)*pow(2, (double)direction));
	}
	return identifier - (char)((double)wallExists(identifier, direction)*pow(2, (double)direction));
}

char wallRotate(char identifier, bool clockwise) {
	// Shifts the boolean equivalent of 'identifier' right or left and carries in the bit pushed out
	// A clockwise rotation requires a shift left; a counter-clockwise rotation requires a shift right
	if (!clockwise) {
		return identifier/2 + (char)(8*(int)wallExists(identifier, 0));
	}
	return (identifier*2)%15;
}
