#define BLANK 0
#define FOOD 1
#define BACTERIUM 2
#include <vector>
using namespace std;
class Cell{
public:
	char type;
	int timeLeft;
	int age;
	bool canBreed;
	Cell(char, int);
	Cell(const Cell&);
};
class Colony{
private:
	Cell* getDyingCell(vector<Cell*>);
	Cell* getBreedingCell(vector<Cell*>);
public:
	char breedLapse;
	char boostFeeding;
	int initialTl;
	int rows;
    int cols;
    int gen;
    vector< vector<Cell*> > cells;
   	Colony(int, int, char, char, double , double, int);
   	void printCells();
    void eatAndBreed();
    void move();
};