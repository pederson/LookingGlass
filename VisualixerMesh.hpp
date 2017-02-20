#ifndef _VISUALIXERMESH_H
#define _VISUALIXERMESH_H

#include "Mesh.hpp"
//#include "RegularMesh.hpp"
#include "Visualixer.hpp"
#include "Hull.hpp"


#include <iostream>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>

class mesh_visualixer : public visualixer{
public:
	mesh_visualixer();
	~mesh_visualixer();

	void bind_mesh(const Mesh & mesh);
	void set_test_case();

protected:

	void onPrepareData();
	void onColors();
	void onAlpha();

	const Mesh * _mesh;
	std::vector<bool> m_subset;
	double m_xwidth, m_ywidth, m_zwidth;
	double m_xslice, m_yslice, m_zslice;
};

#endif