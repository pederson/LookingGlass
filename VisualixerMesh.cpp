#include "VisualixerMesh.hpp"

using namespace std;

//#define _TEST_
vector<visualixer*> _vMeshInstances;

//*************************************** Mesh Visualixer Class *******************************************
mesh_visualixer::mesh_visualixer(){

	_vMeshInstances.push_back(this);

	visualixer_active = false;
	window_name = "Mesh Visualixer";
	rotation_lock = false;
	_colorby = nullptr;
	_color_alpha = nullptr;
	vertices = NULL;
	elements = NULL;

	num_vertices = 0;
	num_per_vertex = 0;

	_num_point_elements = 0;
	_num_line_elements = 0;
	_num_tri_elements = 0;
	_num_quad_elements = 0;

	model_centroid[0] = 0.0;
	model_centroid[1] = 0.0;
	model_centroid[2] = 0.0;
}

mesh_visualixer::~mesh_visualixer(){
	for (unsigned int i=0; i<_vMeshInstances.size(); i++){
		if (_vMeshInstances.at(i) == this){
			_vMeshInstances.erase(_vMeshInstances.begin() + i);
			return;
		}
	}

	if (vertices != NULL) delete[] vertices;
	if (elements != NULL) delete[] elements;
	if (_colorby != nullptr) delete[] _colorby;
	if (_color_alpha != nullptr) delete[] _color_alpha;

}

void mesh_visualixer::bind_mesh(const Mesh & mesh){

	_mesh = &mesh;

	xmax = _mesh->xmax();
	ymax = _mesh->ymax();
	zmax = _mesh->zmax();
	xmin = _mesh->xmin();
	ymin = _mesh->ymin();
	zmin = _mesh->zmin();
	//GLfloat scale = (xmax-xmin)/1.0;

	num_vertices = _mesh->nodecount();
	num_per_vertex = 7;
	num_vertex_points = 3;
	

	return;
}


void mesh_visualixer::set_test_case(){
	num_vertices = 100;
	num_per_vertex = 7;
	num_vertex_points = 3;
	vertices = new GLfloat[num_vertices*num_per_vertex];
	for (unsigned int i=0; i<10; i++){
		for (unsigned int j=0; j<10; j++){
			vertices[(i*10+j)*num_per_vertex] = GLfloat(i);
			vertices[(i*10+j)*num_per_vertex + 1] = GLfloat(j);
			vertices[(i*10+j)*num_per_vertex + 2] = 10.0;
			if (j%2==0){
				vertices[(i*10+j)*num_per_vertex + 3] = 1.0f;
				vertices[(i*10+j)*num_per_vertex + 4] = 0.0f;
				vertices[(i*10+j)*num_per_vertex + 5] = 0.0f;
			}
			else if (j%3==0){
				vertices[(i*10+j)*num_per_vertex + 3] = 0.0f;
				vertices[(i*10+j)*num_per_vertex + 4] = 1.0f;
				vertices[(i*10+j)*num_per_vertex + 5] = 0.0f;
			}
			else {
				vertices[(i*10+j)*num_per_vertex + 3] = 1.0f;
				vertices[(i*10+j)*num_per_vertex + 4] = 1.0f;
				vertices[(i*10+j)*num_per_vertex + 5] = 1.0f;
			}
			vertices[(i*10+j)*num_per_vertex + 6] = 1.0f;
		}
	}

	_num_point_elements = num_vertices;
	_num_line_elements = 18;
	elements = new GLuint[_num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element];
	unsigned int line_element_offset = _num_point_elements*_num_per_point_element;
	// set the point elements
	for (unsigned int i=0; i<num_vertices; i++){
		elements[i] = i;
	}
	// add in the line elements
	for (unsigned int i=0; i<9; i++){
		elements[line_element_offset + i*_num_per_line_element] = i;
		elements[line_element_offset + i*_num_per_line_element+1] = (i+1);
	}
	for (unsigned int i=0; i<9; i++){
		elements[line_element_offset + (i+9)*_num_per_line_element] = i*10;
		elements[line_element_offset + (i+9)*_num_per_line_element + 1] = (i+1)*10;
	}


	model_centroid[0] = 4.5;
	model_centroid[1] = 4.5;
	model_centroid[2] = 10.0;
	xmax = num_vertices-1;
	ymax = num_vertices-1;
	zmax = 10.0;
	xmin = 0;
	ymin = 0;
	zmin = 0;

	return;
}

void mesh_visualixer::onPrepareData(){
	if (_mesh->num_dims() == 1 || _mesh->num_dims() == 2){
		MeshNode nd;
		MeshElement elem;

		vertices = new GLfloat[num_vertices*num_per_vertex];
		for (unsigned int i=0; i<num_vertices; i++){
			nd = _mesh->node(i);

			vertices[i*num_per_vertex] = nd.x();
			vertices[i*num_per_vertex + 1] = nd.y();
			vertices[i*num_per_vertex + 2] = nd.z();
			if (nd.boundary()){
				vertices[i*num_per_vertex + 3] = 1.0f;
				vertices[i*num_per_vertex + 4] = 0.0f;
				vertices[i*num_per_vertex + 5] = 0.0f;
			}
			else {
				vertices[i*num_per_vertex + 3] = 1.0f;
				vertices[i*num_per_vertex + 4] = 1.0f;
				vertices[i*num_per_vertex + 5] = 1.0f;
			}
			vertices[i*num_per_vertex + 6] = 1.0f;
		}

		// figure out how many line elements are needed
		// DYLAN_TODO: this should really be more rigorous and count for each element
		//				in the mesh
		_num_line_elements = 0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			_num_line_elements += elem.num_vertices();
		}

		_num_quad_elements = 0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			if (elem.num_vertices() == 4) _num_quad_elements++;
		}

		_num_tri_elements = 0;
		_num_tri_elements = 2*_num_quad_elements;
		/*
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			if (elem.num_vertices() == 3) _num_tri_elements++;
		}
		cout << "found " << _num_tri_elements << " TRI elements" << endl;
		*/

		_num_point_elements = num_vertices;

		elements = new GLuint[_num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element + _num_tri_elements*_num_per_tri_element + _num_quad_elements*_num_per_quad_element];
		// add point elements
		for (unsigned int i=0; i<num_vertices; i++){
			elements[i] = i;
		}

		// add line elements
		unsigned int line_element_offset = _num_point_elements*_num_per_point_element;
		unsigned int jp1, elements_added=0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			for (unsigned int j=0; j<elem.num_vertices(); j++){
				jp1 = (j+1)%elem.num_vertices();
				elements[line_element_offset + elements_added*_num_per_line_element] = elem.vertex_ind(j);
				elements[line_element_offset + elements_added*_num_per_line_element + 1] = elem.vertex_ind(jp1);

				elements_added++;
			}
		}

		// add tri elements
		unsigned int tri_element_offset = _num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element;
		unsigned int tri_elements_added=0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			if (elem.num_vertices() != 4) continue;
			elements[tri_element_offset + tri_elements_added*_num_per_tri_element] = elem.vertex_ind(0);
			elements[tri_element_offset + tri_elements_added*_num_per_tri_element + 1] = elem.vertex_ind(1);
			elements[tri_element_offset + tri_elements_added*_num_per_tri_element + 2] = elem.vertex_ind(2);
			tri_elements_added++;
			elements[tri_element_offset + tri_elements_added*_num_per_tri_element] = elem.vertex_ind(0);
			elements[tri_element_offset + tri_elements_added*_num_per_tri_element + 1] = elem.vertex_ind(3);
			elements[tri_element_offset + tri_elements_added*_num_per_tri_element + 2] = elem.vertex_ind(2);
			tri_elements_added++;
		}

		model_centroid[0] = (xmax + xmin)/2.0;
		model_centroid[1] = (ymax + ymin)/2.0;
		model_centroid[2] = (zmax + zmin)/2.0;
	}
	else if (_mesh->num_dims() == 3){
		//cout << "STARTING 3 DIMS" << endl;

		model_centroid[0] = (xmax + xmin)/2.0;
		model_centroid[1] = (ymax + ymin)/2.0;
		model_centroid[2] = (zmax + zmin)/2.0;

		MeshNode nd;
		MeshElement elem;

		m_xslice = model_centroid[0];
		m_yslice = model_centroid[1];
		m_zslice = model_centroid[2];
		/*
		const RegularMesh * rmesh = dynamic_cast<const RegularMesh *>(_mesh);
		m_xwidth = rmesh->res();
		m_ywidth = rmesh->res();
		m_zwidth = rmesh->res();
		*/
		// heuristic calculation
		double nx, ny, nz, rho, xvol, yvol, zvol;
		xvol = xmax-xmin;
		yvol = ymax-ymin;
		zvol = zmax-zmin;
		rho = _mesh->nodecount()/(xvol*yvol*zvol);

		m_xwidth = xvol/(rho*yvol*zvol);
		m_ywidth = yvol/(rho*xvol*zvol);
		m_zwidth = zvol/(rho*xvol*yvol);

		m_xwidth = 1.5*pow(1.0/rho, 1.0/3.0);
		m_ywidth = 1.5*m_xwidth;
		m_zwidth = 1.5*m_xwidth;
		//m_xwidth = pow(m_xwidth, 1.0/3.0);
		//m_ywidth = pow(m_ywidth, 1.0/3.0);
		//m_zwidth = pow(m_zwidth, 1.0/3.0);
		
		//cout << "CALCULATED THE WIDTHS: ";
		//cout << "xw: " << m_xwidth << endl;
		//cout << "yw: " << m_ywidth << endl;
		//cout << "zw: " << m_zwidth << endl;

		// find which points in the x, y plane
		m_subset.assign(_mesh->nodecount(), true);
		/*
		Point a1(m_xslice-m_xwidth/2, ymax);
		Point a2(m_xslice+m_xwidth/2, ymax);
		Point a3(m_xslice+m_xwidth/2, ymin);
		Point a4(m_xslice-m_xwidth/2, ymin);
		Hull hxy({a1, a2, a3, a4});
		for (auto i=0; i<_mesh->nodecount(); i++){
			nd = _mesh->node(i);
			if (hxy.contains_point(Point(nd.x(), nd.y()))) m_subset.at(i) = true;
		}
		//cout << "FINISHED XY plane" << endl;

		
		// find which points in the y, z plane
		Point b1(m_zslice-m_zwidth/2, ymax);
		Point b2(m_zslice+m_zwidth/2, ymax);
		Point b3(m_zslice+m_zwidth/2, ymin);
		Point b4(m_zslice-m_zwidth/2, ymin);
		Hull hyz({b1, b2, b3, b4});
		for (auto i=0; i<_mesh->nodecount(); i++){
			nd = _mesh->node(i);
			if (hyz.contains_point(Point(nd.z(), nd.y()))) m_subset.at(i) = true;
		}
		//cout << "FINISHED YZ plane" << endl;
		

		// find which points in the y, x plane
		Point c1(m_yslice+m_ywidth/2, xmax);
		Point c2(m_yslice-m_ywidth/2, xmax);
		Point c3(m_yslice-m_ywidth/2, xmin);
		Point c4(m_yslice+m_ywidth/2, xmin);
		Hull hyx({c1, c2, c3, c4});
		for (auto i=0; i<_mesh->nodecount(); i++){
			nd = _mesh->node(i);
			if (hyx.contains_point(Point(nd.y(), nd.x()))) m_subset.at(i) = true;
		}
		//cout << "FINISHED YX plane" << endl;
		*/
		

		// find the number of true points
		num_vertices = 0;
		for (auto i=0; i<_mesh->nodecount(); i++){
			if (m_subset.at(i)) num_vertices++;
		}
		vertices = new GLfloat[num_vertices*num_per_vertex];
		unsigned int vct=0;
		for (unsigned int i=0; i<_mesh->nodecount(); i++){
			if (!m_subset.at(i)) continue;
			nd = _mesh->node(i);

			vertices[vct*num_per_vertex] = nd.x();
			vertices[vct*num_per_vertex + 1] = nd.y();
			vertices[vct*num_per_vertex + 2] = nd.z();
			if (nd.boundary()){
				vertices[vct*num_per_vertex + 3] = 1.0f;
				vertices[vct*num_per_vertex + 4] = 0.0f;
				vertices[vct*num_per_vertex + 5] = 0.0f;
			}
			else {
				vertices[vct*num_per_vertex + 3] = 1.0f;
				vertices[vct*num_per_vertex + 4] = 1.0f;
				vertices[vct*num_per_vertex + 5] = 1.0f;
			}
			vertices[vct*num_per_vertex + 6] = 1.0f;
			vct++;
		}
		//cout << "FINISHED FILLING VERTICES" << endl;

		//////////// ELEMENTS ///////////////////////
		m_subset.assign(_mesh->nodecount(), true);
		
		// count line elements
		_num_line_elements = 0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			if (elem.type() == ElementType::LINE_2) _num_line_elements++;
		}

		// cast the tri elements as 3 line elements
		_num_tri_elements = 0;
		unsigned int tri_line_elements=0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			if (elem.type() == ElementType::TRI_3) tri_line_elements+=3;
		}

		// cast the tetrahedral elements as 6 line elements
		unsigned int tet_line_elements=0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			if (elem.type() == ElementType::TET_4) tet_line_elements+=6;
		}


		// cout << "line elemetns: " << _num_line_elements << endl;
		// cout << "tri line elements: " << tri_line_elements << endl;


		_num_point_elements = num_vertices;

		elements = new GLuint[_num_point_elements*_num_per_point_element + (_num_line_elements + tri_line_elements + tet_line_elements)*_num_per_line_element];
		unsigned int line_element_offset = _num_point_elements*_num_per_point_element;
		unsigned int tri_line_element_offset = _num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element;
		unsigned int tet_line_element_offset = _num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element + tri_line_elements*_num_per_line_element;
		// set the point elements
		for (unsigned int i=0; i<num_vertices; i++){
			elements[i] = i;
		}

		// set the line elements
		unsigned int lct = 0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			if (elem.type() == ElementType::LINE_2){
				elements[line_element_offset + 2*lct] = elem.vertex_ind(0);
				elements[line_element_offset + 2*lct+1] = elem.vertex_ind(1);
				lct++;
			}
		}

		// set the tri line elements
		unsigned int tct = 0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			if (elem.type() == ElementType::TRI_3){
				elements[tri_line_element_offset + 6*tct] = elem.vertex_ind(0);
				elements[tri_line_element_offset + 6*tct+1] = elem.vertex_ind(1);
				elements[tri_line_element_offset + 6*tct+2] = elem.vertex_ind(1);
				elements[tri_line_element_offset + 6*tct+3] = elem.vertex_ind(2);
				elements[tri_line_element_offset + 6*tct+4] = elem.vertex_ind(2);
				elements[tri_line_element_offset + 6*tct+5] = elem.vertex_ind(0);
				tct++;
			}
		}

		// set the tri line elements
		unsigned int tect = 0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			if (elem.type() == ElementType::TET_4){
				elements[tri_line_element_offset + 12*tect] = elem.vertex_ind(0);
				elements[tri_line_element_offset + 12*tect+1] = elem.vertex_ind(1);
				elements[tri_line_element_offset + 12*tect+2] = elem.vertex_ind(0);
				elements[tri_line_element_offset + 12*tect+3] = elem.vertex_ind(2);
				elements[tri_line_element_offset + 12*tect+4] = elem.vertex_ind(0);
				elements[tri_line_element_offset + 12*tect+5] = elem.vertex_ind(3);
				elements[tri_line_element_offset + 12*tect+6] = elem.vertex_ind(2);
				elements[tri_line_element_offset + 12*tect+7] = elem.vertex_ind(3);
				elements[tri_line_element_offset + 12*tect+8] = elem.vertex_ind(1);
				elements[tri_line_element_offset + 12*tect+9] = elem.vertex_ind(2);
				elements[tri_line_element_offset + 12*tect+10] = elem.vertex_ind(1);
				elements[tri_line_element_offset + 12*tect+11] = elem.vertex_ind(3);
				tect++;
			}
		}

		_num_line_elements += tri_line_elements + tet_line_elements;

	}

}

void mesh_visualixer::onColors(){
	if (_colorby == nullptr) return;
	if (_colorby_max - _colorby_min == 0.0) return;
	// modify the vertex array to incorporate user-defined colors

	rgb ptcolor;
	if (m_subset.size() > 0){
		unsigned int vct = 0;
		for (auto i=0; i<_mesh->nodecount(); i++){
			if (! m_subset.at(i)) continue;
			ptcolor = color_ramp.get_ramp_color(float(_colorby[i]));
			vertices[(vct+1)*num_per_vertex - 4] = ptcolor.R;
			vertices[(vct+1)*num_per_vertex - 3] = ptcolor.G;
			vertices[(vct+1)*num_per_vertex - 2] = ptcolor.B;
			vct++;
		}
	}
	else{
		for (auto i=0; i<num_vertices; i++){
			//ptcolor = color_ramp.get_ramp_color(float(_colorby[i]));
			ptcolor = color_ramp.get_ramp_color(float(log(1.0+_colorby[i])/log(2.0)));
			vertices[(i+1)*num_per_vertex - 4] = ptcolor.R;
			vertices[(i+1)*num_per_vertex - 3] = ptcolor.G;
			vertices[(i+1)*num_per_vertex - 2] = ptcolor.B;
		}
	}


	return;
}


void mesh_visualixer::onAlpha(){
	if (_color_alpha == nullptr) return;

	
	for (auto i=0; i<num_vertices; i++){
		vertices[(i+1)*num_per_vertex - 1] = _color_alpha[i];
	}

}


#ifdef _TEST_
// use cmake to compile

#include "RegularMesh.hpp"

int main(int argc, char * argv[]){
	// declare vars

	// test the mesh viewer
	mesh_visualixer mymvis;
	RegularMesh mesh = RegularMesh::create_regular_grid_n(0.1, 50, 50);//, (unsigned int)30);
	mymvis.bind_mesh(mesh);
	mymvis.set_color_ramp(CRamp::DIVERGENT_9);
	mymvis.set_colorby(&mesh.x());
	//mymvis->set_test_case();
	
	mymvis.run();

	return 0;

}

#endif