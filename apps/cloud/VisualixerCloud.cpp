#include "VisualixerCloud.hpp"

using namespace std;

//#define _TEST_

vector<visualixer*> _vCloudInstances;


//*************************************** PointCloud Visualixer Class *******************************************
cloud_visualixer::cloud_visualixer(){

	_vCloudInstances.push_back(this);

	visualixer_active = false;
	window_name = "Cloud Visualixer";
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

cloud_visualixer::~cloud_visualixer(){
	for (unsigned int i=0; i<_vCloudInstances.size(); i++){
		if (_vCloudInstances.at(i) == this){
			_vCloudInstances.erase(_vCloudInstances.begin() + i);
			return;
		}
	}

	//delete[] window_name;
	//if (color_ramp != NULL) delete[] color_ramp;
	if (vertices != NULL) delete[] vertices;
	if (elements != NULL) delete[] elements;
	if (_color_alpha != nullptr) delete[] _color_alpha;
	if (_colorby != nullptr) delete[] _colorby;
}

void cloud_visualixer::set_test_case(){
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
			else {
				vertices[(i*10+j)*num_per_vertex + 3] = 1.0f;
				vertices[(i*10+j)*num_per_vertex + 4] = 1.0f;
				vertices[(i*10+j)*num_per_vertex + 5] = 1.0f;
			}
		}
	}

	_num_point_elements = num_vertices;
	elements = new GLuint[_num_point_elements*_num_per_point_element];
	for (unsigned int i=0; i<num_vertices; i++){
		elements[i] = i;
	}

	for (unsigned int i=0; i<num_vertices; i++){
		cout << "x: " << vertices[i*num_per_vertex] << " y: " << vertices[i*num_per_vertex +1] << " z: " << vertices[i*num_per_vertex+2] << endl;
	}

	model_centroid[0] = 4.5;
	model_centroid[1] = 4.5;
	model_centroid[2] = 0.0;
	xmax = num_vertices-1;
	ymax = num_vertices-1;
	zmax = 10.0;
	xmin = 0;
	ymin = 0;
	zmin = 0;

	return;
}

void cloud_visualixer::add_cloud(const PointCloud & cloud){
	_cloud = &cloud;
	num_vertices = _cloud->pointcount();
	num_per_vertex = 7;
	num_vertex_points = 3;
	

	return;
}

void cloud_visualixer::onPrepareData(){
	const double *x, *y, *z;
	x = &_cloud->x();
	y = &_cloud->y();
	z = &_cloud->z();
	vertices = new GLfloat[num_vertices*num_per_vertex];
	for (unsigned int i=0; i<num_vertices; i++){
		vertices[i*num_per_vertex] = x[i];
		vertices[i*num_per_vertex + 1] = y[i];
		vertices[i*num_per_vertex + 2] = z[i];

	}

	_num_point_elements = num_vertices;
	elements = new GLuint[_num_point_elements*_num_per_point_element];
	for (unsigned int i=0; i<num_vertices; i++){
		elements[i] = i;
	}

	
  if (_cloud->RGB_present()){
  	const rgb48 * RGB = &_cloud->RGB();
    for (unsigned int i=0; i<num_vertices; i){
      vertices[i*num_per_vertex + 3] = RGB[i].R/65,535;
      vertices[i*num_per_vertex + 4] = RGB[i].G/65,535;
      vertices[i*num_per_vertex + 5] = RGB[i].B/65,535;
      vertices[i*num_per_vertex + 6] = 1.0;
    }
  }
  else {
    //rgb ptcolor;
    for (unsigned int i=0; i<num_vertices; i++){
      //ptcolor = color_ramp.get_ramp_color((cloud->z[i]-cloud->zmin)/(cloud->zmax - cloud->zmin));
      vertices[i*num_per_vertex + 3] = 0.0;
      vertices[i*num_per_vertex + 4] = 0.0;
      vertices[i*num_per_vertex + 5] = 1.0;
      vertices[i*num_per_vertex + 6] = 1.0;
    }
  }

	model_centroid[0] = (_cloud->xmax() + _cloud->xmin())/2.0;
	model_centroid[1] = (_cloud->ymax() + _cloud->ymin())/2.0;
	model_centroid[2] = (_cloud->zmax() + _cloud->zmin())/2.0;
	xmax = _cloud->xmax();
	ymax = _cloud->ymax();
	zmax = _cloud->zmax();
	xmin = _cloud->xmin();
	ymin = _cloud->ymin();
	zmin = _cloud->zmin();

	// default color by Z
	set_colorby(&_cloud->z());

	// default alpha by intensity
	if (_cloud->intensity_present()) set_color_alpha(&_cloud->intensity());

}

/*
bool cloud_visualixer::MainLoop(){

  while(!glfwWindowShouldClose(window_ptr)){

		if (glfwGetKey(window_ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window_ptr, GL_TRUE);

    glfwSwapBuffers(window_ptr);
    glfwPollEvents();

    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    // Draw points
    glDrawElements(GL_POINTS, num_elements*num_per_element , GL_UNSIGNED_INT, NULL);
    //cout << "looping \r" << flush;
	}

	return 0;
}
*/


#ifdef _TEST_
// use cmake to compile

int main(int argc, char * argv[]){
	// declare vars

	// test the point cloud viewer
	cloud_visualixer mycvis;
	//mycvis->set_test_case();
	PointCloud cloud = PointCloud::read_LAS("../testfiles/ComplexSRSInfo.las");
	//PointCloud cloud = PointCloud::read_LAS("../testfiles/xyzrgb_manuscript.las");
	//PointCloud cloud = PointCloud::read_LAS("../testfiles/LAS12_Sample_withRGB_Quick_Terrain_Modeler.las");
	mycvis.add_cloud(cloud);
    //mycvis.set_color_ramp(CRamp::DIVERGENT_1);
    //mycvis.set_colorby(&cloud.z());
    //mycvis.set_color_alpha());
	mycvis.run();

	return 0;

}

#endif