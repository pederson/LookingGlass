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
	// std::shared_ptr<csg::CSGeometry2D> obj1(new csg::CSGeometry2D(std::shared_ptr<csg::Primitive2D>(new csg::Rectangle(csg::Point<2>(0,0),csg::Point<2>(2,2))), std::shared_ptr<csg::Primitive2D>(new csg::Circle(csg::Point<2>(0,0), 0.6)), csg::DIFFERENCE));
	// std::shared_ptr<csg::CSGeometry2D> obj2(new csg::CSGeometry2D(std::shared_ptr<csg::Primitive2D>(new csg::Rectangle(csg::Point<2>(0,0),csg::Point<2>(0.3,1.8))), std::shared_ptr<csg::Primitive2D>(new csg::Rectangle(csg::Point<2>(0,0),csg::Point<2>(1.8,0.3))), csg::UNION));
	// std::shared_ptr<csg::CSGeometry2D> obj3(new csg::CSGeometry2D(obj2, std::shared_ptr<csg::Primitive2D>(new csg::Circle(csg::Point<2>(0,0),0.1)), csg::DIFFERENCE));
	// csg::CSGeometry2D obj = csg::CSGeometry2D(obj1, obj3, csg::UNION);
	// csg::CSGeometry2D obj1(csg::Circle({-0.5, 0.5}, 0.3), csg::Rectangle({0.1,0.0},{0.3,0.6}), csg::UNION);
	// csg::CSGeometry2D obj2(csg::Ellipse({-0.5,-0.5}, {0.3, 0.1}), csg::Triangle({0.0, -0.8}, {0.2, -0.7}, {0.0, -0.5}), csg::UNION);
	// csg::CSGeometry2D obj = csg::CSGeometry2D(obj1, obj2, csg::UNION);

	// create the tree
	static const std::size_t rfactor = 3;
	// csg::Quadtree<double> qtree;
	csg::Orthtree<2, rfactor, double> qtree;
	
	// build a quadtree of doubles corresponding to a cirlce centered at 0,0 with radius 1
	struct CircleThing{
		double rad = 1.0, radsmall = 0.1;
		csg::Point2 cen = csg::Point2(0,0);
		csg::Box<2> bounds = {csg::Point2(-1,-1), csg::Point2(1,1)};
		double coarsesize = 0.05;
		double finesize = 0.003;

		double getValue(csg::Box<2> bx) const{
			csg::Point2 p = 0.5*(bx.lo + bx.hi);
			return getValue(p);
		}

		double getValue(const csg::Point2 & p) const{
			if (csg::Point2::distsq(p,cen) > rad*rad) return 0.0;
			return 1.0;
		}

		bool isUniform(csg::Box<2> bx) const{
			csg::Point2 tl = bx.lo; tl.x[1] = bx.hi.x[1];
			csg::Point2 br = bx.hi; br.x[1] = bx.lo.x[1];
			csg::Point2 c = 0.5*(bx.hi+bx.lo);

			// cout << c << tl << br << endl;
			// cout << getValue(tl) << "  " << getValue(c) << endl;
			if (getValue(tl) == getValue(br) &&
				getValue(br) == getValue(bx.hi) &&
				getValue(bx.hi) == getValue(bx.lo) &&
				getValue(bx.lo) == getValue(c)) return true;
			return false;
		}

		bool isUniform(int key) const{return isUniform(getBox(key));};

		double getValue(int key) const{return getValue(getBox(key));};

		std::size_t getSubdomain(int key) const{return 0;};

		csg::Box<2> getBox(int key) const{
			auto lvl = csg::BruteForceDecoder<2,rfactor,int>::decodeLevel(key);
			double rf = pow(rfactor,lvl);
			csg::Point<2> boxsize = 1.0/static_cast<double>(rf)*(bounds.hi-bounds.lo);
			csg::IntPoint<2> off = csg::BruteForceDecoder<2,rfactor,int>::getOffsetWithinLevel(key);
			csg::Point<2> newlo = bounds.lo+boxsize*off;
			csg::Box<2> rbox(newlo, newlo+boxsize);
			return rbox;
		}
	};

	CircleThing c1;
	csg::Box<2> bounds(csg::Point2(-2.0,-2.0), csg::Point2(2.0,2.0));
	qtree.buildTree(5, c1, c1, c1, 0, 0);
	// qtree.buildLevelBoundary(6, 2.0);
	// qtree.annexLevelBoundary<3>();
	// qtree.reassignLevelBoundary<3>(2.0);
	qtree.print_summary();
	
	// std::cout << "qtree.leaf_end()->first: " << qtree.leaf_end()->first << std::endl;
	// throw -1;
	// draw the skeleton of the leaf nodes
	csg::Box<2> motherbox(csg::Point2(0,0),csg::Point2(1,1));
	// for (auto it=qtree.level_begin(4); it!=qtree.level_end(4); it++){
	for (auto it=qtree.subdomain_begin(5,0); it!=qtree.subdomain_end(5,0); it++){
	// for (auto it=qtree.leaf_begin(); it!=qtree.leaf_end(); it++){
	// for (auto it=qtree.boundary_begin<3>(); it!=qtree.boundary_end<3>(); it++){
		
		// std::cout << "key: " << it->first << std::endl;
		size_t lvl = qtree.getLevel(it->first);
		csg::IntPoint2 off = csg::BruteForceDecoder<2,rfactor,int>::getOffsetWithinLevel(it->first);
		double rfac = pow(1.0/static_cast<double>(rfactor),lvl);
		csg::Box<2> lvlbox(motherbox.lo*rfac,motherbox.hi*rfac);

		csg::Box<2> mybox(rfac*off, rfac*off+lvlbox.hi);
		csg::Point2 tl(mybox.lo.x[0],mybox.hi.x[1]);
		csg::Point2 bl(mybox.lo.x[0],mybox.lo.x[1]);
		csg::Point2 br(mybox.hi.x[0],mybox.lo.x[1]);
		csg::Point2 tr(mybox.hi.x[0],mybox.hi.x[1]);

		mywindow->add_edge(LookingGlass::OwnedPoint(tl.x[0],tl.x[1], 0.0), LookingGlass::OwnedPoint(bl.x[0],bl.x[1], 0.0));
		mywindow->add_edge(LookingGlass::OwnedPoint(bl.x[0],bl.x[1], 0.0), LookingGlass::OwnedPoint(br.x[0],br.x[1], 0.0));
		mywindow->add_edge(LookingGlass::OwnedPoint(br.x[0],br.x[1], 0.0), LookingGlass::OwnedPoint(tr.x[0],tr.x[1], 0.0));
		mywindow->add_edge(LookingGlass::OwnedPoint(tr.x[0],tr.x[1], 0.0), LookingGlass::OwnedPoint(tl.x[0],tl.x[1], 0.0));
		
		// if (it->second.mVal > 1.5){
		// 	mywindow->add_triangle(LookingGlass::OwnedPoint(tl.x[0],tl.x[1], 0.0001), LookingGlass::OwnedPoint(bl.x[0],bl.x[1], 0.0001), LookingGlass::OwnedPoint(br.x[0],br.x[1], 0.0001), LookingGlass::OwnedColor(1,0,0,1));
		// 	mywindow->add_triangle(LookingGlass::OwnedPoint(br.x[0],br.x[1], 0.0001), LookingGlass::OwnedPoint(tr.x[0],tr.x[1], 0.0001), LookingGlass::OwnedPoint(tl.x[0],tl.x[1], 0.0001), LookingGlass::OwnedColor(1,0,0,1));
		// }
		// else if (it->second.mVal > 0.5){
		// 	mywindow->add_triangle(LookingGlass::OwnedPoint(tl.x[0],tl.x[1], 0.0), LookingGlass::OwnedPoint(bl.x[0],bl.x[1], 0.0), LookingGlass::OwnedPoint(br.x[0],br.x[1], 0.0), LookingGlass::OwnedColor(0.5,0.5,0.5,0.5));
		// 	mywindow->add_triangle(LookingGlass::OwnedPoint(br.x[0],br.x[1], 0.0), LookingGlass::OwnedPoint(tr.x[0],tr.x[1], 0.0), LookingGlass::OwnedPoint(tl.x[0],tl.x[1], 0.0), LookingGlass::OwnedColor(0.5,0.5,0.5,0.5));
		// }
		// else{
		// 	mywindow->add_triangle(LookingGlass::OwnedPoint(tl.x[0],tl.x[1], 0.0), LookingGlass::OwnedPoint(bl.x[0],bl.x[1], 0.0), LookingGlass::OwnedPoint(br.x[0],br.x[1], 0.0), LookingGlass::OwnedColor(0.0,0.0,0.0,0.5));
		// 	mywindow->add_triangle(LookingGlass::OwnedPoint(br.x[0],br.x[1], 0.0), LookingGlass::OwnedPoint(tr.x[0],tr.x[1], 0.0), LookingGlass::OwnedPoint(tl.x[0],tl.x[1], 0.0), LookingGlass::OwnedColor(0.0,0.0,0.0,0.5));
		// }
	}

	mywindow->calculate_bounds();
	mywindow->run();
	//cout << "about to delete mywindow" << endl;
	delete mywindow;

	return 0;

};

