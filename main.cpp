#include "colony.h"
#include <iostream>
#include <vector>
using namespace std;
int main(){
	Colony test(5, 5, 5, 2, 0.1, 0.2, 10);
	test.printCells();
	for(int i=0; i<11; i++){
		cout << "Evolving..."<<endl;
		test.eatAndBreed();
		test.printCells();
	}
	return 0;
}