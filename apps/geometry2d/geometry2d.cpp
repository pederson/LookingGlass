#include "../../include/Visualizer.hpp"
#include "../../include/OwnedPoint.hpp"
#include "../../include/OwnedColor.hpp"

#include <csg.h>

// use cmake to compile

int main(int argc, char * argv[]){
	// declare vars
	LookingGlass::Visualizer * mywindow = new LookingGlass::Visualizer();

	// geometric object
	csg::CSGeometry2D obj(csg::Circle({-0.5, 0.0}, 0.5), csg::Circle({-0.25, 0.0}, 0.5), csg::XOR);
	// csg::Circle obj = csg::Circle({-0.5, 0.0}, 0.5);
	// csg::Rectangle obj = csg::Rectangle({0.0,0.0}, {0.1,0.3});
	// csg::Ellipse obj = csg::Ellipse({0.0,0.0}, {0.5,0.3});
	// csg::Triangle obj = csg::Triangle({0.0,0.0}, {0.5,0.3}, {0.1, -0.3});

	// get outline of objitive with 100 points
	std::vector<csg::Hull<2>> hv = obj.get_outline(100);

	for (auto j=0; j<hv.size(); j++){
		csg::Hull<2> h = hv[j];

		for (auto i=0; i<h.points.size(); i++){
			mywindow->add_point(LookingGlass::OwnedPoint(h.points[i].x[0], h.points[i].x[1], 0.0));
		}
		// for (auto i=0; i<h.points.size()-1; i++){
		// 	mywindow->set_edge_element(i, i+1);
		// }
		// mywindow->set_edge_element(h.points.size()-1, 0);
	}
	mywindow->calculate_bounds();

	
	mywindow->run();
	//cout << "about to delete mywindow" << endl;
	delete mywindow;

	return 0;

}

