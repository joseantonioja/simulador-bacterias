#ifndef COLONY_H
#define COLONY_H

#define BLANK 0
#define FOOD 1
#define BACTERIUM 2
#define TOXIN 3
#include <vector>
#include <iostream>
#include <time.h>
#include <stdlib.h>
using namespace std;
class Cell{
public:
    char type;
    int energy;
    int age;
    bool canBreed;
    bool canEat;
    bool canPoison;
    Cell(char, int);
    Cell(const Cell&);
};
class Colony{
private:
    Cell* getDyingCell(vector<Cell*>);
    Cell* getBreedingCell(vector<Cell*>);
    Cell* getPoisonousCell(vector<Cell*>);
    void swapCells(int, int, int, int );
public:
    int breedLapse;
    int boostFeeding;
    int minEnergy;
    int toxinDamage;
    int toxicTime;
    int initialTl;
    int rows;
    int cols;
    int gen;
    int offset;
    int energyLostIteration;
    int counters[4];
    vector< vector<Cell*> > cells;
    Colony(int, int, int, int, double, double, double, int, int, int, int, int);
    void printCells();
    void eatAndBreed();
    void move();
    void decreaseEnergy();
};

#endif // COLONY_H
