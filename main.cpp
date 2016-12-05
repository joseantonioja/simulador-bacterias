#include "colony.h"
#include <iostream>
#include <vector>
using namespace std;
int main(){
	Colony test(5, 5, 5, 2, 0.1, 0.2, 10);
	test.printCells();
	test.move();
	test.printCells();
	cout << "Comiendo"<<endl;
	test.eatAndBreed();
	test.printCells();
	test.move();
	test.printCells();
	cout << "Comiendo"<<endl;
	test.eatAndBreed();
	test.printCells();
	return 0;
}