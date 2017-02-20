#include "../../include/Visualizer.hpp"
#include "../../include/OwnedPoint.hpp"
#include "../../include/OwnedColor.hpp"
#include "../../../CompGeometry/include/PointCloud.hpp"

// use cmake to compile

int main(int argc, char * argv[]){
	// declare vars


	// read in cloud
	PointCloud pc = PointCloud::read_LAS(argv[1]);
	pc.print_summary();

	// write cloud points to the viewer
	LookingGlass::Visualizer * mywindow = new LookingGlass::Visualizer();
	//cout << "about to run" << endl;
	const double * x = &(pc.x());
	const double * y = &(pc.y());
	const double * z = &(pc.z());
	const short unsigned int * in = &(pc.intensity());
	short unsigned int imax = in[0];
	for (auto i=0; i<pc.pointcount(); i++){
		if (in[i] > imax) imax = in[i];
	}

	//mywindow->set_num_vertices(pc.pointcount());
	for (auto i=0; i<pc.pointcount(); i++){
		mywindow->add_point(LookingGlass::OwnedPoint(x[i],y[i],z[i]));
		// mywindow->set_vertex_color(i, LookingGlass::OwnedColor(0.5, 0.5, 0.5, 0.5));
		mywindow->set_vertex_color(i, LookingGlass::OwnedColor(1.0,1.0,1.0,double(in[i])/double(imax)));
	}
	mywindow->calculate_bounds();

	mywindow->run();
	//cout << "about to delete mywindow" << endl;
	delete mywindow;

	return 0;

}

