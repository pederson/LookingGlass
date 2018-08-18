#include "../../include/Visualizer.hpp"
#include "../../include/OwnedPoint.hpp"
#include "../../include/OwnedColor.hpp"
#include <tinyxml2.h>

#include <random>

#include <csg.h>

using namespace csg;
// use cmake to compile





typedef csg::Octree<int> TreeT;




struct QueryObject{
	std::shared_ptr<Primitive3D> obj;


	csg::Box<3> bounds;
	// double coarsesize = 0.05;
	// double finesize = 0.003;
	csg::IntegralKeyDecoder<3,2,int> ikd;


	QueryObject(std::shared_ptr<Primitive3D> & o)
	: obj(o) {bounds = csg::Box<3>(1.5*o->get_bounding_box().lo, 1.5*o->get_bounding_box().hi);};

	double getValue(csg::Box<3> bx) const{
		csg::Point3 p = 0.5*(bx.lo + bx.hi);
		return getValue(p);
	}

	double getValue(const csg::Point3 & p) const{
		if (obj->contains_point(p)) return 1.0;
		return 0.0;
	}

	bool isUniform(csg::Box<3> bx) const{
		csg::Point3 tl = bx.lo; tl.x[1] = bx.hi.x[1];
		csg::Point3 br = bx.hi; br.x[1] = bx.lo.x[1];
		csg::Point3 c = 0.5*(bx.hi+bx.lo);



		// cout << c << tl << br << endl;
		// cout << getValue(tl) << "  " << getValue(c) << endl;
		if (getValue(bx.hi) == getValue(bx.lo) &&
			getValue(bx.lo) == getValue(c)) return true;
		return false;
	}

	bool isUniform(int key) const{return isUniform(getBox(key));};

	double getValue(int key) const{return getValue(getBox(key));};

	// std::size_t getSubdomain(int key) const{return 0;};

	csg::Box<3> getBox(int key) const{
		auto lvl = ikd.getLevel(key);
		double rf = pow(2,lvl);
		csg::Point<3> boxsize = 1.0/static_cast<double>(rf)*(bounds.hi-bounds.lo);
		csg::IntPoint<3> off = ikd.getOffsetWithinLevel(key);
		csg::Point<3> newlo = bounds.lo+boxsize*off;
		csg::Box<3> rbox(newlo, newlo+boxsize);
		return rbox;
	}




	decltype(auto) insert(TreeT & t, std::pair<TreeT::KeyType, TreeT::NodeType> p) const
	{

		std::size_t l = t.getLevel(p.first);
		auto r = t.insert(p, l); // returns an pair<iterator, value> to the inserted element
		// t[l].add_to_set(r.first, s);
		return r;
	}
};

	


int main(int argc, char * argv[]){
	// declare vars
	LookingGlass::Visualizer * mywindow = new LookingGlass::Visualizer();

	// read first input argument as the filename
	std::shared_ptr<Primitive3D> obj = csg::read3D(argv[argc-1]);
	obj->print_summary();

	std::cout << "read the geometry" << std::endl;

	// create the tree
	csg::Octree<int> tree;
	QueryObject c1(obj);
	std::size_t lvlmin = 6, lvlmax = 7;
	csg::Box<3> bounds = obj->get_bounding_box();//(obj->get_bounding_box().lo, obj->get_bounding_box().hi);
	tree.buildTree(lvlmin, lvlmax, c1, c1, c1, 0, 0);

	std::cout << "built the tree" << std::endl;

	

	// draw the skeleton of the leaf nodes
	csg::Box<3> motherbox = bounds;//(csg::Point3(0,0,0),csg::Point3(1,1,1));
	for (auto it=tree.begin(); it!=tree.end(); it++){
		size_t lvl = tree.getLevel(it->first);
		if (c1.isUniform(it->first) || lvl < lvlmin) continue;
		// if (!it->second.isLeaf()) continue;
		// std::cout << "key: " << it->first << std::endl;
		

		// if (lvl <= lvlmin) continue;

		csg::IntPoint3 off = c1.ikd.getOffsetWithinLevel(it->first);
		double rfac = pow(1.0/static_cast<double>(2),lvl);
		csg::Box<3> lvlbox(motherbox.lo*rfac,motherbox.hi*rfac);


		csg::Box<3> mybox = c1.getBox(it->first);//(rfac*off, rfac*off+lvlbox.hi);

		csg::Point3 tl(mybox.lo.x[0],mybox.hi.x[1], mybox.hi.x[2]);
		csg::Point3 bl(mybox.lo.x[0],mybox.lo.x[1], mybox.hi.x[2]);
		csg::Point3 br(mybox.hi.x[0],mybox.lo.x[1], mybox.hi.x[2]);
		csg::Point3 tr(mybox.hi.x[0],mybox.hi.x[1], mybox.hi.x[2]);

		csg::Point3 tlz(mybox.lo.x[0],mybox.hi.x[1], mybox.lo.x[2]);
		csg::Point3 blz(mybox.lo.x[0],mybox.lo.x[1], mybox.lo.x[2]);
		csg::Point3 brz(mybox.hi.x[0],mybox.lo.x[1], mybox.lo.x[2]);
		csg::Point3 trz(mybox.hi.x[0],mybox.hi.x[1], mybox.lo.x[2]);

		// // csg::Point3 tl(mybox.lo.x[0],mybox.hi.x[1]);
		// // csg::Point3 bl(mybox.lo.x[0],mybox.lo.x[1]);


		// mywindow->add_edge(LookingGlass::OwnedPoint(tl.x[0],tl.x[1],tl.x[2]), LookingGlass::OwnedPoint(bl.x[0],bl.x[1],bl.x[2]));
		// mywindow->add_edge(LookingGlass::OwnedPoint(bl.x[0],bl.x[1],bl.x[2]), LookingGlass::OwnedPoint(br.x[0],br.x[1],br.x[2]));
		// mywindow->add_edge(LookingGlass::OwnedPoint(br.x[0],br.x[1],br.x[2]), LookingGlass::OwnedPoint(tr.x[0],tr.x[1],tr.x[2]));
		// mywindow->add_edge(LookingGlass::OwnedPoint(tr.x[0],tr.x[1],tr.x[2]), LookingGlass::OwnedPoint(tl.x[0],tl.x[1],tl.x[2]));
		
		// mywindow->add_edge(LookingGlass::OwnedPoint(tlz.x[0],tlz.x[1],tlz.x[2]), LookingGlass::OwnedPoint(blz.x[0],blz.x[1],blz.x[2]));
		// mywindow->add_edge(LookingGlass::OwnedPoint(blz.x[0],blz.x[1],blz.x[2]), LookingGlass::OwnedPoint(brz.x[0],brz.x[1],brz.x[2]));
		// mywindow->add_edge(LookingGlass::OwnedPoint(brz.x[0],brz.x[1],brz.x[2]), LookingGlass::OwnedPoint(trz.x[0],trz.x[1],trz.x[2]));
		// mywindow->add_edge(LookingGlass::OwnedPoint(trz.x[0],trz.x[1],trz.x[2]), LookingGlass::OwnedPoint(tlz.x[0],tlz.x[1],tlz.x[2]));


		// mywindow->add_edge(LookingGlass::OwnedPoint(tl.x[0],tl.x[1],tl.x[2]), LookingGlass::OwnedPoint(tlz.x[0],tlz.x[1],tlz.x[2]));
		// mywindow->add_edge(LookingGlass::OwnedPoint(bl.x[0],bl.x[1],bl.x[2]), LookingGlass::OwnedPoint(blz.x[0],blz.x[1],blz.x[2]));
		// mywindow->add_edge(LookingGlass::OwnedPoint(br.x[0],br.x[1],br.x[2]), LookingGlass::OwnedPoint(brz.x[0],brz.x[1],brz.x[2]));
		// mywindow->add_edge(LookingGlass::OwnedPoint(tr.x[0],tr.x[1],tr.x[2]), LookingGlass::OwnedPoint(trz.x[0],trz.x[1],trz.x[2]));


		csg::Point3 ctr = 1.0/8.0*(tl+tlz+tr+trz+bl+blz+br+brz);
		mywindow->add_point(LookingGlass::OwnedPoint(ctr.x[0], ctr.x[1], ctr.x[2]));

		// std::cout << "added point" << std::endl;
	}

	std::cout << "added a bunch of edges" << std::endl;


	// // get geometry triangulation
	// std::shared_ptr<csg::Triangulation<3>> triang = std::make_shared<csg::Triangulation<3>>(obj->get_triangulation(500));

	// rgb col = Color::DodgerBlue();
	// double alpha = 0.9;
	// for (auto i=0; i<triang->points.size(); i++){
	// 	mywindow->add_point(LookingGlass::OwnedPoint(triang->points[i].x[0], triang->points[i].x[1], triang->points[i].x[2]));
	// 	mywindow->set_vertex_color(i, LookingGlass::OwnedColor(col.R, col.G, col.B, alpha));
	// }
	// for (auto i=0; i<triang->triangles.size(); i++){
	// 		// display the object as triangles
	// 		mywindow->set_triangle_element(triang->triangles[i].x[0], triang->triangles[i].x[1], triang->triangles[i].x[2]);
			
	// 		// display the "skeleton" of the object ... the edges
	// 		// mywindow->set_edge_element(triang->triangles[i].x[0], triang->triangles[i].x[1]);
	// 		// mywindow->set_edge_element(triang->triangles[i].x[1], triang->triangles[i].x[2]);
	// 		// mywindow->set_edge_element(triang->triangles[i].x[2], triang->triangles[i].x[0]);
	
	// 		// mywindow->set_edge_element(triang.triangles[i].x[0], triang.triangles[i].x[1]);
	// 		// mywindow->set_edge_element(triang.triangles[i].x[1], triang.triangles[i].x[2]);
	// 		// mywindow->set_edge_element(triang.triangles[i].x[2], triang.triangles[i].x[0]);

	// }


	mywindow->calculate_bounds();
	mywindow->run();
	//cout << "about to delete mywindow" << endl;
	delete mywindow;

	return 0;

}

