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
			cout <<(char)( cells[i][j]->type + '0')<< ":" << cells[i][j]->timeLeft <<"\t";
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
					tmpCells[i][j]->age = 0;
					cells[i][j] = tmpCells[i][j];
				}
				else if(tmpCells[i][j]->age==0)
					cells[i][j] = tmpCells[i][j];
			}
			else
				cells[i][j] = tmpCells[i][j];
		}
}

void Colony::swapCells(int x1, int y1, int x2, int y2){
	Cell* tmp = cells[x1][y1];
	cells[x1][y1] = cells[x2][y2];
	cells[x2][y2] = tmp;
}

void Colony::move(){
	cout << "Moviendo..."<<endl;
	int counters[3];
	vector< Cell* > neighborhood;
	neighborhood.resize(4);
	srand (time(NULL));
	for(int i=rand()%2; i<rows-1; i+=2){
		for(int j=rand()%2; j<cols-1; j+=2){
			cout << "Procesando cuadro ("<< i << "," << j<<"),("<< i+1<<", "<<j+1<<")"<<endl; 
			counters[BLANK] = 0;
			counters[BACTERIUM] = 0;
			counters[FOOD] = 0;
			counters[cells[i][j]->type]++;
			counters[cells[i][j+1]->type]++;
			counters[cells[i+1][j]->type]++;
			counters[cells[i+1][j+1]->type]++;
			cout << "Hay "<<counters[BLANK]<<" blancos, "<<counters[BACTERIUM]<<"Bacterias, "<<counters[FOOD]<<" Nutrientes"<<endl;
			if(counters[BACTERIUM]>0){
				if(counters[FOOD]==0){
					cout << "No hay comida, pero si bacterias, rotando..."<<endl;
					neighborhood[0] = cells[i][j];
					neighborhood[1] = cells[i][j+1];
					neighborhood[2] = cells[i+1][j+1];
					neighborhood[3] = cells[i+1][j];
					if(rand()%100<50){
						//Rotar a la izquierda
						cells[i][j] = neighborhood[1];
						cells[i][j+1] = neighborhood[2];
						cells[i+1][j] = neighborhood[0];
						cells[i+1][j+1] = neighborhood[3];
					}
					else{						
						//Rotar a la derecha
						cells[i][j] = neighborhood[3];
						cells[i][j+1] = neighborhood[0];
						cells[i+1][j+1] = neighborhood[1];
						cells[i+1][j] = neighborhood[2];
					}
				}
				else if(counters[FOOD] == 1){
					if(counters[BACTERIUM]==1){
						//Hacer que coincida fila o columna
						//Hay 4 casos
						//El movimiento horizontal tiene una probabilidad igual al vertical
						cout << "Una bacteria y una comida, moviendo..."<<endl;
						if(cells[i][j]->type==BACTERIUM && cells[i+1][j+1]->type==FOOD){
							if(rand()%100<50)
								swapCells(i, j, i, j+1);
							else
								swapCells(i, j, i+1,j);
						}
						else if(cells[i][j]->type==FOOD && cells[i+1][j+1]->type==BACTERIUM){
							if(rand()%100<50)
								swapCells(i+1, j+1, i+1, j);
							else
								swapCells(i+1, j+1, i, j+1);
						}
						else if(cells[i][j+1]->type==BACTERIUM && cells[i+1][j]->type==FOOD){
							if(rand()%100 < 50)
								swapCells(i, j+1, i, j);
							else 
								swapCells(i, j+1, i+1, j+1);
						}
						else if(cells[i][j+1]->type==FOOD && cells[i+1][j]->type==BACTERIUM){
							if(rand()%100<50)
								swapCells(i+1, j, i+1, j+1);
							else
								swapCells(i+1, j, i, j);
						}
					}
					else if(counters[BACTERIUM]==2){
						cout << "Dos bacterias y un nutriente, moviendo"<<endl;
						//Hacer que ambas queden a los lados
						if(cells[i][j]->type==FOOD && cells[i+1][j+1]->type==BACTERIUM){
								if(cells[i][j+1]->type==BACTERIUM)
									swapCells(i+1, j+1, i+1, j);
								else 
									swapCells(i+1, j+1, i, j+1);
						}
						else if(cells[i][j+1]->type==FOOD && cells[i+1][j]->type==BACTERIUM){
								if(cells[i][j]->type==BACTERIUM)
									swapCells(i+1, j, i+1, j+1);
								else 
									swapCells(i+1, j, i, j);
						}
						else if(cells[i+1][j]->type==FOOD && cells[i][j+1]->type==BACTERIUM){
								if(cells[i+1][j+1]->type==BACTERIUM)
									swapCells(i, j+1, i, j);
								else 
									swapCells(i, j+1, i+1, j+1);
						}
						else if(cells[i+1][j+1]->type==FOOD && cells[i][j]->type==BACTERIUM){
								if(cells[i+1][j]->type==BACTERIUM)
									swapCells(i, j, i , j+1);
								else 
									swapCells(i, j, i+1, j);
						}
					}
				}
			}
		}
	}
}