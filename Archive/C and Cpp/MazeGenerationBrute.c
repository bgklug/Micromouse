//Name: Random MM Maze
//Author: Dillon Mills
//Purpose: Generate a random maze to test MicroMouse logic.

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//----------//----------//NOTES//----------//----------//
//Takes too long to generate a maze, try different tactic
//May not need to set all walls to zero each time.

int main(void){
	//----------//----------//INITIALIZATION//----------//----------//
	int b[16][16][4];	//maze
	int c[16][16];		//flood (consider combining)
	int gen = 0;		//generation count
	int goodGen = 0;	//whether or not the maze is solvable
	clock_t tStart=clock(), tEnd=clock();	//starting and ending time
	
	//----------//----------//MAZE GENERATION//----------//----------//
	while(!goodGen){	//must generate a good map before continuing
		
		//prevents bad rand() seed where it makes every square the same
		int goodSeed = 0;
		if(gen){
			for(int i=0; i<15; i++){
				goodSeed += b[2][i][1];
			}
		}
		if(!goodSeed||(goodSeed==15)){ srand(clock()); }

		//----------//Walls//----------//
		//sets all walls to zero
		for(int i=0; i<16; i++){
			for(int j=0; j<16; j++){
				for(int k=0; k<4; k++){
					b[i][j][k] = 0;
				}
			}
		}
	
			//generate walls randomly
		for(int i=0; i<16; i++){		//consider changing to for(int i=0; i<256; i+=2){} and eliminate j
			for(int j=0; j<16; j++){
				for(int k=0; k<4; k++){
					//randomly generates every other grid space (checker board)
					if(i%2){
						if(j%2){
							b[i][j][k] = rand() % 2;
						}else{
							b[i][j][k] = 0;
						}
					}else{
						if(j%2){
							b[i][j][k] = 0;
						}else{
							b[i][j][k] = rand() % 2;
						}
					}
				}
			}
		}

		//makes the middle four squares have walls on all sides (one to be removed later)
		b[7][7][0] = 1;
		b[7][7][1] = 1;
		b[7][7][2] = 1;
		b[7][7][3] = 1;
		b[7][8][0] = 1;
		b[7][8][1] = 1;
		b[7][8][2] = 1;
		b[7][8][3] = 1;
		b[8][7][0] = 1;
		b[8][7][1] = 1;
		b[8][7][2] = 1;
		b[8][7][3] = 1;
		b[8][8][0] = 1;
		b[8][8][1] = 1;
		b[8][8][2] = 1;
		b[8][8][3] = 1;

		//randomly generates one opening to the middle
		int randWall = rand()%8;
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

		//makes sure each peg has at least one wall
		for(int i=0; i<15; i++){
			for(int j=0; j<15; j++){
				if(!(b[i][j][1]+b[i][j+1][2]+b[i+1][j][0]+b[i+1][j+1][3])){
					int randPeg = rand()%4;
					switch(randPeg){
					case 0: b[i][j][1] = 1; break;
					case 1: b[i][j+1][2] = 1; break;
					case 2: b[i+1][j][0] = 1; break;
					case 3: b[i+1][j+1][3] = 1; break;
					}
				}
			}
		}

		//if a grid space is missing a wall, it checks the next space to see if it sould have one.
		for(int i=0; i<16; i++){
			for(int j=0; j<16; j++){
				for(int k=0; k<4; k++){
					if(!b[i][j][0]){
						b[i][j][0] = b[i][j+1][2];
					}
					if(!b[i][j][1]){
						b[i][j][1] = b[i+1][j][3];
					}
					if(!b[i][j][2]){
						b[i][j][2] = b[i][j-1][0];
					}
					if(!b[i][j][3]){
						b[i][j][3] = b[i-1][j][1];
					}
				}
			}
		}

		//make the edges have walls
		for(int i=0; i<16; i++){
			b[0][i][3] = 1;
			b[15][i][1] = 1;
			b[i][0][2] = 1;
			b[i][15][0] = 1;
		}
	
		//----------//Flood//----------//
		//fills all flood array spaces with -1
		for(int i=0;i<16;i++){
			for(int j=0;j<16;j++){
				c[i][j] = -1;
			}
		}

		//fills the four goal flood array spaces with 0
 	 	c[7][7] = 0;
 	 	c[7][8] = 0;
 	 	c[8][7] = 0;
 	 	c[8][8] = 0;

	  	//fills the flood array with values using flood fill logic
	  	for(int k=0;k<256;k++){
	    	for(int i=0;i<16;i++){
	      		for(int j=0;j<16;j++){
	        		if(c[i][j]==k){  //if the flood array space equals k (starting at 0), place k+1 in adjacent flood array spaces
						if(j<15){
	          				if(!b[i][j+1][2] && (c[i][j+1]==-1)){  //North
	            				c[i][j+1] = c[i][j] + 1;
	     	     			}
						}
						if(j>0){
	          				if(!b[i][j-1][0] && (c[i][j-1]==-1)){  //South
	           					c[i][j-1] = c[i][j] + 1;
		          				}
					}
						if(i<15){
	          				if(!b[i+1][j][3] && (c[i+1][j]==-1)){  //East
	            				c[i+1][j] = c[i][j] + 1;
	          				}
						}
						if(i>0){
	          				if(!b[i-1][j][1] && (c[i-1][j]==-1)){  //West
	            				c[i-1][j] = c[i][j] + 1;
	          				}
						}
	        		}
	      		}
	    	}
	 	}
	
		//prints debugging info to console
		system("cls");
		tEnd = clock();
		printf("Time:\t\t%.2f\n", (double)(tEnd - tStart)/1000);
		printf("Generation:\t%d\n", gen++);
		printf("goodSeed:\t%d\n", goodSeed);
	
		
		printf(" _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _");
		for(int i=0; i<16; i++){
				printf("\n|");
			for(int j=0; j<16; j++){
				
				//visual representation
				if(b[i][j][1]){
					printf("_");}
				else{
					printf(" ");}
				if(b[i][j][0]){
					printf("|");}
				else{
					printf(" ");}
	
				//printf("%d ", b[i][j][0]+b[i][j][1]+b[i][j][2]+b[i][j][3]);	//wall count
	
				//printf("%d\t",c[i][j]);	//flood value
			}
		}

		if((c[0][0]!=-1)&&(c[0][15]!=-1)&&(c[15][0]!=-1)&&(c[15][15]!=-1)){goodGen = 1;}
	}
	
	//prints a good maze to console (visual representation, wall count per square, or flood values)
	printf(" _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _");
	for(int i=0; i<16; i++){
			printf("\n|");
		for(int j=0; j<16; j++){
			
			//visual representation
			if(b[i][j][1]){
				printf("_");}
			else{
				printf(" ");}
			if(b[i][j][0]){
				printf("|");}
			else{
				printf(" ");}

			//printf("%d ", b[i][j][0]+b[i][j][1]+b[i][j][2]+b[i][j][3]);	//wall count

			//printf("%d\t",c[i][j]);	//flood value
		}
	}
	printf("\n");

	return 0;
	
}
