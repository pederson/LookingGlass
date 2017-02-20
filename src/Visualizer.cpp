#include "../include/Visualizer.hpp"
#include "../include/ColorRamp.hpp"

// using namespace std;


// this contains definitions for the openGL visualizer widget
//
// the visualizer should be able to:
//  - visualize flow in 2d and 3d and move around in it
//  - visualize the geometry in 2d and 3d and move around in it
//  - visualize and interact with point clouds

/* There is some really good OpenGL 4 info here:
	http://www.swiftless.com/opengl4tuts.html

	https://en.wikibooks.org/wiki/OpenGL_Programming

	https://www.opengl.org/sdk/docs/man3/xhtml/

	http://antongerdelan.net/opengl/shaders.html
*/


namespace LookingGlass{

std::vector<Visualizer *> _vInstances;

//*************************************** Base Visualizer Class *******************************************
Visualizer::Visualizer(){

	// really should keep track of instances of visualizer
	_vInstances.push_back(this);

	m_visualizer_active = false;
	m_window_name = "Visualizer";
	m_rotation_lock = false;

	m_centroid = OwnedPoint(0,0,0);
	m_bounding_box.lo = std::shared_ptr<DrawablePoint>(new OwnedPoint(0,0,0));
	m_bounding_box.hi = std::shared_ptr<DrawablePoint>(new OwnedPoint(0,0,0));

	m_display_colorbar = false;
}

Visualizer::~Visualizer(){
	for (unsigned int i=0; i<_vInstances.size(); i++){
		if (_vInstances.at(i) == this){
			_vInstances.erase(_vInstances.begin() + i);
			return;
		}
	}
}

void Visualizer::set_window_name(std::string w_name){
	m_window_name = w_name;
	return;
}

void Visualizer::set_color_ramp(CRamp ramp_name){
  m_color_ramp.set_ramp(ramp_name);
}





void Visualizer::run(){
	onInit();
	std::cout << "INIT DONE" << std::endl;
	onPrepareData();
	std::cout << "PREPARE DONE" << std::endl;
	onColors();
	std::cout << "COLORS DONE" << std::endl;
	onAlpha();
	std::cout << "ALPHA DONE" << std::endl;
	onRender();
	std::cout << "RENDER DONE" << std::endl;
	onShaders();
	std::cout << "SHADERS DONE" << std::endl;
	PreLoopTasks();
	std::cout << "PRELOOP DONE" << std::endl;
	MainLoop();
	std::cout << "LOOP DONE" << std::endl;
	onExit();
	std::cout << "EXIT DONE" << std::endl;
	return;
}

void Visualizer::set_test_case(){

	// these are the vertices (4 of them) for 2 triangles
	m_vertices.resize(8*7);
	m_vertices[0] = 10.0-0.5; m_vertices[1] = 10.0+0.5; m_vertices[2] = 0.0; 			m_vertices[3] = 1.0; m_vertices[4] = 0.0; m_vertices[5] = 0.0; m_vertices[6] = 0.1;
	m_vertices[7] = 10.0+0.5; m_vertices[8] = 10.0+0.5; m_vertices[9] = 0.0; 			m_vertices[10] = 0.0; m_vertices[11] = 1.0; m_vertices[12] = 0.0; m_vertices[13] = 0.1;
	m_vertices[14] = 10.0+0.5; m_vertices[15] = 10.0-0.5; m_vertices[16] = 0.0; 		m_vertices[17] = 0.0; m_vertices[18] = 0.0; m_vertices[19] = 1.0; m_vertices[20] = 0.1;
	m_vertices[21] = 10.0-0.5; m_vertices[22] = 10.0-0.5; m_vertices[23] = 0.0; 		m_vertices[24] = 1.0; m_vertices[25] = 1.0; m_vertices[26] = 1.0; m_vertices[27] = 0.1;

	m_vertices[28+0] = 9.5-0.5; m_vertices[28+1] = 9.5+0.5; m_vertices[28+2] = -0.5; 			m_vertices[28+3] = 1.0; m_vertices[28+4] = 0.0; m_vertices[28+5] = 0.0; m_vertices[28+6] = 1.0;
	m_vertices[28+7] = 9.5+0.5; m_vertices[28+8] = 9.5+0.5; m_vertices[28+9] = -0.5; 			m_vertices[28+10] = 0.0; m_vertices[28+11] = 1.0; m_vertices[28+12] = 0.0; m_vertices[28+13] = 1.0;
	m_vertices[28+14] = 9.5+0.5; m_vertices[28+15] = 9.5-0.5; m_vertices[28+16] = -0.5; 		m_vertices[28+17] = 0.0; m_vertices[28+18] = 0.0; m_vertices[28+19] = 1.0; m_vertices[28+20] = 1.0;
	m_vertices[28+21] = 9.5-0.5; m_vertices[28+22] = 9.5-0.5; m_vertices[28+23] = -0.5; 		m_vertices[28+24] = 1.0; m_vertices[28+25] = 1.0; m_vertices[28+26] = 1.0; m_vertices[28+27] = 1.0;

	// set point elements corresponding to the vertices
	m_point_elems.resize(8*1);
	m_point_elems[0] = 0;
	m_point_elems[1] = 1;
	m_point_elems[2] = 2;
	m_point_elems[3] = 3;

	m_point_elems[4] = 4;
	m_point_elems[5] = 5;
	m_point_elems[6] = 6;
	m_point_elems[7] = 7;

	// set edges 
	m_edge_elems.resize(8*2);
	m_edge_elems[0] = 0; m_edge_elems[1] = 1;
	m_edge_elems[2] = 1; m_edge_elems[3] = 2;
	m_edge_elems[4] = 2; m_edge_elems[5] = 3;
	m_edge_elems[6] = 3; m_edge_elems[7] = 0;

	m_edge_elems[8+0] = 4 + 0; m_edge_elems[8+1] = 4 + 1;
	m_edge_elems[8+2] = 4 + 1; m_edge_elems[8+3] = 4 + 2;
	m_edge_elems[8+4] = 4 + 2; m_edge_elems[8+5] = 4 + 3;
	m_edge_elems[8+6] = 4 + 3; m_edge_elems[8+7] = 4 + 0;

	// set the elements of the 2 triangles
	m_triangle_elems.resize(4*3);
	m_triangle_elems[0] = 0; m_triangle_elems[1] = 1; m_triangle_elems[2] = 2;
	m_triangle_elems[3] = 2; m_triangle_elems[4] = 3; m_triangle_elems[5] = 0;

	m_triangle_elems[6+0] = 4+0; m_triangle_elems[6+1] = 4+1; m_triangle_elems[6+2] = 4+2;
	m_triangle_elems[6+3] = 4+2; m_triangle_elems[6+4] = 4+3; m_triangle_elems[6+5] = 4+0;


	m_centroid = OwnedPoint(10.0, 10.0, -0.5);
	m_bounding_box.lo = std::shared_ptr<DrawablePoint>(new OwnedPoint(9.0, 9.0, -0.5));
	m_bounding_box.hi = std::shared_ptr<DrawablePoint>(new OwnedPoint(10.5, 10.5, 0.0));

	return;
}





void Visualizer::onMouseClick(int button, int action, int modifiers){

}

void Visualizer::onMouseWheel(double xoffset, double yoffset){

}

void Visualizer::onKeyboard(int key, int scancode, int action, int modifiers){

}

void Visualizer::onCursorPosition(double xpos, double ypos){
}

/*
void Visualizer::onMouseClick(int button, int action, int modifiers){
	bool state;
	if (action == GLFW_PRESS) state = true;
	else if(action == GLFW_RELEASE) state = false;
	else std::cout << "unknown mouse state" << std::endl;


	switch (button){
		case GLFW_MOUSE_BUTTON_LEFT :
			left_mouse_engaged = state;
			if (state) {
				glfwGetCursorPos(m_window_ptr, &x_upon_click, &y_upon_click);
				new_eye = eye_vec;
			}
			else {
				eye_vec = new_eye;
				recalcCamera();
			}

			break;
		case GLFW_MOUSE_BUTTON_MIDDLE :
			middle_mouse_engaged = state;
			//std::cout << "middle mouse click " << (state? "on":"off")  << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT :
			right_mouse_engaged = state;
			if (state) {
				glfwGetCursorPos(m_window_ptr, &x_upon_click, &y_upon_click);
				new_eye = eye_vec;
				new_focus = focus_vec;
			}
			else {
				eye_vec = new_eye;
				focus_vec = new_focus;
				recalcCamera();
			}
			break;
		otherwise :
			std::cout << "unknown mouse button" << std::endl;
	}
	return;
}

void Visualizer::onMouseWheel(double xoffset, double yoffset){
	//std::cout << "MOUSE WHEEL: xoffset: " << xoffset << " yoffset: " << yoffset << std::endl;
	float zoom_scale_new;
	if (yoffset < 0){
		zoom_level -= 0.05;

	}
	else if (yoffset > 0){
		zoom_level += 0.05;

	}
	//std::cout << "zoom level: " << zoom_level << std::endl;
	if (zoom_level > 5.0) zoom_level = 5.0; // limit the zoom in
	if (zoom_level < 0.5) zoom_level = 0.5; // limit the zoom out
	zoom_scale_new = 1/(zoom_level);
	eye_vec = focus_vec + (eye_vec-focus_vec)*(zoom_scale_new-zoom_scale + 1.0f);
	zoom_scale = zoom_scale_new;
	view = glm::lookAt(
        eye_vec,
        focus_vec,
        up_vec
    );
}

void Visualizer::onKeyboard(int key, int scancode, int action, int modifiers){
	//std::cout << "KEYBOARD PRESS" << std::endl;
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){ // resets the view
		onKeySpace();
	}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS){
		onKeyC();
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS){
		onKeyF();
	}
}

void Visualizer::onCursorPosition(double xpos, double ypos){
	if (left_mouse_engaged) onMouseLeftDrag(xpos, ypos);
	else if(right_mouse_engaged) onMouseRightDrag(xpos, ypos);
	else if(middle_mouse_engaged) onMouseMiddleDrag(xpos, ypos);
}

//DYLAN_TODO: do this using quaternions instead
void Visualizer::onMouseLeftDrag(double xpos, double ypos){
	double new_x_pos, new_y_pos;
	int width, height;
	glm::vec3 rot_vec, in_world_ip;

	glfwGetWindowSize(m_window_ptr, &width, &height);
	glfwGetCursorPos(m_window_ptr, &new_x_pos, &new_y_pos);

	rotdeg = ((new_x_pos-x_upon_click)*(new_x_pos-x_upon_click)
					 + (new_y_pos-y_upon_click)*(new_y_pos-y_upon_click))/(width*width + height*height)*VX_PI/2;


	in_world_ip = float(new_x_pos-x_upon_click)*-camera_side + float(new_y_pos-y_upon_click)*camera_up;

	// comes from the cross product
	rot_vec = glm::cross(in_world_ip, focus_vec - eye_vec);

	new_eye = focus_vec +  glm::rotate((eye_vec-focus_vec), rotdeg, rot_vec);
	view = glm::lookAt(new_eye, focus_vec, up_vec);
}

void Visualizer::onMouseRightDrag(double xpos, double ypos){
	double new_x_pos, new_y_pos;
	int width, height;
	glm::vec3 in_plane, in_world_ip;
	glm::vec4 world_in_plane;
	GLfloat pan_scale, in_plane_norm;

	glfwGetWindowSize(m_window_ptr, &width, &height);
	glfwGetCursorPos(m_window_ptr, &new_x_pos, &new_y_pos);


	pan_scale = ((new_x_pos-x_upon_click)*(new_x_pos-x_upon_click) + (new_y_pos-y_upon_click)*(new_y_pos-y_upon_click))/(width*width + height*height)*((xmax-xmin)*(xmax-xmin)+(ymax-ymin)*(ymax-ymin));
	//std::cout << "pan scale: " << pan_scale << std::endl;

	in_world_ip = float(new_x_pos-x_upon_click)*-camera_side + float(new_y_pos-y_upon_click)*camera_up;
	in_world_ip = glm::normalize(in_world_ip);

	//std::cout << "about to translate " << std::endl;
	new_eye = eye_vec + pan_scale*in_world_ip;
	new_focus = focus_vec + pan_scale*in_world_ip;
	view = glm::lookAt(new_eye, new_focus, up_vec);
	//std::cout << "finished translating" << std::endl;
}

void Visualizer::onMouseMiddleDrag(double xpos, double ypos){
	//std::cout << "dragging middle mouse button" << std::endl;
}

void Visualizer::onKeyDown(unsigned char key, int x, int y){
	std::cout << "Pressed Key with unsigned char : " << key << std::endl;
	return;
}

void Visualizer::onKeyUp(unsigned char key, int x, int y){
	std::cout << "Released Key with unsigned char: " << key << std::endl;
	return;
}

void Visualizer::onReshape(int new_width, int new_height){
	std::cout << "reshaping" << std::endl;
}

void Visualizer::SetFullscreen(bool bFullscreen){

}
*/

void Visualizer::recalcCamera(){
	m_camera_side = glm::normalize(glm::cross(m_focus_vec - m_eye_vec, m_up_vec - m_eye_vec + m_focus_vec));
	m_camera_up = glm::normalize(glm::cross(m_camera_side, m_focus_vec - m_eye_vec));

	
	// std::cout << "m_focus_vec: " << m_focus_vec.x << ", " << m_focus_vec.y << ", " << m_focus_vec.z << std::endl;
	// std::cout << "m_eye_vec: " << m_eye_vec.x << ", " << m_eye_vec.y << ", " << m_eye_vec.z << std::endl;
	// std::cout << "m_up_vec: " << m_up_vec.x << ", " << m_up_vec.y << ", " << m_up_vec.z << std::endl;
	// std::cout << "m_camera_side: " << m_camera_side.x << ", " << m_camera_side.y << ", " << camera_side.z << std::endl;
	// std::cout << "m_camera_up: " << m_camera_up.x << ", " << m_camera_up.y << ", " << m_camera_up.z << std::endl;
	
}

/*
void Visualizer::onKeySpace(){
	eye_vec.x = model_centroid[0];
	eye_vec.y = model_centroid[1];
	eye_vec.z = eyez_init;
	focus_vec.x = model_centroid[0];
	focus_vec.y = model_centroid[1];
	focus_vec.z = model_centroid[2];
	up_vec.x = 0.0f;
	up_vec.y = 1.0f;
	up_vec.z = 0.0f;

	recalcCamera();

	//std::cout << "zmax is " << zmax << std::endl;
	view = glm::lookAt(
        eye_vec,
        focus_vec,
        up_vec
    	);
	return;
}

void Visualizer::onKeyC(){
	cycleColorRamp();
	return;
}

void Visualizer::onKeyF(){

	return;
}
*/





/*
void Visualizer::cycleColorRamp(){
	if (m_colorby==nullptr) return;
	color_ramp.cycle_ramp();
	//rgb half;
	//half = color_ramp.get_ramp_color(0.5);
	//std::cout << "new color ramp at 0.5: (" << half.R << ", " << half.G << ", " << half.B << ")" << std::endl;
	onColors();
	onRefresh();
	return;
}


void Visualizer::update_colorbar(){
	if (!m_display_colorbar) return;

	unsigned int rsz = color_ramp.ramp_size();

	GLfloat cramp_min_loc_y = -0.8;
	GLfloat cramp_max_loc_y = 0.8;
	GLfloat cramp_min_loc_x = 0.7;
	GLfloat cramp_max_loc_x = 0.8;

	//std::cout << "num vertices old: " << m_colorbar_vertices.size() << std::endl;
	
	// this changes the location in memory
	m_colorbar_vertices.resize(4*2*rsz);
	m_colorbar_colors.resize(4*3*rsz);
	m_colorbar_elements.resize(2*3*rsz);


	GLfloat yspacing = (cramp_max_loc_y - cramp_min_loc_y)/GLfloat(color_ramp.ramp_size());


	for (auto i=0; i<color_ramp.ramp_size(); i++){
		m_colorbar_vertices[4*2*i] = cramp_min_loc_x;
		m_colorbar_vertices[4*2*i + 1] = cramp_min_loc_y + i*yspacing;
		m_colorbar_vertices[4*2*i + 2] = cramp_max_loc_x;
		m_colorbar_vertices[4*2*i + 3] = cramp_min_loc_y + i*yspacing;
		m_colorbar_vertices[4*2*i + 4] = cramp_max_loc_x;
		m_colorbar_vertices[4*2*i + 5] = cramp_min_loc_y + (i+1)*yspacing;
		m_colorbar_vertices[4*2*i + 6] = cramp_min_loc_x;
		m_colorbar_vertices[4*2*i + 7] = cramp_min_loc_y + (i+1)*yspacing;
	

		// elements
		m_colorbar_elements[2*3*i] = 4*i;
		m_colorbar_elements[2*3*i + 1] = 4*i+1;
		m_colorbar_elements[2*3*i + 2] = 4*i+2;
		m_colorbar_elements[2*3*i + 3] = 4*i;
		m_colorbar_elements[2*3*i + 4] = 4*i+2;
		m_colorbar_elements[2*3*i + 5] = 4*i+3;

		// colors
		rgb clr = color_ramp.get_ramp_color(GLfloat(i)/GLfloat(color_ramp.ramp_size()));
		m_colorbar_colors[4*3*i] = clr.R;
		m_colorbar_colors[4*3*i + 1] = clr.G;
		m_colorbar_colors[4*3*i + 2] = clr.B;
		m_colorbar_colors[4*3*i + 3] = clr.R;
		m_colorbar_colors[4*3*i + 4] = clr.G;
		m_colorbar_colors[4*3*i + 5] = clr.B;
		m_colorbar_colors[4*3*i + 6] = clr.R;
		m_colorbar_colors[4*3*i + 7] = clr.G;
		m_colorbar_colors[4*3*i + 8] = clr.B;
		m_colorbar_colors[4*3*i + 9] = clr.R;
		m_colorbar_colors[4*3*i + 10] = clr.G;
		m_colorbar_colors[4*3*i + 11] = clr.B;
		m_colorbar_colors[4*3*i + 12] = clr.R;
		m_colorbar_colors[4*3*i + 13] = clr.G;
		m_colorbar_colors[4*3*i + 14] = clr.B;

		// std::cout << "vertex: " << m_colorbar_vertices[4*2*i] << " , " << m_colorbar_vertices[4*2*i + 1] << std::endl;
		// std::cout << "vertex: " << m_colorbar_vertices[4*2*i + 2] << " , " << m_colorbar_vertices[4*2*i + 3] << std::endl;
		// std::cout << "vertex: " << m_colorbar_vertices[4*2*i + 4] << " , " << m_colorbar_vertices[4*2*i + 5] << std::endl;
		// std::cout << "vertex: " << m_colorbar_vertices[4*2*i + 6] << " , " << m_colorbar_vertices[4*2*i + 7] << std::endl;
		// std::cout << "color: " << clr.R << " , " << clr.G << " , " << clr.B << std::endl;
		// std::cout << "elements: " << m_colorbar_elements[2*3*i] << ", " << m_colorbar_elements[2*3*i+1] << ", " << m_colorbar_elements[2*3*i+2] << std::endl;
	}

	return;
}

void Visualizer::compile_colorbar(){
	if (!m_display_colorbar) return;

	glGenVertexArrays(1, &m_vao_colorbar);
	glBindVertexArray(m_vao_colorbar);

	// generate 2 VBOs
	GLuint vbos[2];
	glGenBuffers(2, &vbos[0]);
	m_vbo_colorbar_vertices = vbos[0];
	m_vbo_colorbar_colors = vbos[1];

	//std::cout << "about to bind" << std::endl;

	// VBO for vertices
	//glGenBuffers (1, &m_vbo_colorbar_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colorbar_vertices);
	glBufferData(GL_ARRAY_BUFFER, m_colorbar_vertices.size()* sizeof(GLfloat), m_colorbar_vertices.data(), GL_STATIC_DRAW);

	// VBO for colors
	//glGenBuffers (1, &m_vbo_colorbar_colors);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colorbar_colors);
	glBufferData(GL_ARRAY_BUFFER, m_colorbar_colors.size()* sizeof(GLfloat), m_colorbar_colors.data(), GL_STATIC_DRAW);

	// Element array
	glGenBuffers(1, &m_ebo_colorbar);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_colorbar);	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_colorbar_elements.size() * sizeof(GLuint), m_colorbar_elements.data(), GL_STATIC_DRAW);

	// // enable point size specification
	// glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	// // enable alpha channel
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// // enable depth 
	//glEnable(GL_DEPTH_TEST);
	//std::cout << "about to compile" << std::endl;

	// compile the vertex shader
	m_colorbar_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar * vssource = this->ColorBar_v_glsl();
	glShaderSource(m_colorbar_vertex_shader, 1, &(vssource), NULL);
	glCompileShader(m_colorbar_vertex_shader);

	GLint status;
	GLint logLength;
	glGetShaderiv(m_colorbar_vertex_shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE){
		std::cout << "vertex shader failed to compile" << std::endl;
		glGetShaderiv(m_colorbar_vertex_shader, GL_INFO_LOG_LENGTH , &logLength);
		if (logLength > 1) {
		    GLchar* compiler_log = (GLchar*)malloc(logLength);
		    glGetShaderInfoLog(m_colorbar_vertex_shader, logLength, 0, compiler_log);
		    printf("%s\n", compiler_log);
		    free (compiler_log);
		}
	}


	// compile the fragment shader
	m_colorbar_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar * fssource = this->ColorBar_f_glsl();
	glShaderSource(m_colorbar_fragment_shader, 1, &(fssource), NULL);
	glCompileShader(m_colorbar_fragment_shader);

	glGetShaderiv(m_colorbar_fragment_shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE){
		std::cout << "fragment shader failed to compile" << std::endl;
		glGetShaderiv(m_colorbar_fragment_shader, GL_INFO_LOG_LENGTH , &logLength);
		if (logLength > 1) {
		    GLchar* compiler_log = (GLchar*)malloc(logLength);
		    glGetShaderInfoLog(m_colorbar_fragment_shader, logLength, 0, compiler_log);
		    printf("%s\n", compiler_log);
		    free (compiler_log);
		}
	}

	// link the vertex and fragment shader into a shader program
	m_colorbar_shader_program = glCreateProgram();
	glAttachShader(m_colorbar_shader_program, m_colorbar_vertex_shader);
	glAttachShader(m_colorbar_shader_program, m_colorbar_fragment_shader);
	glBindFragDataLocation(m_colorbar_shader_program, 0, "outColor");
	glLinkProgram(m_colorbar_shader_program);
	glUseProgram(m_colorbar_shader_program);

	// Specify the layout of the vertex data passed into the shader program
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colorbar_vertices);
	m_colorbar_pos_attrib = glGetAttribLocation(m_colorbar_shader_program, "position");
	glEnableVertexAttribArray(m_colorbar_pos_attrib);
	glVertexAttribPointer(m_colorbar_pos_attrib, 
						  2, 
						  GL_FLOAT, 
						  GL_FALSE, 
						  0, 
						  0
						  );

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colorbar_colors);
	m_colorbar_color_attrib = glGetAttribLocation(m_colorbar_shader_program, "color");
	glEnableVertexAttribArray(m_colorbar_color_attrib);
	glVertexAttribPointer(m_colorbar_color_attrib, 	// attribute
						  3, 			// number of values 
						  GL_FLOAT, 	// data type
						  GL_FALSE, 	// take the value as-is
						  0, //3 * sizeof(GLfloat), 	// stride
						  0	// offset
						  );


}

void Visualizer::draw_colorbar(){
	glUseProgram(m_colorbar_shader_program);
	glBindVertexArray(m_vao_colorbar);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colorbar_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colorbar_colors);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_colorbar);

	glDrawElements(GL_TRIANGLES, m_colorbar_elements.size() , GL_UNSIGNED_INT, (void *)0);

	glBindVertexArray(0);
	//std::cout << "drawing colorbar" << std::endl;
}

void Visualizer::free_colorbar(){
	if (!m_display_colorbar) return;

	glDeleteProgram(m_colorbar_shader_program);
	glDeleteShader(m_colorbar_fragment_shader);
	glDeleteShader(m_colorbar_vertex_shader);
	
	glDeleteBuffers(1, &m_ebo_colorbar);
	glDeleteBuffers(1, &m_vbo_colorbar_vertices);
	glDeleteBuffers(1, &m_vbo_colorbar_colors);

	glDeleteVertexArrays(1, &m_vao_colorbar);
}

const GLchar * Visualizer::ColorBar_v_glsl(){
	const GLchar* vertexSource =
	    "#version 140\n"
	    "in vec2 position;"
	    "in vec3 color;"
	    "out vec3 Color;"
	    "void main() {"
	    "   Color = color;"
      	"   gl_PointSize = 2.0;"
	    "   gl_Position = vec4(position, 0.0,  1.0);"
	    "}";
	return vertexSource;
}

const GLchar * Visualizer::ColorBar_f_glsl(){
	const GLchar* fragmentSource =
	    "#version 140\n"
	    "in vec3 Color;"
	    "out vec4 outColor;"
	    "void main() {"
	    "   outColor = vec4(Color, 1.0);"
	    "}";
  return fragmentSource;
}
*/


const GLchar * Visualizer::VertexShaderSource(){
	// vertex shader sources
	const GLchar* vertexSource =
	    "#version 140\n"
	    "in vec3 position;"
	    "in vec4 color;"
	    "out vec4 Color;"
	    "uniform mat4 model;"
	    "uniform mat4 view;"
    	"uniform mat4 proj;"
	    "void main() {"
	    "   Color = color;"
      	"   gl_PointSize = 4.0;"
	    "   gl_Position = proj*view*model*vec4(position, 1.0);"
	    "}";
	return vertexSource;

}

const GLchar * Visualizer::FragmentShaderSource(){
  // fragment shader source
	const GLchar* fragmentSource =
	    "#version 140\n"
	    "in vec4 Color;"
	    "out vec4 outColor;"
	    "void main() {"
	    "   outColor = Color;"//vec4(Color, 1.0);"
	    "}";
  return fragmentSource;
}

void Visualizer::onInit(){

	if (!glfwInit()){
		std::cerr << "glfw Init failure " << std::endl;
		throw -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // This must be compatible with the installed version of OpenGL
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); THIS CAUSES A FAILURE
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);


	m_window_ptr = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, m_window_name.c_str(), NULL, NULL);
	// m_window_ptr= std::shared_ptr<GLFWwindow>(glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, m_window_name.c_str(), NULL, NULL)); // Windowed
	//GLFWwindow* window = glfwCreateWindow(400, 300, "OpenGL", glfwGetPrimaryMonitor(), NULL); // Fullscreen
	if ( !m_window_ptr ) {
		std::cerr << "failed to create window" << std::endl;
        glfwTerminate();
        exit ( EXIT_FAILURE );
  }
	glfwMakeContextCurrent(m_window_ptr);

	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
    if ( GLEW_OK != glew_status ) {
        exit ( EXIT_FAILURE );
  }

  // set callbacks
  glfwSetMouseButtonCallback(m_window_ptr, sMouseClick);
  glfwSetScrollCallback(m_window_ptr, sMouseWheel);
  glfwSetKeyCallback(m_window_ptr, sKeyboard);
  glfwSetCursorPosCallback(m_window_ptr, sCursorPosition);
}


void Visualizer::onPrepareData(){


}


void Visualizer::onColors(){
	if (m_colorby.size()==0) return;
	if (m_colorby_max - m_colorby_min == 0.0) return;
	// modify the vertex array to incorporate user-defined colors


	rgb ptcolor;
	for (auto i=0; i<m_vertices.size()/7; i++){
		//ptcolor = color_ramp.get_ramp_color(float((m_colorby[i])/(m_colorby_max - m_colorby_min)));
		ptcolor = m_color_ramp.get_ramp_color(m_colorby[i]);
		m_vertices[(i+1)*7 - 4] = ptcolor.R;
		m_vertices[(i+1)*7 - 3] = ptcolor.G;
		m_vertices[(i+1)*7 - 2] = ptcolor.B;
	}

	return;
}


void Visualizer::onAlpha(){
	if (m_color_alpha.size() == 0) return;

	
	for (auto i=0; i<m_vertices.size()/7; i++){
		m_vertices[(i+1)*7 - 1] = m_color_alpha[i];
	}

}


void Visualizer::onRender(){

	// Create Vertex Array Object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// create VBO and copy data to it
	glGenBuffers (1, &m_vbo);

	// visualizer specific data definitions
	// this one happens to be XYZRGB
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), &(m_vertices.front()), GL_STATIC_DRAW);

	// Create an element array if necessary

	// point elements
	glGenBuffers(1, &m_ebo_points);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_points);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_point_elems.size() * sizeof(GLuint), &(m_point_elems.front()), GL_STATIC_DRAW);

	// edge elements
	glGenBuffers(1, &m_ebo_edges);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_edges);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_edge_elems.size() * sizeof(GLuint), &(m_edge_elems.front()), GL_STATIC_DRAW);

	// triangle elements
	glGenBuffers(1, &m_ebo_triangles);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_triangles);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangle_elems.size() * sizeof(GLuint), &(m_triangle_elems.front()), GL_STATIC_DRAW);

	// enable point size specification
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	// enable alpha channel
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// enable depth 
	glEnable(GL_DEPTH_TEST);

  return;
}

void Visualizer::onShaders(){

	// create and compile the vertex shader
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar * vssource = this->VertexShaderSource();
	glShaderSource(m_vertexShader, 1, &(vssource), NULL);
	glCompileShader(m_vertexShader);

	GLint status;
	GLint logLength;
	glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE){
		std::cerr << "vertex shader failed to compile" << std::endl;
		glGetShaderiv(m_vertexShader, GL_INFO_LOG_LENGTH , &logLength);
		if (logLength > 1) {
		    GLchar* compiler_log = (GLchar*)malloc(logLength);
		    glGetShaderInfoLog(m_vertexShader, logLength, 0, compiler_log);
		    printf("%s\n", compiler_log);
		    free (compiler_log);
		}
	}


	// create and compile the fragment shader
	m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar * fssource = this->FragmentShaderSource();
	glShaderSource(m_fragmentShader, 1, &(fssource), NULL);
	glCompileShader(m_fragmentShader);

	glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE){
		std::cerr << "fragment shader failed to compile" << std::endl;
		glGetShaderiv(m_fragmentShader, GL_INFO_LOG_LENGTH , &logLength);
		if (logLength > 1) {
		    GLchar* compiler_log = (GLchar*)malloc(logLength);
		    glGetShaderInfoLog(m_fragmentShader, logLength, 0, compiler_log);
		    printf("%s\n", compiler_log);
		    free (compiler_log);
		}
	}



	// link the vertex and fragment shader into a shader program
	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, m_vertexShader);
	glAttachShader(m_shaderProgram, m_fragmentShader);
	glBindFragDataLocation(m_shaderProgram, 0, "outColor");
	glLinkProgram(m_shaderProgram);
	glUseProgram(m_shaderProgram);

	// std::cout << "LINKED TO SHADER PROGRAM " << std::endl;

	// Specify the layout of the vertex data passed into the shader program
	//std::cout << "num_vertices: " << num_vertices << " num_vertex_points: " << num_vertex_points << " num_per_vertex: " << num_per_vertex << std::endl;
	GLint posAttrib = glGetAttribLocation(m_shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 			// attribute
						  3, 					// number of values
						  GL_FLOAT, 			// data type
						  GL_FALSE, 			// take the value as-is
						  7 * sizeof(GLfloat), 	// stride
						  0);					// offset

	GLint colAttrib = glGetAttribLocation(m_shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 	// attribute
						  4, 			// number of values 
						  GL_FLOAT, 	// data type
						  GL_FALSE, 	// take the value as-is
						  7 * sizeof(GLfloat), 	// stride
						  (void*)(3 * sizeof(GLfloat))	// offset
						  );


	// std::cout << "SPECIFIED LAYOUT" << std::endl;

	// specify the uniforms that are passed into the shader program
	m_rotdeg = 0;
	m_model = glm::rotate(m_model, m_rotdeg, glm::vec3(0.0f, 0.0f, 1.0f)); // angle in radians to suppress some output
	m_uniModel = glGetUniformLocation(m_shaderProgram, "model");
	glUniformMatrix4fv(m_uniModel, 1, GL_FALSE, glm::value_ptr(m_model));

	// std::cout << "SPECIFIED UNIFORMS" << std::endl;

	// Set up view matrix
	m_zoom_level = 1.0f;
	m_zoom_scale = 1.0f;
	m_up_vec = glm::vec3(0.0f, 1.0f, 0.0f);
	// calculate the eye z position so that it can view the whole scene
	if (m_bounding_box.hi->x() - m_bounding_box.lo->x() > m_bounding_box.hi->y() - m_bounding_box.lo->y()){
		m_eyez_init = 1.0*(m_bounding_box.hi->x()-m_bounding_box.lo->x()) + m_bounding_box.hi->z();
	}
	else {
		m_eyez_init = 1.0*(m_bounding_box.hi->y()-m_bounding_box.lo->y()) + m_bounding_box.hi->z();
	}


	// std:: cout << "CALCULATED EYE_Z" << std::endl;

	// if (xmax - xmin > ymax - ymin) eyez_init = (xmax-xmin) + zmax;
	// else eyez_init = (ymax - ymin) + zmax;
	//std::cout << "centroid is: " << model_centroid[0] << ", " << model_centroid[1] << ", " << model_centroid[2] << std::endl;
	m_focus_vec = glm::vec3(m_centroid.x(), m_centroid.y(), m_centroid.z());
	m_eye_vec = glm::vec3(m_centroid.x(), m_centroid.y(), m_eyez_init);
	// m_focus_vec = glm::vec3(model_centroid[0], model_centroid[1], model_centroid[2]);
	// m_eye_vec = glm::vec3(model_centroid[0], model_centroid[1], eyez_init);
	m_view = glm::lookAt(
				m_eye_vec, // camera position
				m_focus_vec, // the position to be looking at
				m_up_vec  // the up vector
				);
	recalcCamera();

	// std::cout << "RECALCULATED CAMERA" << std::endl;

	m_uniView = glGetUniformLocation(m_shaderProgram, "view");
	glUniformMatrix4fv(m_uniView, 1, GL_FALSE, glm::value_ptr(m_view));

	m_proj = glm::perspective(0.785f, float(DEFAULT_WIDTH)/float(DEFAULT_HEIGHT), 0.000005f, 100000.0f);
	m_uniProj = glGetUniformLocation(m_shaderProgram, "proj");
	glUniformMatrix4fv(m_uniProj, 1, GL_FALSE, glm::value_ptr(m_proj));




	// update_colorbar();
	// //std::cout << " colorbar is updated" << std::endl;
	// compile_colorbar();
	// //std::cout << " colorbar is compiled" << std::endl;
}

void Visualizer::PreLoopTasks(){
	return;
}


bool Visualizer::MainLoop(){
	// std::cout << "ABOUT TO DRAW: " << std::endl;
	// std::cout << "    " << _num_point_elements << " point elements" << std::endl;
	// std::cout << "    " << _num_line_elements << " line elements" << std::endl;
	// std::cout << "    " << _num_tri_elements << " tri elements" << std::endl;

	while(!glfwWindowShouldClose(m_window_ptr)){

		LoopTasks();

	    if (glfwGetKey(m_window_ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(m_window_ptr, GL_TRUE);

	    glfwSwapBuffers(m_window_ptr);
	    glfwPollEvents();

	    // Clear the screen to black
	    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	    if (m_display_triangles) 	draw_triangles();
	    if (m_display_edges)		draw_edges();
	    if (m_display_points)		draw_points();
	    // if (m_display_colorbar) draw_colorbar();


	}

	return 0;
}

void Visualizer::LoopTasks(){
	return;
}


void Visualizer::draw_points(){
	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_points);

	glUniformMatrix4fv(m_uniModel, 1, GL_FALSE, glm::value_ptr(m_model));
    glUniformMatrix4fv(m_uniView, 1, GL_FALSE, glm::value_ptr(m_view));
    glUniformMatrix4fv(m_uniProj, 1, GL_FALSE, glm::value_ptr(m_proj));

    // Draw points
    glDrawElements(GL_POINTS, m_point_elems.size() , GL_UNSIGNED_INT, (void *)(0));
	
    glBindVertexArray(0);
	return;
}


void Visualizer::draw_edges(){
	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_edges);

	glUniformMatrix4fv(m_uniModel, 1, GL_FALSE, glm::value_ptr(m_model));
    glUniformMatrix4fv(m_uniView, 1, GL_FALSE, glm::value_ptr(m_view));
    glUniformMatrix4fv(m_uniProj, 1, GL_FALSE, glm::value_ptr(m_proj));

    // Draw lines
    glDrawElements(GL_LINES, m_edge_elems.size() , GL_UNSIGNED_INT, (void *)(0));

    glBindVertexArray(0);
	return;
}


void Visualizer::draw_triangles(){
	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_triangles);

	glUniformMatrix4fv(m_uniModel, 1, GL_FALSE, glm::value_ptr(m_model));
    glUniformMatrix4fv(m_uniView, 1, GL_FALSE, glm::value_ptr(m_view));
    glUniformMatrix4fv(m_uniProj, 1, GL_FALSE, glm::value_ptr(m_proj));

    // Draw triangles
	glDrawElements(GL_TRIANGLES, m_triangle_elems.size() , GL_UNSIGNED_INT, (void *)(0));
	
    glBindVertexArray(0);
	return;
}

void Visualizer::onExit(){

	// free_colorbar();

	glDeleteProgram(m_shaderProgram);
	glDeleteShader(m_fragmentShader);
	glDeleteShader(m_vertexShader);
	
	glDeleteBuffers(1, &m_ebo_triangles);
	glDeleteBuffers(1, &m_ebo_edges);
	glDeleteBuffers(1, &m_ebo_points);
	glDeleteBuffers(1, &m_vbo);

	glDeleteVertexArrays(1, &m_vao);

	glfwDestroyWindow( m_window_ptr );
	glfwTerminate();
	return;
}


void Visualizer::onRefresh(){
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), &(m_vertices.front()), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_triangles);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangle_elems.size() * sizeof(GLuint), &(m_triangle_elems.front()), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_edges);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_edge_elems.size() * sizeof(GLuint), &(m_edge_elems.front()), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_points);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_edge_elems.size() * sizeof(GLuint), &(m_edge_elems.front()), GL_STATIC_DRAW);

	// if (m_display_colorbar){
	// 	free_colorbar();
	// 	update_colorbar();
	// 	compile_colorbar();
	// } 
}







void Visualizer::sMouseClick(GLFWwindow * window, int button, int action, int modifiers){
	for (unsigned int i=0; i<_vInstances.size(); i++){
		if (_vInstances.at(i)->m_window_ptr == window){
			_vInstances.at(i)->onMouseClick(button, action, modifiers);
			return;
		}
	}
	return;
}


void Visualizer::sMouseWheel(GLFWwindow * window, double xoffset, double yoffset){
	for (unsigned int i=0; i<_vInstances.size(); i++){
		if (_vInstances.at(i)->m_window_ptr == window){
			_vInstances.at(i)->onMouseWheel(xoffset, yoffset);
			return;
		}
	}
	return;
}

void Visualizer::sKeyboard(GLFWwindow * window, int key, int scancode, int action, int modifiers){
	for (unsigned int i=0; i<_vInstances.size(); i++){
		if (_vInstances.at(i)->m_window_ptr == window){
			_vInstances.at(i)->onKeyboard(key, scancode, action, modifiers);
			return;
		}
	}
	return;
}

void Visualizer::sCursorPosition(GLFWwindow * window, double xpos, double ypos){
	for (unsigned int i=0; i<_vInstances.size(); i++){
		if (_vInstances.at(i)->m_window_ptr == window){
			_vInstances.at(i)->onCursorPosition(xpos, ypos);
			return;
		}
	}
	return;
}


}