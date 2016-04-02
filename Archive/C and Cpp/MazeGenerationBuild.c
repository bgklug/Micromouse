//Name: MM Maze - Build
//Author: Dillon Mills
//Purpose: Randomly build a 16x16 maze to test MicroMouse algorithms.

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int randint();

//----------//----------//NOTES//----------//----------//

int main(void)
{
	//----------//----------//INITIALIZATION//----------//----------//
	int b[16][16][4];	//maze
	int c[16][16];		//flood (consider combining with maze b[][][])
	int gen = 0;		//generation count
	int goodGen = 0;	//whether or not the maze is solvable
	clock_t tStart=clock(), tEnd=clock();	//starting and ending time
	srand(clock());    
	
	//----------//----------//MAZE GENERATION//----------//----------//

	//----------//Constants//----------//
	//manually places constants into the maze array

	//Clean Slate//
	//starts the maze without having any walls anywhere
	for(int i=0; i<=16; i++){
		for(int j=0; j<=16; j++){
			for(int k=0; k<4; k++){
				b[i][j][k] = 0;
			}
		}
	}

	//Center Walls//
	//makes the center squares have walls on all sides
	for(int i=7; i<=8; i++){
		for(int j=7; j<=8; j++){
			for(int k=0; k<4; k++){
				b[i][j][k] = 1;
			}
		}
	}

	//Center Opening//
	//randomly generates one opening to the center
	int randWall = randint()%8;
	switch(randWall){
		case 0: b[7][8][0] = 0; break;
		case 1: b[8][8][0] = 0; break;
		case 2: b[8][7][1] = 0; break;
		case 3: b[8][8][1] = 0; break;
		case 4: b[7][7][2] = 0; break;
		case 5: b[8][7][2] = 0; break;
		case 6: b[7][7][3] = 0; break;
		case 7: b[7][8][3] = 0; break;
	}

	//Corners//
	//places the walls in the four corners
	b[0][0][0] = 0;
	b[0][0][1] = 1;
	b[0][1][2] = 0;
	b[1][0][3] = 1;
	b[0][15][1] = 0;
	b[0][15][2] = 1;
	b[0][14][0] = 1;
	b[1][15][3] = 0;
	b[15][0][0] = 1;
	b[15][0][3] = 0;
	b[15][1][2] = 1;
	b[14][0][1] = 0;
	b[15][15][2] = 0;
	b[15][15][3] = 1;
	b[15][14][0] = 0;
	b[14][15][1] = 1;

	//Boarder//
	//make the edges have walls
	for(int i=0; i<16; i++){
		b[0][i][3] = 1;
		b[15][i][1] = 1;
		b[i][0][2] = 1;
		b[i][15][0] = 1;
	}

	for(int i=0; i<16; i++){
		for(int j=0; j<16; j++){
			goodGen = 0;
			while(!goodGen){	//makes sure maze is solvable

				//----------//Seed//----------//
				//prevents bad rand() seed where it makes every square the same
				

				//----------//Walls//----------//
				//build a random maze
				
				//Checkerboard//
				//only generates every other grid space
				for(int k=0; k<4; k++){
					if(i%2){
						if(j%2){
							b[i][j][k] = randint() % 2;
						}else{
							b[i][j][k] = 0;
						}
					}else{
						if(j%2){
							b[i][j][k] = 0;
						}else{
							b[i][j][k] = randint() % 2;
						}
					}
				}

	//----------//Constants//----------//
	//This was placed here carelessly to make sure the constants stay as they should be without having to be creative
	
	//Center Walls//
	//makes the center squares have walls on all sides
	for(int k=7; k<=8; k++){
		for(int l=7; l<=8; l++){
			for(int m=0; m<4; m++){
				b[k][l][m] = 1;
			}
		}
	}

	//Center Opening//
	//places center walls
	switch(randWall){
		case 0: b[7][8][0] = 0; break;
		case 1: b[8][8][0] = 0; break;
		case 2: b[8][7][1] = 0; break;
		case 3: b[8][8][1] = 0; break;
		case 4: b[7][7][2] = 0; break;
		case 5: b[8][7][2] = 0; break;
		case 6: b[7][7][3] = 0; break;
		case 7: b[7][8][3] = 0; break;
	}

	//Corners//
	//places the walls in the four corners
	b[0][0][0] = 0;
	b[0][0][1] = 1;
	b[0][1][2] = 0;
	b[1][0][3] = 1;
	b[0][15][1] = 0;
	b[0][15][2] = 1;
	b[0][14][0] = 1;
	b[1][15][3] = 0;
	b[15][0][0] = 1;
	b[15][0][3] = 0;
	b[15][1][2] = 1;
	b[14][0][1] = 0;
	b[15][15][2] = 0;
	b[15][15][3] = 1;
	b[15][14][0] = 0;
	b[14][15][1] = 1;

	//Boarder//
	//make the edges have walls
	for(int k=0; k<16; k++){
		b[0][k][3] = 1;
		b[15][k][1] = 1;
		b[k][0][2] = 1;
		b[k][15][0] = 1;
	}
/*
				//Pegs//
				//places a wall on every peg
				if((i>0)&&(j>0)){
					if(!(b[i][j][3]+b[i][j-1][2]+b[i-1][j][0]+b[i-1][j-1][1])){
						int randPeg = randint()%4;
						switch(randPeg){
							case 0: b[i][j][1] = 1; break;
							case 1: b[i][j-1][2] = 1; break;
							case 2: b[i-1][j][0] = 1; break;
							case 3: b[i-1][j-1][3] = 1; break;
						}
					}
				}
*/

				//Check//
				//changes the values of adjacent walls
				for(int k=0; k<16; k++){
					for(int l=0; l<16; l++){
						if(k>0){
							b[k-1][l][0] = b[k][l][2];
						}
						if(k<15){
							b[k+1][l][2] = b[k][l][0];
						}
						if(l>0){
							b[k][l-1][1] = b[k][l][3];
						}
						if(l<15){
							b[k][l+1][3] = b[k][l][1];
						}
					}
				}
		

				//----------//Flood//----------//
				//test random maze with flood logic

				//Reset//
				//fills all flood array spaces with -1
				for(int k=0;k<16;k++){
					for(int l=0;l<16;l++){
						c[k][l] = -1;
					}
				}

				//Center//
				//fills the four goal flood array spaces with 0
 			 	c[7][7] = 0;
 			 	c[7][8] = 0;
 			 	c[8][7] = 0;
 			 	c[8][8] = 0;
		
				//Floodfill//
			  	//fills the flood array with values using flood fill logic
			  	for(int l=0;l<256;l++){
			    	for(int m=0;m<16;m++){
	 		     		for(int n=0;n<16;n++){
	  			      		if(c[m][n]==l){  //if the flood array space equals k (starting at 0), place l+1 in adjacent flood array spaces
								if(n<15){
	     		     				if(!b[m][n+1][2] && (c[m][n+1]==-1)){  //North
	      			      				c[m][n+1] = c[m][n] + 1;
	     	  			   			}
								}
								if(n>0){
	      	    					if(!b[m][n-1][0] && (c[m][n-1]==-1)){  //South
	           							c[m][n-1] = c[m][n] + 1;
		          					}
								}
								if(m<15){
	          						if(!b[m+1][n][3] && (c[m+1][n]==-1)){  //East
	            						c[m+1][n] = c[m][n] + 1;
	          						}
								}
								if(m>0){
	          						if(!b[m-1][n][1] && (c[m-1][n]==-1)){  //West
	            						c[m-1][n] = c[m][n] + 1;
	          						}
								}
	        				}
	      				}
	    			}
	 			}
				
				//Check//
				//checks if all four corners can reach the center
				if((c[0][0]!=-1)&&(c[0][15]!=-1)&&(c[15][0]!=-1)&&(c[15][15]!=-1)){
					goodGen = 1;
				}
			

				//----------//Debugging//----------//
				//code to help debug
	
				//Print Info//
				//prints time elapsed, generation count, and goodSeed value to console
				system("cls");
				tEnd = clock();
				printf("Time:\t\t%.2f\n", (double)(tEnd - tStart)/1000);
				printf("Generation:\t%d\n", gen++);
				
				//Print Maze//
				//prints visual representation of maze

		printf(" _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _");
		for(int i=0; i<16; i++){
				printf("\n|");
			for(int j=0; j<16; j++){
/*
				//visual representation
				if(b[i][j][1]){
					printf("_");}
				else{
					printf(" ");}
				if(b[i][j][0]){
					printf("|");}
				else{
					printf(" ");}
*/
				//printf("%d ", b[i][j][0]+b[i][j][1]+b[i][j][2]+b[i][j][3]);	//wall count
	
				printf("%d\t",c[i][j]);	//flood value
			}
		}
			}
		}
	}
	return 0;
}

int randint(){
	static i =1;
	i++;
	srand(clock()+i);
	return rand();
}
