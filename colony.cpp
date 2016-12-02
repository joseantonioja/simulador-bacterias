#include "colony.h"
#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>
Cell::Cell(char t, int initialTime){
	type = t;
	timeLeft = initialTime;
	age = 0;
	if(type == BACTERIUM)
		canBreed = true;
	else
		canBreed = false;
} 
Cell::Cell(const Cell& c){
	type = c.type;
	timeLeft = c.timeLeft;
	age = c.age;
	canBreed = c.canBreed;
}
//probFood + probBact < 1
Colony::Colony(int r, int c, char bl, char bf, double probFood, double probBact, int initialTime){
	breedLapse = bl;
	boostFeeding = bf;
	rows = r;
    cols = c;
    initialTl = initialTime;
    cells.resize(rows);
    for(int i=0; i<rows; i++)
    	cells[i].resize(cols);
    if(probFood>=0){
    	srand (time(NULL));
    	for(int i=0; i<rows; i++)
    		for(int j=0; j<cols; j++){
    			double randVal = (rand()%100)/100.0;
    			if(randVal <= probBact)
    				cells[i][j] = new Cell(BACTERIUM, initialTime);
    			else if(randVal<=probBact + probFood )
    				cells[i][j] = new Cell(FOOD, 0);
    			else
    				cells[i][j] = new Cell(BLANK, 0);
    		}
    }
    else{
    	for(int i=0; i<rows; i++)
    		for(int j=0; j<cols; j++)
    			cells[i][j] = new Cell(BLANK, 0);
    }
}

void Colony::printCells(){
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			cout <<(char)( cells[i][j]->type + '0')<< ":" << cells[i][j]->timeLeft << "\t";
		}
		cout << endl;
	}
}

Cell* Colony::getDyingCell(vector<Cell*> a){
	int minT = 100000;
	Cell* tmp = NULL;
	srand (time(NULL));
	if(rand()%100 < 50)
		for(int i=0; i<4; i++){
			if(a[i]!=NULL)
				if(minT>a[i]->timeLeft){
					minT = a[i]->timeLeft;
					tmp = a[i];
				}
		}
	else
		for(int i=3; i>=0; i--){
			if(a[i]!=NULL)
				if(minT>a[i]->timeLeft){
					minT = a[i]->timeLeft;
					tmp = a[i];
				}
		}
	return tmp;
}

Cell* Colony::getBreedingCell(vector<Cell*> a){
	srand (time(NULL));
	if(rand()%100 < 50){
		for(int i=0; i<4; i++)
			if(a[i]!=NULL)
				if(a[i]->canBreed && a[i]->age%breedLapse==0)
					return a[i];
	}
	else{
		for(int i=3; i>=0; i--)
			if(a[i]!=NULL)
				if(a[i]->canBreed && a[i]->age%breedLapse==0)
					return a[i];
	}
	return NULL;
}


void Colony::eatAndBreed(){
	vector< vector<Cell*> > tmpCells;
	tmpCells.resize(rows);
	for(int i=0; i<rows; i++)
		tmpCells[i].resize(cols);
	vector<Cell*> neighborhood;
	neighborhood.resize(4);
	int x, y;
	for(int i=0; i<rows; i++)
		for(int j=0; j<cols; j++){
			if(cells[i][j]->type==BACTERIUM){
				cells[i][j]->canBreed = true;
				cells[i][j]->age += 1;
				cells[i][j]->timeLeft -= 1;
			}
			tmpCells[i][j] = new Cell(*cells[i][j]);
		}
	//Cells inside the space
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			if(cells[i][j]->type == BLANK){
				neighborhood[0] = NULL; neighborhood[1] = NULL; neighborhood[2] = NULL; neighborhood[3] = NULL; 
				if(i>0)
					if(cells[i-1][j]->type == BACTERIUM)
						neighborhood[0] = cells[i-1][j];
				if(i<rows-1)
					if(cells[i+1][j]->type == BACTERIUM)
						neighborhood[1] = cells[i+1][j];
				if(j>0)
					if(cells[i][j-1]->type == BACTERIUM)
						neighborhood[2] = cells[i][j-1];
				if(j<cols - 1)
					if(cells[i][j+1]->type == BACTERIUM)
						neighborhood[3] = cells[i][j+1];
				Cell *tmp = getBreedingCell(neighborhood);
				if(tmp!=NULL){
					tmp->canBreed = false;
					tmpCells[i][j]->type = BACTERIUM;
					tmpCells[i][j]->age = 0;
					tmpCells[i][j]->timeLeft = initialTl;
				}
			}
			else if(cells[i][j]->type == FOOD){
				neighborhood[0] = NULL; neighborhood[1] = NULL; neighborhood[2] = NULL; neighborhood[3] = NULL; 
				Cell *tmp = NULL;
				if(i>0)
					if(cells[i-1][j]->type == BACTERIUM)
						neighborhood[0] = cells[i-1][j];
				if(i<rows-1)
					if(cells[i+1][j]->type == BACTERIUM)
						neighborhood[1] = cells[i+1][j];
				if(j>0)
					if(cells[i][j-1]->type == BACTERIUM)
						neighborhood[2] = cells[i][j-1];
				if(j<cols - 1)
					if(cells[i][j+1]->type == BACTERIUM)
						neighborhood[3] = cells[i][j+1];
				tmp = getDyingCell(neighborhood);
				if(tmp!=NULL){
					tmp->timeLeft+=boostFeeding;
					tmpCells[i][j]->type = BLANK;
				}
			}
		}
	}
	for(int i=0; i<rows; i++)
		for(int j=0; j<cols; j++){
			if(tmpCells[i][j]->type==BACTERIUM){
				if(tmpCells[i][j]->timeLeft==0){
					tmpCells[i][j]->type = FOOD;
					cells[i][j] = tmpCells[i][j];
				}
				else if(tmpCells[i][j]->age==0)
					cells[i][j] = tmpCells[i][j];
			}
			else
				cells[i][j] = tmpCells[i][j];
		}
}
