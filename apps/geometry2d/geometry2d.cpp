#include "../../include/Visualizer.hpp"
#include "../../include/OwnedPoint.hpp"
#include "../../include/OwnedColor.hpp"

#include <random>

#include <csg.h>

// use cmake to compile

int main(int argc, char * argv[]){
	// declare vars
	LookingGlass::Visualizer * mywindow = new LookingGlass::Visualizer();

	// geometric object
	// csg::CSGeometry2D obj(csg::Circle({-0.5, 0.0}, 0.5), csg::Circle({-0.25, 0.0}, 0.5), csg::XOR);
	// csg::Circle obj = csg::Circle({-0.5, 0.0}, 0.5);
	// csg::Rectangle obj = csg::Rectangle({0.0,0.0}, {0.1,0.3});
	// csg::Ellipse obj = csg::Ellipse({0.0,0.0}, {0.5,0.3});
	// csg::Triangle obj = csg::Triangle({0.0,0.0}, {0.5,0.3}, {0.1, -0.3});

	std::shared_ptr<csg::CSGeometry2D> obj1(new csg::CSGeometry2D(std::shared_ptr<csg::Primitive2D>(new csg::Rectangle(csg::Point<2>(0,0),csg::Point<2>(2,2))), std::shared_ptr<csg::Primitive2D>(new csg::Circle(csg::Point<2>(0,0), 0.6)), csg::DIFFERENCE));
	std::shared_ptr<csg::CSGeometry2D> obj2(new csg::CSGeometry2D(std::shared_ptr<csg::Primitive2D>(new csg::Rectangle(csg::Point<2>(0,0),csg::Point<2>(0.3,1.8))), std::shared_ptr<csg::Primitive2D>(new csg::Rectangle(csg::Point<2>(0,0),csg::Point<2>(1.8,0.3))), csg::UNION));
	std::shared_ptr<csg::CSGeometry2D> obj3(new csg::CSGeometry2D(obj2, std::shared_ptr<csg::Primitive2D>(new csg::Circle(csg::Point<2>(0,0),0.1)), csg::DIFFERENCE));
	csg::CSGeometry2D obj = csg::CSGeometry2D(obj1, obj3, csg::UNION);
	// csg::CSGeometry2D obj1(csg::Circle({-0.5, 0.5}, 0.3), csg::Rectangle({0.1,0.0},{0.3,0.6}), csg::UNION);
	// csg::CSGeometry2D obj2(csg::Ellipse({-0.5,-0.5}, {0.3, 0.1}), csg::Triangle({0.0, -0.8}, {0.2, -0.7}, {0.0, -0.5}), csg::UNION);
	// csg::CSGeometry2D obj = csg::CSGeometry2D(obj1, obj2, csg::UNION);

	// get outline of objitive with 100 points
	// std::vector<csg::Hull<2>> hv = obj.get_outline(100);

	// // visualize a Delaunay triangulation of a bunch of random points in a square
	// csg::RandomHash randhash;
	// std::vector<csg::Point<2>> rpts;
	// std::default_random_engine generator;
 //  	std::uniform_real_distribution<double> distribution(0.0,1.0);
 //  	rpts.resize(200*200);
	// for (auto i=0; i<200; i++){
	// 	for (auto j=0; j<200; j++){
	// 		double r = distribution(generator);
	// 		double theta = distribution(generator)*2*csg::pi;
	// 		rpts[i*200+j] = {r*cos(theta), r*sin(theta)};
	// 	}
	// }
	// csg::Delaunay dtri(rpts, 0);

	// get geometry triangulation
	std::shared_ptr<csg::Triangulation<3>> triang = csg::read_STL(argv[1], 0);
	// csg::Triangulation<2> triang = obj.get_triangulation(500);

	for (auto i=0; i<triang->points.size(); i++){
		// mywindow->add_point(LookingGlass::OwnedPoint(triang.points[i].x[0], triang.points[i].x[1], 0.0));
		mywindow->add_point(LookingGlass::OwnedPoint(triang->points[i].x[0], triang->points[i].x[1], triang->points[i].x[2]));
		mywindow->set_vertex_color(i, LookingGlass::OwnedColor(1.0,0.3,0.3,1.0));
	}
	for (auto i=0; i<triang->triangles.size(); i++){
			// mywindow->set_triangle_element(triang->triangles[i].x[0], triang->triangles[i].x[1], triang->triangles[i].x[2]);
			// mywindow->set_edge_element(triang.triangles[i].x[0], triang.triangles[i].x[1]);
			// mywindow->set_edge_element(triang.triangles[i].x[1], triang.triangles[i].x[2]);
			// mywindow->set_edge_element(triang.triangles[i].x[2], triang.triangles[i].x[0]);

			mywindow->set_edge_element(triang->triangles[i].x[0], triang->triangles[i].x[1]);
			mywindow->set_edge_element(triang->triangles[i].x[1], triang->triangles[i].x[2]);
			mywindow->set_edge_element(triang->triangles[i].x[2], triang->triangles[i].x[0]);
	}

	// unsigned int ct = 0;
	// for (auto j=0; j<hv.size(); j++){
	// 	csg::Hull<2> h = hv[j];

	// 	// for (auto i=0; i<h.points.size(); i++){
	// 	// 	mywindow->add_point(LookingGlass::OwnedPoint(h.points[i].x[0], h.points[i].x[1], 0.0));
	// 	// }
	// 	// for (auto i=0; i<h.points.size()-1; i++){
	// 	// 	mywindow->set_edge_element(ct+i, ct+i+1);
	// 	// }
	// 	// mywindow->set_edge_element(ct+h.points.size()-1, ct);

	// 	for (auto i=0; i<rpts.size(); i++) mywindow->add_point(LookingGlass::OwnedPoint(rpts[i].x[0], rpts[i].x[1], 0.0));
	// 	for (auto i=0; i<dtri.triangles.size(); i++){
	// 		if (dtri.triangles[i].state >0){
	// 			mywindow->set_edge_element(dtri.triangles[i].vertices[0], dtri.triangles[i].vertices[1]);
	// 			mywindow->set_edge_element(dtri.triangles[i].vertices[1], dtri.triangles[i].vertices[2]);
	// 			mywindow->set_edge_element(dtri.triangles[i].vertices[2], dtri.triangles[i].vertices[0]);
	// 		}
	// 	}

	// 	// for (auto i=0; i<triang.points.size(); i++) mywindow->add_point(LookingGlass::OwnedPoint(triang.points[i].x[0], triang.points[i].x[1], 0.0));
	// 	// for (auto i=0; i<triang.triangles.size(); i++){
	// 	// 		mywindow->set_triangle_element(triang.triangles[i].x[0], triang.triangles[i].x[1], triang.triangles[i].x[2]);
	// 	// 		// mywindow->set_edge_element(triang.triangles[i].x[0], triang.triangles[i].x[1]);
	// 	// 		// mywindow->set_edge_element(triang.triangles[i].x[1], triang.triangles[i].x[2]);
	// 	// 		// mywindow->set_edge_element(triang.triangles[i].x[2], triang.triangles[i].x[0]);
	// 	// }

	// 	// ct += h.points.size();
	// }


	mywindow->calculate_bounds();
	mywindow->run();
	//cout << "about to delete mywindow" << endl;
	delete mywindow;

	return 0;

}

