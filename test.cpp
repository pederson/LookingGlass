#include "include/Visualizer.hpp"

// use cmake to compile

int main(int argc, char * argv[]){
	// declare vars

	// test the base class
	LookingGlass::Visualizer * mywindow = new LookingGlass::Visualizer();
	//cout << "about to run" << endl;
	mywindow->set_test_case();
	mywindow->run();
	//cout << "about to delete mywindow" << endl;
	delete mywindow;

	return 0;

}

