#ifndef _VISUALIZER_H
#define _VISUALIZER_H

// includes from this package
#include "ColorRamp.hpp"
#include "DrawablePoint.hpp"
#include "DrawableBox.hpp"
#include "OwnedPoint.hpp"

// includes from c++ lib
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

// includes from c lib
#include <stdio.h>
#include <stdlib.h>

// incudes from GLEW
#include <GL/glew.h>

// includes from GLFW3
#include <GLFW/glfw3.h>

// includes from GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/fast_exponential.hpp>
#include <glm/gtx/rotate_vector.hpp>
//#include <glm/transform.hpp>

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_CENTER_X 0
#define DEFAULT_CENTER_Y 0

#define VX_PI 3.14159265358979323846264338327950288

// this contains definitions for the openGL visualizer widget
//
// the visualizer should be able to:
//  - visualize flow in 2d and 3d and move around in it
//  - visualize the geometry in 2d and 3d and move around in it
//  - visualize parametric or mesh CAD models and move around
//  - visualize and interact with point clouds


namespace LookingGlass{

//************** base visualizer class *********************
class Visualizer{
public:

	Visualizer();
	virtual ~Visualizer();

	// getters and setters
	virtual std::string get_window_name() {return m_window_name;};
	virtual void set_window_name(std::string w_name);

	void set_color_ramp(CRamp ramp_name);
	void set_color_interpolation(bool interp) {m_color_ramp.set_interpolation(interp);};


	// //template <class T> void set_colorby(const T * color_by);
	// template <class T>
	// void set_colorby(T const * color_by, bool recalcminmax=true){
	// 	if (color_by == nullptr){
	// 		//std::cout << "SHIT THE COLORBY INPUT IS NULL" << std::endl;
	// 		return;
	// 	}
	// 	if (_colorby == nullptr) _colorby = new double[num_vertices];
	// 	for (unsigned int i=0; i<num_vertices; i++) _colorby[i] = double(color_by[i]);

	// 	//std::cout << "converted all to double" << std::endl;

	// 	// find the min of the incoming values
	// 	if (recalcminmax){
	// 		//std::cout << "readjusted the colorby" << std::endl;
	// 		_colorby_max = _colorby[0];
	// 		for (auto i=1; i<num_vertices; i++){
	// 			if (_colorby[i] > _colorby_max) _colorby_max = _colorby[i];
	// 		}
	// 		_colorby_min = _colorby[0];
	// 		for (auto i=1; i<num_vertices; i++){
	// 			if (_colorby[i] < _colorby_min) _colorby_min = _colorby[i];
	// 		}

	// 		//std::cout << "found the min and max" << std::endl;
	// 	}

		
		
	// 	// subtract out the min value
	// 	// normalize the colorby array from 0.0 to 1.0
	// 	// with 0 input locked at 0.5
	// 	//for (auto i=0; i<num_vertices; i++) _colorby[i] = (_colorby[i] - _colorby_min)/(_colorby_max - _colorby_min);
	// 	float fsmax, fsmin, fsscale, xscale;
	// 	xscale = std::max(std::abs(_colorby_max), std::abs(_colorby_min));
	// 	for (auto i=0; i<num_vertices; i++) _colorby[i] = (_colorby[i])/(xscale)*0.49 + 0.51;
	// 	//std::cout << "subtracted out the min value and normalized" << std::endl;

	// 	return;
	// }


	// template <class T>
	// void set_color_alpha(T const * alpha, bool recalcminmax=true){
	// 	if (alpha == nullptr){
	// 		//std::cout << "SHIT THE ALPHA INPUT IS NULL" << std::endl;
	// 		return;
	// 	}
	// 	if (_color_alpha == nullptr) _color_alpha = new double[num_vertices];
	// 	for (unsigned int i=0; i<num_vertices; i++) _color_alpha[i] = double(alpha[i]);

	// 	if (recalcminmax){
	// 		//std::cout << "readjusted the alpha" << std::endl;
	// 		_alpha_max = _color_alpha[0]; _alpha_min = _color_alpha[0];
	// 		for (auto i=1; i<num_vertices; i++){
	// 			if (_color_alpha[i] > _alpha_max) _alpha_max = _color_alpha[i];
	// 			if (_color_alpha[i] < _alpha_min) _alpha_min = _color_alpha[i];
	// 		}


	// 		// normalize the values from 0.1 to 1.0
	// 		if (_alpha_max == _alpha_min){
	// 			for (auto i=0; i<num_vertices; i++) _color_alpha[i] = 1.0;
	// 		}
	// 		else {
	// 			for (auto i=0; i<num_vertices; i++) _color_alpha[i] = (_color_alpha[i] - _alpha_min)/(_alpha_max-_alpha_min)*0.3 + 0.7;
	// 		}
	// 	}

	// }

	//virtual void set_custom_colors(rgb * colors);
	//void set_lock_rotation(bool lock_mode);

	virtual void set_test_case();

	// running the Visualizer
	virtual void run();

protected:
	// context data
	GLFWwindow * 					m_window_ptr;
	std::string 					m_window_name;
	// GLFWwindow * window_ptr;
	// std::string window_name;

	// model related data
	std::vector<GLfloat>	m_vertices;			// holds the vertex data (XYZRGBA)
	std::vector<GLuint> 	m_point_elems;		// element with 1 point
	std::vector<GLuint>		m_edge_elems;		// element of 2 points
	std::vector<GLuint>		m_triangle_elems;	// element of 3 points
	// std::vector<GLuint>		m_quads;			// element of 4 points

	OwnedPoint 				m_centroid;
	DrawableBox				m_bounding_box;

	// GLfloat * vertices;
	// GLuint * elements;
	// float model_centroid[3]; // from [model_min, model_max]
	// float xmin, xmax, ymin, ymax, zmin, zmax;
	// unsigned int num_vertices, num_per_vertex, num_vertex_points;
	// unsigned int num_elements, num_per_element; // this should be deleted one VisualizerGeometricObject is fixed
	// unsigned int _num_point_elements, _num_per_point_element=1;
	// unsigned int _num_line_elements, _num_per_line_element=2;
	// unsigned int _num_tri_elements, _num_per_tri_element=3;
	// unsigned int _num_quad_elements, _num_per_quad_element=4;


	// colors ... so pretty
	std::vector<double>		m_colorby;
	std::vector<double>		m_color_alpha;
	double 					m_colorby_min, m_colorby_max;
	double					m_alpha_min, m_alpha_max;
  	// double * _colorby; // one per point
  	// double * _color_alpha;
  	// double _colorby_max, _colorby_min, _alpha_max, _alpha_min;


  	// virtual void draw_model();
  	bool 			m_display_triangles = true;
  	bool			m_display_edges = true;
  	bool			m_display_points = true;
  	void draw_points();
  	void draw_edges();
  	void draw_triangles();
	
	
	// color bar display
	ColorRamp 				m_color_ramp;
	bool 					m_display_colorbar;
	// std::vector<GLfloat> 	m_colorbar_vertices;
	// std::vector<GLuint> 	m_colorbar_elements;
	// std::vector<GLfloat> 	m_colorbar_colors;
	// GLuint 					m_vao_colorbar, m_vbo_colorbar_vertices, m_vbo_colorbar_colors, m_ebo_colorbar;
	// GLuint 					m_colorbar_vertex_shader, m_colorbar_fragment_shader, m_colorbar_shader_program;
	// GLint 					m_colorbar_pos_attrib, m_colorbar_color_attrib;
	// virtual const GLchar * ColorBar_v_glsl();
	// virtual const GLchar * ColorBar_f_glsl();
	// virtual void update_colorbar();
	// virtual void compile_colorbar();
	// virtual void draw_colorbar();
	// virtual void free_colorbar();
	

	// compass data and display

	// model shaders and buffer objects
	GLuint 					m_vbo, m_vao;
	GLuint					m_ebo_points, m_ebo_edges, m_ebo_triangles;
	GLuint 					m_vertexShader, m_fragmentShader, m_shaderProgram;
	GLint 					m_uniModel, m_uniView, m_uniProj;

	// visualizer user viewing data
	glm::mat4 				m_model, m_view, m_proj;
	glm::vec3 				m_eye_vec, m_focus_vec, m_up_vec;
	glm::vec3 				m_new_eye, m_new_focus;
	glm::vec3 				m_camera_side, m_camera_up;
	float 					m_rotdeg, m_zoom_level, m_zoom_scale, m_eyez_init;

	// status data
	bool 					m_left_mouse_engaged; 		// is the left mouse button clicked?
	bool 					m_middle_mouse_engaged; 		// is the middle mouse button clicked?
	bool 					m_right_mouse_engaged; 		// is the right mouse button clicked?
	double 					m_x_upon_click, m_y_upon_click; // x and y positions of mouse upon click
	bool 					m_visualizer_active; 		// is the window currently drawn?
	bool 					m_rotation_lock; 			// lock mouse rotations?
	bool 					m_pan_lock; 				// lock mouse panning
	bool 					m_zoom_lock; 				// lock mouse zooming


	// base callbacks to interface with GLFW
	virtual void onMouseClick(int button, int action, int modifiers);
	virtual void onMouseWheel(double xoffset, double yoffset);
	virtual void onKeyboard(int key, int scancode, int action, int modifiers);
	virtual void onCursorPosition(double xpos, double ypos);

	/*
	// derived callbacks defined by me
	virtual void onMouseLeftDrag(double xpos, double ypos);
	virtual void onMouseRightDrag(double xpos, double ypos);
	virtual void onMouseMiddleDrag(double xpos, double ypos);
	virtual void onKeyDown(unsigned char key, int x, int y);
	virtual void onKeyUp(unsigned char key, int x, int y);
	virtual void onKeySpace();
	virtual void onKeyC();
	virtual void onKeyF();
	virtual void onReshape(int new_width, int new_height);
	virtual void SetFullscreen(bool bFullscreen);
	virtual void cycleColorRamp();
	*/
	virtual void recalcCamera();



	// functions related to the context creation and main loop rendering
	virtual const GLchar * VertexShaderSource();
	virtual const GLchar * FragmentShaderSource();
	virtual void onInit();
	virtual void onPrepareData();
	virtual void onColors();
	virtual void onAlpha();
	virtual void onRender();
	virtual void onShaders();
	virtual void PreLoopTasks();
	virtual	bool MainLoop();
	virtual void LoopTasks();
	virtual void onExit();
	virtual void onRefresh();	// everything is already prepared, just refresh the screen


private:

	// base callbacks for GLFW functions
	static void sMouseClick(GLFWwindow * window, int button, int action, int modifiers);
	static void sMouseWheel(GLFWwindow * window, double xoffset, double yoffset);
	static void sKeyboard(GLFWwindow * window, int key, int scancode, int action, int modifiers);
	static void sCursorPosition(GLFWwindow * window, double xpos, double ypos);

};
}

#endif
