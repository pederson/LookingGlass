#include "VisualixerSimulation.hpp"

using namespace std;

vector<visualixer*> _vSimulationInstances;

simulation_visualixer::simulation_visualixer(){

	_vSimulationInstances.push_back(this);

	visualixer_active = false;
	window_name = "Simulation Visualixer";
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

	_freq_Hz = 30;
	_colorby_field = "";
	_colorby_field_index = 0;
	_alpha_field = "";
	_cur_time_step = 0;
	_increment_val = 1;
	set_color_ramp(CRamp::MATLAB_PARULA);

	model_centroid[0] = 0.0;
	model_centroid[1] = 0.0;
	model_centroid[2] = 0.0;

	m_display_colorbar = false;

}

simulation_visualixer::simulation_visualixer(const SimulationData & simdat){
	_vSimulationInstances.push_back(this);

	visualixer_active = false;
	window_name = "Simulation Visualixer";
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

	_freq_Hz = 30;
	_colorby_field = "";
	_colorby_field_index = 0;
	_alpha_field = "";
	_cur_time_step = 0;
	_increment_val = 1;
	set_color_ramp(CRamp::MATLAB_PARULA);

	model_centroid[0] = 0.0;
	model_centroid[1] = 0.0;
	model_centroid[2] = 0.0;

	m_display_colorbar = false;

	bind_simulation(simdat);
}

simulation_visualixer::~simulation_visualixer(){
	for (unsigned int i=0; i<_vSimulationInstances.size(); i++){
		if (_vSimulationInstances.at(i) == this){
			_vSimulationInstances.erase(_vSimulationInstances.begin() + i);
			return;
		}
	}

	//delete[] window_name;
	if (vertices != NULL) delete[] vertices;
	if (elements != NULL) delete[] elements;
	if (_color_alpha != nullptr) delete[] _color_alpha;
	if (_colorby != nullptr) delete[] _colorby;

}

void simulation_visualixer::bind_simulation(const SimulationData & simdata){
	_simdata = &simdata;

	_mesh = &_simdata->mesh();

	xmax = _mesh->xmax();
	ymax = _mesh->ymax();
	zmax = _mesh->zmax();
	xmin = _mesh->xmin();
	ymin = _mesh->ymin();
	zmin = _mesh->zmin();

	num_vertices = _mesh->nodecount();
	num_per_vertex = 7;
	num_vertex_points = 3;

	set_colorby_field(simdata.nodefields().at(0));
}


void simulation_visualixer::set_snapshot_increment(unsigned int inc){
	_increment_val = inc;
}

void simulation_visualixer::set_colorby_field(std::string fieldname){
	_colorby_field = fieldname;
}

void simulation_visualixer::set_alpha_field(std::string fieldname){
	_alpha_field = fieldname;
}

void simulation_visualixer::onKeyF(){
	// cycle the field being displayed
	_colorby_field_index = (++_colorby_field_index)%_simdata->fieldnames().size();
	_colorby_field = _simdata->nodefields().at(_colorby_field_index);
	cout << "Displaying field: " << _colorby_field;
	

	// find the max/min over all the time steps for this new field
	const double * dat = &(_simdata->get_data_at_index(0, _colorby_field));
	_colorby_max = dat[0]; _colorby_min = dat[0];
	for (auto t=0; t<_simdata->num_time_steps(); t++){
		dat = &(_simdata->get_data_at_index(t, _colorby_field));
		for (auto i=0; i<_mesh->nodecount(); i++){
			if (dat[i] > _colorby_max) _colorby_max = dat[i];
			if (dat[i] < _colorby_min) _colorby_min = dat[i];
		}
	}
	unsigned int ntrue = num_vertices;
	num_vertices = _mesh->nodecount();
	set_colorby(&(_simdata->get_data_at_index(0, _colorby_field)), false);
	num_vertices = ntrue;

	cout << "  minval: " << _colorby_min << "  maxval: " << _colorby_max << endl;

	// bool cbarstatus = m_display_colorbar;
	// m_display_colorbar = false;
	onColors();
	onAlpha();
	onRefresh();
	// m_display_colorbar = cbarstatus;
}

void simulation_visualixer::increment_time_step(){
	//_increment_val = 10;
	_cur_time_step = (_cur_time_step+_increment_val)%_simdata->num_time_steps();
	unsigned int ntrue = num_vertices;
	num_vertices = _mesh->nodecount();
	set_colorby(&(_simdata->get_data_at_index(_cur_time_step, _colorby_field)), false);
	if (_alpha_field.compare("") != 0) set_color_alpha(&(_simdata->get_data_at_index(_cur_time_step, _alpha_field)), false);
	num_vertices = ntrue;
	
	bool cbarstatus = m_display_colorbar;
	m_display_colorbar = false;
	onColors();
	onAlpha();
	onRefresh();
	m_display_colorbar = cbarstatus;
	
}

// void simulation_visualixer::run(){
// 	onInit();
// 	onPrepareData();
// 	onColors();
// 	onAlpha();
// 	onRender();
// 	onShaders();
// 	MainLoop();
// 	onExit();
// 	return;
// }

void simulation_visualixer::onPrepareData(){
	if (_mesh->num_dims() == 1 || _mesh->num_dims() == 2){
		const Mesh * themesh = _simdata->mesh();
		MeshNode nd;
		MeshElement elem;


		vertices = new GLfloat[num_vertices*num_per_vertex];
		for (unsigned int i=0; i<num_vertices; i++){
			nd = themesh->node(i);

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

		/*
		// figure out how many line elements are needed
		// DYLAN_TODO: this should really be more rigorous and count for each element
		//				in the mesh
		_num_line_elements = 0;
		for (unsigned int i=0; i<themesh->elementcount(); i++){
			elem = themesh->element(i);
			_num_line_elements += elem.num_vertices();
		}

		_num_quad_elements = 0;
		for (unsigned int i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			if (elem.num_vertices() == 4) _num_quad_elements++;
		}

		_num_tri_elements = 0;
		_num_tri_elements = 2*_num_quad_elements;

		_num_point_elements = num_vertices;

		elements = new GLuint[_num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element];
		// add point elements
		for (unsigned int i=0; i<num_vertices; i++){
			elements[i] = i;
		}

		// add line elements
		unsigned int line_element_offset = _num_point_elements*_num_per_point_element;
		unsigned int jp1, elements_added=0;
		for (unsigned int i=0; i<themesh->elementcount(); i++){
			elem = themesh->element(i);
			for (unsigned int j=0; j<elem.num_vertices(); j++){
				jp1 = (j+1)%elem.num_vertices();
				elements[line_element_offset + elements_added*_num_per_line_element] = elem.vertex_ind(j);
				elements[line_element_offset + elements_added*_num_per_line_element + 1] = elem.vertex_ind(jp1);

				elements_added++;
			}
		}
		*/


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

		if (_colorby_field.compare("") != 0){
			// set colorby max and min
			
			const double * dat = &(_simdata->get_data_at_index(0, _colorby_field));
			_colorby_max = dat[0]; _colorby_min = dat[0];
			for (auto t=0; t<_simdata->num_time_steps(); t++){
				dat = &(_simdata->get_data_at_index(t, _colorby_field));
				for (auto i=0; i<num_vertices; i++){
					if (dat[i] > _colorby_max) _colorby_max = dat[i];
					if (dat[i] < _colorby_min) _colorby_min = dat[i];
				}
			}
			
			set_colorby(&(_simdata->get_data_at_index(0, _colorby_field)), false);
			//cout << "set min and max colorby" << endl;
			//cout << "max: " << colorby_max << "\t min: " << colorby_min << endl;
		} 
		if (_alpha_field.compare("") != 0){
			set_color_alpha(&(_simdata->get_data_at_index(0, _alpha_field)));
		}
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

		// heuristic calculation
		double nx, ny, nz, rho, xvol, yvol, zvol;
		xvol = xmax-xmin;
		yvol = ymax-ymin;
		zvol = zmax-zmin;
		rho = _mesh->nodecount()/(xvol*yvol*zvol);

		m_xwidth = xvol/(rho*yvol*zvol);
		m_ywidth = yvol/(rho*xvol*zvol);
		m_zwidth = zvol/(rho*xvol*yvol);

		m_xwidth = 1.25*pow(1.0/rho, 1.0/3.0);
		m_ywidth = 1.25*m_xwidth;
		m_zwidth = 1.25*m_xwidth;
		//m_xwidth = pow(m_xwidth, 1.0/3.0);
		//m_ywidth = pow(m_ywidth, 1.0/3.0);
		//m_zwidth = pow(m_zwidth, 1.0/3.0);
		
		//cout << "CALCULATED THE WIDTHS: ";
		//cout << "xw: " << m_xwidth << endl;
		//cout << "yw: " << m_ywidth << endl;
		//cout << "zw: " << m_zwidth << endl;

		num_vertices = _mesh->nodecount();
		vertices = new GLfloat[num_vertices*num_per_vertex];
		for (auto i=0; i<_mesh->nodecount(); i++){
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

		// Now fill in the correct elements for slices
		_num_point_elements = 0;
		_num_tri_elements = 0;
		unsigned int nneighbs;

		// Slice of constant X
		Point a1(m_xslice-m_xwidth/2, zmin);
		Point a2(m_xslice+m_xwidth/2, zmin);
		Point a3(m_xslice+m_xwidth/2, zmax);
		Point a4(m_xslice-m_xwidth/2, zmax);
		Hull hxy({a1, a2, a3, a4});

		// Slice of constant Y
		Point c1(m_yslice-m_ywidth/2, zmin);
		Point c2(m_yslice+m_ywidth/2, zmin);
		Point c3(m_yslice+m_ywidth/2, zmax);
		Point c4(m_yslice-m_ywidth/2, zmax);
		Hull hyx({c1, c2, c3, c4});

		// Slice of constant Z
		Point b1(ymin, m_zslice-m_zwidth/2);
		Point b2(ymax, m_zslice-m_zwidth/2);
		Point b3(ymax, m_zslice+m_zwidth/2);
		Point b4(ymin, m_zslice+m_zwidth/2);
		Hull hyz({b1, b2, b3, b4});

		m_subset.assign(_mesh->nodecount(), false);
		vector<bool> ptelemvec(m_subset);

		

		// Point elements for X slice
		for (auto i=0; i<_mesh->nodecount(); i++){
			nd = _mesh->node(i);
			if (hxy.contains_point(Point(nd.x(), nd.z()))){
				m_subset.at(i) = true;
				ptelemvec.at(i) = true;
				_num_point_elements++;
			}
		}
		// Triangle elements for X slice
		for (auto i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			nneighbs=0;
			for (auto j=0; j<elem.num_vertices(); j++){
				if (m_subset.at(elem.vertex_ind(j))) nneighbs++;
			}
			if (nneighbs==4) _num_tri_elements += 4;
		}
		//cout << "FINISHED X SLICE" << endl;

		// Point elements for Y slice
		m_subset.assign(_mesh->nodecount(), false);
		for (auto i=0; i<_mesh->nodecount(); i++){
			nd = _mesh->node(i);
			if (hyx.contains_point(Point(nd.y(), nd.z()))) {
				m_subset.at(i) = true;
				ptelemvec.at(i) = true;
				_num_point_elements++;
			}
		}
		// Triangle elements for Y slice
		for (auto i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			nneighbs=0;
			for (auto j=0; j<elem.num_vertices(); j++){
				if (m_subset.at(elem.vertex_ind(j))) nneighbs++;
			}
			if (nneighbs==4) _num_tri_elements += 4;
		}
		//cout << "FINISHED Y SLICE" << endl;
		
		// Point elements for Z slice
		m_subset.assign(_mesh->nodecount(), false);
		for (auto i=0; i<_mesh->nodecount(); i++){
			nd = _mesh->node(i);
			if (hyz.contains_point(Point(nd.y(), nd.z()))){ 
				m_subset.at(i) = true;
				ptelemvec.at(i) = true;
				_num_point_elements++;
			}
		}
		
		// Triangle elements for Z slice
		for (auto i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			nneighbs=0;
			for (auto j=0; j<elem.num_vertices(); j++){
				if (m_subset.at(elem.vertex_ind(j))) nneighbs++;
			}
			if (nneighbs==4) _num_tri_elements += 4;
		}
		//cout << "FINISHED YZ plane" << endl;

		// final count for point elements
		_num_point_elements = 0;
		for (auto i=0; i<_mesh->nodecount(); i++){
			if (ptelemvec.at(i)) _num_point_elements++;
		}
	
		
		// Go back and actually add vertices and elements
		_num_line_elements = 0;
		//_num_tri_elements = 0;
		
		elements = new GLuint[_num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element + _num_tri_elements*_num_per_tri_element];

		unsigned int vct=0;
		// set the point elements
		//cout << " THERE ARE " << _num_point_elements << " POINT ELEMENTS" << endl;
		for (unsigned int i=0; i<_mesh->nodecount(); i++){
			if (!ptelemvec.at(i)) continue;
			elements[vct] = i;
			vct++;
		}

		m_subset.clear();

		unsigned int trielemct=0;
		unsigned int tri_element_offset = _num_point_elements*_num_per_point_element;
		
		// set the triangle elements
		m_subset.assign(_mesh->nodecount(), false);
		for (auto i=0; i<_mesh->nodecount(); i++){
			nd = _mesh->node(i);
			if (hxy.contains_point(Point(nd.x(), nd.z()))){
				m_subset.at(i) = true;
			}
		}
		// Connections for X slice
		for (auto i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			nneighbs=0;
			for (auto j=0; j<elem.num_vertices(); j++){
				if (m_subset.at(elem.vertex_ind(j))) nneighbs++;
			}
			if (nneighbs==4){
				unsigned int ect=0;
				vector<unsigned int> els(4);
				for (auto j=0; j<elem.num_vertices(); j++){
					if (m_subset.at(elem.vertex_ind(j))){
						els[ect] = elem.vertex_ind(j);
						ect++;
					}
				}
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[0];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[1];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[2];
				trielemct++;
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[1];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[2];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[3];
				trielemct++;
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[2];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[3];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[0];
				trielemct++;
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[3];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[0];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[1];
				trielemct++;
				
			}
		}
		
		//cout << "FINISHED X SLICE" << endl;

		m_subset.assign(_mesh->nodecount(), false);
		for (auto i=0; i<_mesh->nodecount(); i++){
			nd = _mesh->node(i);
			if (hyx.contains_point(Point(nd.y(), nd.z()))) {
				m_subset.at(i) = true;
			}
		}
		// Connections for Y slice
		for (auto i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			nneighbs=0;
			for (auto j=0; j<elem.num_vertices(); j++){
				if (m_subset.at(elem.vertex_ind(j))) nneighbs++;
			}
			if (nneighbs==4){
				unsigned int ect=0;
				vector<unsigned int> els(4);
				for (auto j=0; j<elem.num_vertices(); j++){
					if (m_subset.at(elem.vertex_ind(j))){
						els[ect] = elem.vertex_ind(j);
						ect++;
					}
				}
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[0];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[1];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[2];
				trielemct++;
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[1];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[2];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[3];
				trielemct++;
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[2];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[3];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[0];
				trielemct++;
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[3];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[0];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[1];
				trielemct++;
				
			}
		}
		
		//cout << "FINISHED Y SLICE" << endl;
		
		m_subset.assign(_mesh->nodecount(), false);
		for (auto i=0; i<_mesh->nodecount(); i++){
			nd = _mesh->node(i);
			if (hyz.contains_point(Point(nd.y(), nd.z()))){ 
				m_subset.at(i) = true;
			}
		}
		// Connections for Z slice
		for (auto i=0; i<_mesh->elementcount(); i++){
			elem = _mesh->element(i);
			nneighbs=0;
			for (auto j=0; j<elem.num_vertices(); j++){
				if (m_subset.at(elem.vertex_ind(j))) nneighbs++;
			}
			if (nneighbs==4){
				unsigned int ect=0;
				vector<unsigned int> els(4);
				for (auto j=0; j<elem.num_vertices(); j++){
					if (m_subset.at(elem.vertex_ind(j))){
						els[ect] = elem.vertex_ind(j);
						ect++;
					}
				}
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[0];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[1];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[2];
				trielemct++;
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[1];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[2];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[3];
				trielemct++;
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[2];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[3];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[0];
				trielemct++;
				elements[tri_element_offset + trielemct*_num_per_tri_element] = els[3];
				elements[tri_element_offset + trielemct*_num_per_tri_element+1] = els[0];
				elements[tri_element_offset + trielemct*_num_per_tri_element+2] = els[1];
				trielemct++;
				
			}
		}
		//*/
		


		//cout << "FINISHED POINT ELEMENTS" << endl;



		//cout << "ALOMOST DONE" << endl;
		if (_colorby_field.compare("") != 0){
			// set colorby max and min
			//cout << "COLORBY FIELD: " << _colorby_field << endl;
			
			const double * dat = &(_simdata->get_data_at_index(0, _colorby_field));
			_colorby_max = dat[0]; _colorby_min = dat[0];
			for (auto t=0; t<_simdata->num_time_steps(); t++){
				dat = &(_simdata->get_data_at_index(t, _colorby_field));
				for (auto i=0; i<_mesh->nodecount(); i++){
					if (dat[i] > _colorby_max) _colorby_max = dat[i];
					if (dat[i] < _colorby_min) _colorby_min = dat[i];
				}
			}
			//unsigned int ntrue = num_vertices;
			num_vertices = _mesh->nodecount();
			set_colorby(&(_simdata->get_data_at_index(0, _colorby_field)), false);
			//num_vertices = ntrue;
			//cout << "set min and max colorby" << endl;
			//cout << "max: " << colorby_max << "\t min: " << colorby_min << endl;
		} 
		if (_alpha_field.compare("") != 0){
			unsigned int ntrue = num_vertices;
			num_vertices = _mesh->nodecount();
			set_color_alpha(&(_simdata->get_data_at_index(0, _alpha_field)));
			//num_vertices = ntrue;
		}
		//cout << "YEP HERE" << endl;
	}

}

/*
void simulation_visualixer::onColors(){
	if (_colorby == nullptr) return;
	if (_colorby_max - _colorby_min == 0.0) return;
	// modify the vertex array to incorporate user-defined colors


	rgb ptcolor;
	for (auto i=0; i<num_vertices; i++){
		ptcolor = color_ramp.get_ramp_color(float((_colorby[i])/(_colorby_max - _colorby_min)));
		vertices[(i+1)*num_per_vertex - 4] = ptcolor.R;
		vertices[(i+1)*num_per_vertex - 3] = ptcolor.G;
		vertices[(i+1)*num_per_vertex - 2] = ptcolor.B;
	}


	return;
}
*/


void simulation_visualixer::onAlpha(){
	if (_color_alpha == nullptr) return;

	for (auto i=0; i<num_vertices; i++){
		vertices[(i+1)*num_per_vertex - 1] = _color_alpha[i];
	}

	// if (m_subset.size() > 0){
	// 	unsigned int vct=0;
	// 	for (auto i=0; i<_mesh->nodecount(); i++){
	// 		if (! m_subset.at(i)) continue;
	// 		vertices[(vct+1)*num_per_vertex - 1] = _color_alpha[i];
	// 		vct++;
	// 	}
	// }
	// else{
		
	// }

}

void simulation_visualixer::PreLoopTasks(){
	m_lastTime = glfwGetTime();
}

void simulation_visualixer::LoopTasks(){
	m_curTime = glfwGetTime();
  	if (m_curTime-m_lastTime >= 1.0/double(_freq_Hz)){ 
  		//cout << "I should be incrementing: " << curTime << "\r" << flush;
  		increment_time_step();
  		m_lastTime = m_curTime;
  	}
}


// bool simulation_visualixer::MainLoop(){


// 	double lastTime = glfwGetTime(), curTime;
//   while(!glfwWindowShouldClose(window_ptr)){

//   	curTime = glfwGetTime();
//   	if (curTime-lastTime >= 1.0/double(_freq_Hz)){ 
//   		//cout << "I should be incrementing: " << curTime << "\r" << flush;
//   		increment_time_step();
//   		lastTime = curTime;
//   	}
  	

//     if (glfwGetKey(window_ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window_ptr, GL_TRUE);

//     glfwSwapBuffers(window_ptr);
//     glfwPollEvents();

//     // Clear the screen to black
//     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
//     glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
//     glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

//     // Draw nodes
//     glDrawElements(GL_POINTS, _num_point_elements*_num_per_point_element , GL_UNSIGNED_INT, (void *)((0) * sizeof(GLuint)));
//     glDrawElements(GL_LINES, _num_line_elements*_num_per_line_element , GL_UNSIGNED_INT, (void *)((_num_point_elements*_num_per_point_element) * sizeof(GLuint)));
//     glDrawElements(GL_TRIANGLES, _num_tri_elements*_num_per_tri_element , GL_UNSIGNED_INT, (void *)((_num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element) * sizeof(GLuint)));
//     //cout << "looping \r" << flush;
// 	}

// 	return 0;
// }



