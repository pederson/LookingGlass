#include "../../include/Visualizer.hpp"
#include "../../include/OwnedPoint.hpp"
#include "../../include/OwnedColor.hpp"

#include <csg.h>

// use cmake to compile

int main(int argc, char * argv[]){
	// declare vars


	// read in cloud
	csg::CSGeometry2D obj(csg::Circle({-0.5, 0.0}, 0.5), csg::Circle({0.5, 0.0}, 0.5), csg::UNION);

	// //mywindow->set_num_vertices(pc.pointcount());
	// for (auto i=0; i<pc.pointcount(); i++){
	// 	mywindow->add_point(LookingGlass::OwnedPoint(x[i],y[i],z[i]));
	// 	// mywindow->set_vertex_color(i, LookingGlass::OwnedColor(0.5, 0.5, 0.5, 0.5));
	// 	mywindow->set_vertex_color(i, LookingGlass::OwnedColor(1.0,1.0,1.0,double(in[i])/double(imax)));
	// }
	// mywindow->calculate_bounds();

	LookingGlass::Visualizer * mywindow = new LookingGlass::Visualizer();
	mywindow->run();
	//cout << "about to delete mywindow" << endl;
	delete mywindow;

	return 0;

}

