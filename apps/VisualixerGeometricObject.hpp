#ifndef _VISUALIXERCLOUD_H
#define _VISUALIXERCLOUD_H

#include "GeometricObject.hpp"
#include "Visualixer.hpp"


#include <iostream>
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>

//**************** Mesh Model Visualixer ********************
// viewing an mesh model geometry (like an STL model)
class mesh_model_visualixer : public visualixer{
public:
	mesh_model_visualixer();
	~mesh_model_visualixer();

	void add_model(const mesh_model & model);
	void set_test_case();

protected:

	GLfloat * normals;
	GLuint normalbuffer, num_normals;

	const mesh_model * _model;

	//const GLchar * VertexShaderSource();
	//const GLchar * FragmentShaderSource();

	void onPrepareData();
	void onRender();
	void onShaders();
	//bool MainLoop();
	void onExit();

};

//***************** Parametric Model Visualixer ************************
// viewing an parametric geometry (like a CAD model)
class parametric_model_visualixer : public visualixer{

	parametric_model_visualixer();
	~parametric_model_visualixer();

	//void add_model(parametric_model);
	void set_test_case();

protected:
	void onRender();
	bool MainLoop();
	void onExit();

};

#endif