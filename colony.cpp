#include "colony.h"

Cell::Cell(char t, int initialTime){
    type = t;
    energy = initialTime;
    age = 0;
    canEat = false;
    canBreed = false;
    if(t==TOXIN)
        canPoison = true;
    else
        canPoison = false;
}
Cell::Cell(const Cell& c){
    type = c.type;
    energy = c.energy;
    age = c.age;
    canBreed = c.canBreed;
    canEat = c.canEat;
    canPoison = c.canPoison;
}
//probFood + probBact < 1
Colony::Colony(int r, int c, int bl, int bf, double probFood, double probBact, double probToxin, int initialTime, int td , int tt, int me, int el){
    breedLapse = bl;
    boostFeeding = bf;
    rows = r;
    cols = c;
    offset = 0;
    toxicTime = tt;
    toxinDamage = td;
    initialTl = initialTime;
    minEnergy = me;
    energyLostIteration = el;
    gen = 0;
    counters[BLANK] = 0; counters[BACTERIUM] = 0; counters[FOOD] = 0; counters[TOXIN] = 0;
    cout << "Tiempo de generacion "<<breedLapse<<endl;
    cout << "Incremento energetico "<<boostFeeding<<endl;
    cout << "Filas "<< rows<<endl;
    cout << "Columnas "<<cols<<endl;
    cout << "Lapso de toxinas "<<toxicTime<<endl;
    cout << "Daño por toxina "<<toxinDamage<<endl;
    cout << "Energia inicial "<<initialTime<<endl;
    cout << "Energía mínima para reproducción "<<minEnergy<<endl;
    cells.resize(rows);
    for(int i=0; i<rows; i++)
        cells[i].resize(cols);
    srand (time(NULL));
    if(probFood>=0){
        for(int i=0; i<rows; i++)
            for(int j=0; j<cols; j++){
                double randVal = (rand()%100)/100.0;
                if(randVal < probBact){
                    cells[i][j] = new Cell(BACTERIUM, initialTime);
                    counters[BACTERIUM]++;
                }
                else if(randVal<probBact + probFood ){
                    cells[i][j] = new Cell(FOOD, 0);
                    counters[FOOD]++;
                }
                else if(randVal < probBact + probFood + probToxin){
                    cells[i][j] = new Cell(TOXIN, 0);
                    counters[TOXIN]++;
                }
                else{
                    cells[i][j] = new Cell(BLANK, 0);
                    counters[BLANK]++;
                }
            }
    }
    else{
        counters[BLANK] = rows*cols;
        for(int i=0; i<rows; i++)
            for(int j=0; j<cols; j++)
                cells[i][j] = new Cell(BLANK, 0);
    }
    cout << "Recuento: "<<endl;
    cout << counters[BACTERIUM] << " bacterias"<<endl;
    cout << counters[TOXIN] << " toxinas"<<endl;
    cout << counters[FOOD] << " nutrientes"<<endl;
}

void Colony::printCells(){
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            cout <<(char)( cells[i][j]->type + '0')<< ":" << cells[i][j]->energy <<"\t";
        }
        cout << endl;
    }
}

Cell* Colony::getDyingCell(vector<Cell*> a){
    int minT = 100000;
    Cell* tmp = NULL;
    if(rand()%100 < 50)
        for(int i=0; i<4; i++){
            if(a[i]!=NULL)
                if(minT>a[i]->energy && a[i]->canEat){
                    minT = a[i]->energy;
                    tmp = a[i];
                }
        }
    else
        for(int i=3; i>=0; i--){
            if(a[i]!=NULL)
                if(minT>a[i]->energy && a[i]->canEat){
                    minT = a[i]->energy;
                    tmp = a[i];
                }
        }
    return tmp;
}

Cell* Colony::getBreedingCell(vector<Cell*> a){
    if(rand()%100 < 50){
        for(int i=0; i<4; i++)
            if(a[i]!=NULL)
                if(a[i]->canBreed && a[i]->energy >=minEnergy)
                    return a[i];
    }
    else{
        for(int i=3; i>=0; i--)
            if(a[i]!=NULL)
                if(a[i]->canBreed && a[i]->energy >=minEnergy)
                    return a[i];
    }
    return NULL;
}

Cell* Colony::getPoisonousCell(vector<Cell*> a){
    for(int i=0; i<4; i++){
        if(a[i]!=NULL)
            if(a[i]->canPoison)
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
                cells[i][j]->canBreed = false;
                cells[i][j]->canEat = true;
                if((cells[i][j]->age%toxicTime)==0 && cells[i][j]->age>0){
                    cells[i][j]->canPoison = true;
                    cout << cells[i][j]->age << " y pudo envenenar"<<endl;
                }
                if((cells[i][j]->age%breedLapse)==0 && cells[i][j]->age>0 && cells[i][j]->energy >=minEnergy){
                    cells[i][j]->canBreed = true;
                    cout << cells[i][j]->age << " y pudo fisionarse"<<endl;
                }
            }
            tmpCells[i][j] = new Cell(*cells[i][j]);
        }
    //Cells inside the space
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            neighborhood[0] = NULL; neighborhood[1] = NULL; neighborhood[2] = NULL; neighborhood[3] = NULL;
            if(cells[i][j]->type == BLANK){
                if(i>0){if(cells[i-1][j]->type == BACTERIUM){neighborhood[0] = cells[i-1][j];}}
                if(i<rows-1){if(cells[i+1][j]->type == BACTERIUM)neighborhood[1] = cells[i+1][j];}
                if(j>0){if(cells[i][j-1]->type == BACTERIUM)neighborhood[2] = cells[i][j-1];}
                if(j<cols - 1){if(cells[i][j+1]->type == BACTERIUM) neighborhood[3] = cells[i][j+1];}
                //Nacimiento de una bacteria
                Cell *tmp = getBreedingCell(neighborhood);
                if(tmp!=NULL){
                    //El resultado se queda en tmpCells
                    tmp->canBreed = false;
                    tmpCells[i][j]->type = BACTERIUM;
                    tmpCells[i][j]->age = 0;
                    tmpCells[i][j]->energy = initialTl;
                }
                //Excrecion de una toxina
                else{
                    tmp = getPoisonousCell(neighborhood);
                    if(tmp!=NULL){
                        //El resultado se queda en tmpCells
                        tmp->canPoison = false;
                        tmpCells[i][j]->type = TOXIN;
                        tmpCells[i][j]->canPoison = true;
                    }
                }
            }
            //Alimentacion de una bacteria
            else if(cells[i][j]->type == FOOD){
                if(i>0){if(cells[i-1][j]->type == BACTERIUM){neighborhood[0] = cells[i-1][j];}}
                if(i<rows-1){if(cells[i+1][j]->type == BACTERIUM)neighborhood[1] = cells[i+1][j];}
                if(j>0){if(cells[i][j-1]->type == BACTERIUM)neighborhood[2] = cells[i][j-1];}
                if(j<cols - 1){if(cells[i][j+1]->type == BACTERIUM) neighborhood[3] = cells[i][j+1];}
                Cell *tmp = NULL;
                tmp = getDyingCell(neighborhood);
                if(tmp!=NULL){
                    //El resultado se queda en tmpCells
                    tmp->energy+=boostFeeding;
                    tmp->canEat = false;
                    tmpCells[i][j]->type = BLANK;
                }
            }
            //Intoxicacion de una bacteria
            else if(cells[i][j]->type == TOXIN){
                if(i>0){if(cells[i-1][j]->type == BACTERIUM && cells[i-1][j]->energy>0){neighborhood[0] = cells[i-1][j];}}
                if(i<rows-1){if(cells[i+1][j]->type == BACTERIUM && cells[i+1][j]->energy>0)neighborhood[1] = cells[i+1][j];}
                if(j>0){if(cells[i][j-1]->type == BACTERIUM && cells[i][j-1]->energy>0)neighborhood[2] = cells[i][j-1];}
                if(j<cols - 1){if(cells[i][j+1]->type == BACTERIUM && cells[i][j+1]->energy>0) neighborhood[3] = cells[i][j+1];}
                Cell *tmp = NULL;
                tmp = getDyingCell(neighborhood);
                if(tmp!=NULL){
                    cells[i][j]->canPoison = false;
                    tmp->energy-=toxinDamage;
                    if(tmp->energy < 0)
                        tmp->energy = 0;
                }
            }
        }
    }
    counters[BLANK] = 0; counters[BACTERIUM] = 0; counters[FOOD] = 0; counters[TOXIN] = 0;
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++){
            if(cells[i][j]->type == BACTERIUM){
                if(cells[i][j]->energy==0){
                    cells[i][j]->type = FOOD;
                    cells[i][j]->age = 0;
                }
            }
            else if(tmpCells[i][j]->type==BACTERIUM && tmpCells[i][j]->age==0)
                cells[i][j] = tmpCells[i][j];
            else if(cells[i][j]->type == TOXIN && !cells[i][j]->canPoison)
                cells[i][j]->type = BLANK;
            else if(tmpCells[i][j]->type != cells[i][j]->type)
                cells[i][j] = tmpCells[i][j];
            counters[cells[i][j]->type]+=1;
            if(cells[i][j]->type == BACTERIUM)
                cells[i][j]->age+=1;
        }
    cout << "Recuento: "<<endl;
    cout << counters[BACTERIUM] << " bacterias"<<endl;
    cout << counters[TOXIN] << " toxinas"<<endl;
    cout << counters[FOOD] << " nutrientes"<<endl;
}

void Colony::swapCells(int x1, int y1, int x2, int y2){
    Cell* tmp = cells[x1][y1];
    cells[x1][y1] = cells[x2][y2];
    cells[x2][y2] = tmp;
}

void Colony::move(){
    vector< Cell* > neighborhood;
    neighborhood.resize(4);
    int r = 0;
    int countersT[4];
    for(int i=offset/2; i<rows-1; i+=2){
        for(int j=offset%2; j<cols-1; j+=2){
            countersT[BLANK] = 0; countersT[BACTERIUM] = 0; countersT[FOOD] = 0; countersT[TOXIN] = 0;
            countersT[cells[i][j]->type]++;
            countersT[cells[i][j+1]->type]++;
            countersT[cells[i+1][j]->type]++;
            countersT[cells[i+1][j+1]->type]++;
            if(countersT[BACTERIUM]+countersT[FOOD] + countersT[TOXIN]<4){
                if(countersT[BACTERIUM]>0){
                    if(countersT[TOXIN]==0){
                        if(countersT[FOOD]==0){
                            neighborhood[0] = cells[i][j]; neighborhood[1] = cells[i][j+1]; neighborhood[2] = cells[i+1][j+1]; neighborhood[3] = cells[i+1][j];
                            if(rand()%100<50){
                                //Rotar a la izquierda
                                cells[i][j] = neighborhood[1]; cells[i][j+1] = neighborhood[2]; cells[i+1][j] = neighborhood[0]; cells[i+1][j+1] = neighborhood[3];
                            }
                            else{
                                //Rotar a la derecha
                                cells[i][j] = neighborhood[3]; cells[i][j+1] = neighborhood[0]; cells[i+1][j+1] = neighborhood[1]; cells[i+1][j] = neighborhood[2];
                            }
                        }
                        else if(countersT[FOOD] == 1){
                            if(countersT[BACTERIUM]==1){
                                //Hacer que coincida fila o columna
                                //Hay 4 casos
                                //El movimiento horizontal tiene una probabilidad igual al vertical
                                r = rand()%100;
                                if(cells[i][j]->type==BACTERIUM && cells[i+1][j+1]->type==FOOD){
                                    if(r<50) swapCells(i, j, i, j+1);
                                    else swapCells(i, j, i+1,j);
                                }
                                else if(cells[i][j]->type==FOOD && cells[i+1][j+1]->type==BACTERIUM){
                                    if(r<50) swapCells(i+1, j+1, i+1, j);
                                    else swapCells(i+1, j+1, i, j+1);
                                }
                                else if(cells[i][j+1]->type==BACTERIUM && cells[i+1][j]->type==FOOD){
                                    if(r < 50) swapCells(i, j+1, i, j);
                                    else swapCells(i, j+1, i+1, j+1);
                                }
                                else if(cells[i][j+1]->type==FOOD && cells[i+1][j]->type==BACTERIUM){
                                    if(r<50) swapCells(i+1, j, i+1, j+1);
                                    else swapCells(i+1, j, i, j);
                                }
                            }
                            else if(countersT[BACTERIUM]==2){
                                //Hacer que ambas queden a los lados
                                if(cells[i][j]->type==FOOD && cells[i+1][j+1]->type==BACTERIUM){
                                        if(cells[i][j+1]->type==BACTERIUM) swapCells(i+1, j+1, i+1, j);
                                        else swapCells(i+1, j+1, i, j+1);
                                }
                                else if(cells[i][j+1]->type==FOOD && cells[i+1][j]->type==BACTERIUM){
                                        if(cells[i][j]->type==BACTERIUM) swapCells(i+1, j, i+1, j+1);
                                        else swapCells(i+1, j, i, j);
                                }
                                else if(cells[i+1][j]->type==FOOD && cells[i][j+1]->type==BACTERIUM){
                                        if(cells[i+1][j+1]->type==BACTERIUM) swapCells(i, j+1, i, j);
                                        else swapCells(i, j+1, i+1, j+1);
                                }
                                else if(cells[i+1][j+1]->type==FOOD && cells[i][j]->type==BACTERIUM){
                                        if(cells[i+1][j]->type==BACTERIUM) swapCells(i, j, i , j+1);
                                        else swapCells(i, j, i+1, j);
                                }
                            }
                        }
                    }
                    else{
                        if(countersT[BACTERIUM]==1 && countersT[TOXIN]==1){
                            if(countersT[FOOD]==0){
                                //Evadir a como de lugar la toxina
                                if(cells[i][j]->type == BACTERIUM){
                                    if(cells[i][j+1]->type == TOXIN) swapCells(i, j, i+1,j);
                                    else if(cells[i+1][j]->type==TOXIN) swapCells(i, j, i, j+1);
                                }
                                else if(cells[i+1][j]->type == BACTERIUM){
                                    if(cells[i][j]->type == TOXIN) swapCells(i+1, j, i+1, j+1);
                                    else if(cells[i+1][j+1]->type == TOXIN)	swapCells(i+1, j, i, j);
                                }
                                else if(cells[i+1][j+1]->type == BACTERIUM){
                                    if(cells[i+1][j]->type==TOXIN) swapCells(i+1, j+1, i, j+1);
                                    else if(cells[i][j+1]->type == TOXIN) swapCells(i+1, j+1, i+1, j);
                                }
                                else if(cells[i][j+1]->type == BACTERIUM){
                                    if(cells[i][j]->type == TOXIN) swapCells(i, j+1, i+1, j+1);
                                    else if(cells[i+1][j+1]->type == TOXIN) swapCells(i, j+1, i, j);
                                }
                            }
                            else if(countersT[FOOD] == 1){
                                //Moverse para encontrarse a lado de la comida evitando la toxina
                                if(cells[i][j]->type == BACTERIUM && cells[i+1][j+1]->type == FOOD){
                                    if(cells[i][j+1]->type == BLANK) swapCells(i, j, i, j+1);
                                    else swapCells(i, j, i+1, j);
                                }
                                else if(cells[i+1][j+1]->type == BACTERIUM && cells[i][j]->type==FOOD){
                                    if(cells[i][j+1]->type == BLANK) swapCells(i+1, j+1, i, j+1);
                                    else swapCells(i+1, j+1, i+1, j);
                                }
                                else if(cells[i][j+1]->type==BACTERIUM && cells[i+1][j]->type==FOOD){
                                    if(cells[i+1][j+1]->type == BLANK) swapCells(i, j+1, i+1, j+1);
                                    else swapCells(i, j+1, i, j);
                                }
                                else if(cells[i+1][j]->type==BACTERIUM && cells[i][j+1]->type==FOOD){
                                    if(cells[i][j]->type == BLANK) swapCells(i+1, j, i, j);
                                    else swapCells(i+1, j, i+1, j+1);
                                }
                            }
                        }
                        else if(countersT[BACTERIUM]==2 && countersT[TOXIN]==1){
                            //Mover una de las bacterias para ser salvada
                            r = rand()%100;
                            if(cells[i][j]->type == TOXIN && cells[i+1][j+1]->type==BLANK){
                                if(r<50) swapCells(i, j+1, i+1, j+1);
                                else swapCells(i+1, j, i+1, j+1);
                            }
                            else if(cells[i+1][j+1]->type==TOXIN && cells[i][j]->type==BLANK){
                                if(r<50) swapCells(i+1, j, i, j);
                                else swapCells(i, j+1, i, j);
                            }
                            else if(cells[i][j+1]->type==TOXIN && cells[i+1][j]->type==BLANK){
                                if(r<50) swapCells(i, j, i+1, j);
                                else swapCells(i+1, j+1, i+1, j);
                            }
                            else if(cells[i+1][j]->type==TOXIN && cells[i][j+1]->type==BLANK){
                                if(r<50) swapCells(i, j, i, j+1);
                                else swapCells(i+1, j+1, i, j+1);
                            }
                        }
                    }
                }
            }
        }
    }
    offset += 1;
    if(offset > 3)
        offset = 0;
}

void Colony::decreaseEnergy(){
    for(int i =0; i<rows; i++)
        for(int j=0; j<cols; j++)
            if(cells[i][j]->type == BACTERIUM){
                cells[i][j]->energy-=energyLostIteration;
                if(cells[i][j]->energy <= 0){
                    cells[i][j]->type = FOOD;
                    cells[i][j]->age = 0;
                }
            }
}
