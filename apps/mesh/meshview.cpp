#include "../../include/Visualizer.hpp"
#include "../../include/OwnedPoint.hpp"
#include "../../include/OwnedColor.hpp"

#include "../../../simbox/include/Mesh.hpp"
#include "../../../simbox/include/Mesh2D.hpp"

// use cmake to compile

int main(int argc, char * argv[]){
	// declare vars


	// read in mesh
	std::shared_ptr<simbox::Mesh2D> msh = simbox::Mesh2D::read_MSH(argv[1]);
	msh->print_summary();

	// write cloud points to the viewer
	LookingGlass::Visualizer * mywindow = new LookingGlass::Visualizer();
	//cout << "about to run" << endl;
	

	//mywindow->set_num_vertices(pc.pointcount());
	for (auto i=0; i<msh->snodecount(); i++){
		mywindow->add_point(LookingGlass::OwnedPoint(msh->snode(i).x[0],msh->snode(i).x[1], 0.0));
		// mywindow->set_vertex_color(i, LookingGlass::OwnedColor(0.5, 0.5, 0.5, 0.5));
		// mywindow->set_vertex_color(i, LookingGlass::OwnedColor(1.0,1.0,1.0,double(in[i])/double(imax)));
	}
	for (auto i=0; i<msh->selementcount(); i++){
		simbox::Element<2> se = msh->selement(i);
		for (auto j=0; j<se.nodeinds.size()-1; j++){
			mywindow->set_edge_element(se.nodeinds[j], se.nodeinds[j+1]);
		}
		mywindow->set_edge_element(se.nodeinds[se.nodeinds.size()-1], se.nodeinds[0]);
	}
	mywindow->calculate_bounds();

	mywindow->run();
	//cout << "about to delete mywindow" << endl;
	delete mywindow;

	return 0;

}

