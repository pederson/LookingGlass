#include "Visualixer.hpp"

using namespace std;

//#define _TEST_

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


std::vector<visualixer*> _vInstances;

//*************************************** Base Visualixer Class *******************************************
visualixer::visualixer(){

	// really should keep track of instances of visualixer
	_vInstances.push_back(this);

	visualixer_active = false;
	window_name = "Visualixer";
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

	m_display_colorbar = false;
}

visualixer::~visualixer(){
	for (unsigned int i=0; i<_vInstances.size(); i++){
		if (_vInstances.at(i) == this){
			_vInstances.erase(_vInstances.begin() + i);
			return;
		}
	}

	if (vertices != NULL) delete[] vertices;
	if (elements != NULL) delete[] elements;
	if (_color_alpha != nullptr) delete[] _color_alpha;
	if (_colorby != nullptr) delete[] _colorby;
}

void visualixer::set_window_name(string w_name){
	window_name = w_name;
	return;
}

void visualixer::set_color_ramp(CRamp ramp_name){
  color_ramp.set_ramp(ramp_name);
}





void visualixer::run(){
	onInit();
	onPrepareData();
	onColors();
	onAlpha();
	onRender();
	onShaders();
	PreLoopTasks();
	MainLoop();
	onExit();
	return;
}

void visualixer::set_test_case(){
	num_vertices = 4;
	num_per_vertex = 7;
	num_vertex_points = 3;
	vertices = new GLfloat[num_vertices*num_per_vertex];
	vertices[0] = 10.0-0.5; vertices[1] = 10.0+0.5; vertices[2] = 0.0; vertices[3] = 1.0; vertices[4] = 0.0; vertices[5] = 0.0; vertices[6] = 1.0;
	vertices[7] = 10.0+0.5; vertices[8] = 10.0+0.5; vertices[9] = 0.0; vertices[10] = 0.0; vertices[11] = 1.0; vertices[12] = 0.0; vertices[13] = 1.0;
	vertices[14] = 10.0+0.5; vertices[15] = 10.0-0.5; vertices[16] = 0.0; vertices[17] = 0.0; vertices[18] = 0.0; vertices[19] = 1.0; vertices[20] = 0.1;
	vertices[21] = 10.0-0.5; vertices[22] = 10.0-0.5; vertices[23] = 0.0; vertices[24] = 1.0; vertices[25] = 1.0; vertices[26] = 1.0; vertices[27] = 0.1;

  _num_tri_elements = 2;
  elements = new GLuint[_num_per_tri_element*_num_tri_elements];
  elements[0] = 0; elements[1] = 1; elements[2] = 2;
  elements[3] = 2; elements[4] = 3; elements[5] = 0;



  model_centroid[0] = 10.0;
  model_centroid[1] = 10.0;
  model_centroid[2] = 0.0;
  xmax = 0.5;
  ymax = 0.5;
  zmax = 0.0;
  xmin = -0.5;
  ymin = -0.5;
  zmin = 0.0;

	return;
}





void visualixer::onMouseClick(int button, int action, int modifiers){
	bool state;
	if (action == GLFW_PRESS) state = true;
	else if(action == GLFW_RELEASE) state = false;
	else cout << "unknown mouse state" << endl;


	switch (button){
		case GLFW_MOUSE_BUTTON_LEFT :
			left_mouse_engaged = state;
			if (state) {
				glfwGetCursorPos(window_ptr, &x_upon_click, &y_upon_click);
				new_eye = eye_vec;
			}
			else {
				eye_vec = new_eye;
				recalcCamera();
			}

			break;
		case GLFW_MOUSE_BUTTON_MIDDLE :
			middle_mouse_engaged = state;
			//cout << "middle mouse click " << (state? "on":"off")  << endl;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT :
			right_mouse_engaged = state;
			if (state) {
				glfwGetCursorPos(window_ptr, &x_upon_click, &y_upon_click);
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
			cout << "unknown mouse button" << endl;
	}
	return;
}

void visualixer::onMouseWheel(double xoffset, double yoffset){
	//cout << "MOUSE WHEEL: xoffset: " << xoffset << " yoffset: " << yoffset << endl;
	float zoom_scale_new;
	if (yoffset < 0){
		zoom_level -= 0.05;

	}
	else if (yoffset > 0){
		zoom_level += 0.05;

	}
	//cout << "zoom level: " << zoom_level << endl;
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

void visualixer::onKeyboard(int key, int scancode, int action, int modifiers){
	//cout << "KEYBOARD PRESS" << endl;
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

void visualixer::onCursorPosition(double xpos, double ypos){
	if (left_mouse_engaged) onMouseLeftDrag(xpos, ypos);
	else if(right_mouse_engaged) onMouseRightDrag(xpos, ypos);
	else if(middle_mouse_engaged) onMouseMiddleDrag(xpos, ypos);
}

//DYLAN_TODO: do this using quaternions instead
void visualixer::onMouseLeftDrag(double xpos, double ypos){
	double new_x_pos, new_y_pos;
	int width, height;
	glm::vec3 rot_vec, in_world_ip;

	glfwGetWindowSize(window_ptr, &width, &height);
	glfwGetCursorPos(window_ptr, &new_x_pos, &new_y_pos);

	rotdeg = ((new_x_pos-x_upon_click)*(new_x_pos-x_upon_click)
					 + (new_y_pos-y_upon_click)*(new_y_pos-y_upon_click))/(width*width + height*height)*VX_PI/2;


	in_world_ip = float(new_x_pos-x_upon_click)*-camera_side + float(new_y_pos-y_upon_click)*camera_up;

	// comes from the cross product
	rot_vec = glm::cross(in_world_ip, focus_vec - eye_vec);

	new_eye = focus_vec +  glm::rotate((eye_vec-focus_vec), rotdeg, rot_vec);
	view = glm::lookAt(new_eye, focus_vec, up_vec);
}

void visualixer::onMouseRightDrag(double xpos, double ypos){
	double new_x_pos, new_y_pos;
	int width, height;
	glm::vec3 in_plane, in_world_ip;
	glm::vec4 world_in_plane;
	GLfloat pan_scale, in_plane_norm;

	glfwGetWindowSize(window_ptr, &width, &height);
	glfwGetCursorPos(window_ptr, &new_x_pos, &new_y_pos);


	pan_scale = ((new_x_pos-x_upon_click)*(new_x_pos-x_upon_click) + (new_y_pos-y_upon_click)*(new_y_pos-y_upon_click))/(width*width + height*height)*((xmax-xmin)*(xmax-xmin)+(ymax-ymin)*(ymax-ymin));
	//cout << "pan scale: " << pan_scale << endl;

	in_world_ip = float(new_x_pos-x_upon_click)*-camera_side + float(new_y_pos-y_upon_click)*camera_up;
	in_world_ip = glm::normalize(in_world_ip);

	//cout << "about to translate " << endl;
	new_eye = eye_vec + pan_scale*in_world_ip;
	new_focus = focus_vec + pan_scale*in_world_ip;
	view = glm::lookAt(new_eye, new_focus, up_vec);
	//cout << "finished translating" << endl;
}

void visualixer::onMouseMiddleDrag(double xpos, double ypos){
	//cout << "dragging middle mouse button" << endl;
}

void visualixer::onKeyDown(unsigned char key, int x, int y){
	cout << "Pressed Key with unsigned char : " << key << endl;
	return;
}

void visualixer::onKeyUp(unsigned char key, int x, int y){
	cout << "Released Key with unsigned char: " << key << endl;
	return;
}

void visualixer::onReshape(int new_width, int new_height){
	cout << "reshaping" << endl;
}

void visualixer::SetFullscreen(bool bFullscreen){

}

void visualixer::recalcCamera(){
	camera_side = glm::normalize(glm::cross(focus_vec - eye_vec, up_vec - eye_vec + focus_vec));
	camera_up = glm::normalize(glm::cross(camera_side, focus_vec - eye_vec));

	/*
	cout << "focus_vec: " << focus_vec.x << ", " << focus_vec.y << ", " << focus_vec.z << endl;
	cout << "eye_vec: " << eye_vec.x << ", " << eye_vec.y << ", " << eye_vec.z << endl;
	cout << "up_vec: " << up_vec.x << ", " << up_vec.y << ", " << up_vec.z << endl;
	cout << "camera_side: " << camera_side.x << ", " << camera_side.y << ", " << camera_side.z << endl;
	cout << "camera_up: " << camera_up.x << ", " << camera_up.y << ", " << camera_up.z << endl;
	*/
}

void visualixer::onKeySpace(){
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

	//cout << "zmax is " << zmax << endl;
	view = glm::lookAt(
        eye_vec,
        focus_vec,
        up_vec
    	);
	return;
}

void visualixer::onKeyC(){
	cycleColorRamp();
	return;
}

void visualixer::onKeyF(){

	return;
}

void visualixer::cycleColorRamp(){
	if (_colorby==nullptr) return;
	color_ramp.cycle_ramp();
	//rgb half;
	//half = color_ramp.get_ramp_color(0.5);
	//cout << "new color ramp at 0.5: (" << half.R << ", " << half.G << ", " << half.B << ")" << endl;
	onColors();
	onRefresh();
	return;
}


void visualixer::update_colorbar(){
	if (!m_display_colorbar) return;

	unsigned int rsz = color_ramp.ramp_size();

	GLfloat cramp_min_loc_y = -0.8;
	GLfloat cramp_max_loc_y = 0.8;
	GLfloat cramp_min_loc_x = 0.7;
	GLfloat cramp_max_loc_x = 0.8;

	//cout << "num vertices old: " << m_colorbar_vertices.size() << endl;
	
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

		// cout << "vertex: " << m_colorbar_vertices[4*2*i] << " , " << m_colorbar_vertices[4*2*i + 1] << endl;
		// cout << "vertex: " << m_colorbar_vertices[4*2*i + 2] << " , " << m_colorbar_vertices[4*2*i + 3] << endl;
		// cout << "vertex: " << m_colorbar_vertices[4*2*i + 4] << " , " << m_colorbar_vertices[4*2*i + 5] << endl;
		// cout << "vertex: " << m_colorbar_vertices[4*2*i + 6] << " , " << m_colorbar_vertices[4*2*i + 7] << endl;
		// cout << "color: " << clr.R << " , " << clr.G << " , " << clr.B << endl;
		// cout << "elements: " << m_colorbar_elements[2*3*i] << ", " << m_colorbar_elements[2*3*i+1] << ", " << m_colorbar_elements[2*3*i+2] << endl;
	}

	return;
}

void visualixer::compile_colorbar(){
	if (!m_display_colorbar) return;

	glGenVertexArrays(1, &m_vao_colorbar);
	glBindVertexArray(m_vao_colorbar);

	// generate 2 VBOs
	GLuint vbos[2];
	glGenBuffers(2, &vbos[0]);
	m_vbo_colorbar_vertices = vbos[0];
	m_vbo_colorbar_colors = vbos[1];

	//cout << "about to bind" << endl;

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
	//cout << "about to compile" << endl;

	// compile the vertex shader
	m_colorbar_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar * vssource = this->ColorBar_v_glsl();
	glShaderSource(m_colorbar_vertex_shader, 1, &(vssource), NULL);
	glCompileShader(m_colorbar_vertex_shader);

	GLint status;
	GLint logLength;
	glGetShaderiv(m_colorbar_vertex_shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE){
		cout << "vertex shader failed to compile" << endl;
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
		cout << "fragment shader failed to compile" << endl;
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

void visualixer::draw_colorbar(){
	glUseProgram(m_colorbar_shader_program);
	glBindVertexArray(m_vao_colorbar);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colorbar_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colorbar_colors);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_colorbar);

	glDrawElements(GL_TRIANGLES, m_colorbar_elements.size() , GL_UNSIGNED_INT, (void *)0);

	glBindVertexArray(0);
	//cout << "drawing colorbar" << endl;
}

void visualixer::free_colorbar(){
	if (!m_display_colorbar) return;

	glDeleteProgram(m_colorbar_shader_program);
	glDeleteShader(m_colorbar_fragment_shader);
	glDeleteShader(m_colorbar_vertex_shader);
	
	glDeleteBuffers(1, &m_ebo_colorbar);
	glDeleteBuffers(1, &m_vbo_colorbar_vertices);
	glDeleteBuffers(1, &m_vbo_colorbar_colors);

	glDeleteVertexArrays(1, &m_vao_colorbar);
}

const GLchar * visualixer::ColorBar_v_glsl(){
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

const GLchar * visualixer::ColorBar_f_glsl(){
	const GLchar* fragmentSource =
	    "#version 140\n"
	    "in vec3 Color;"
	    "out vec4 outColor;"
	    "void main() {"
	    "   outColor = vec4(Color, 1.0);"
	    "}";
  return fragmentSource;
}


const GLchar * visualixer::VertexShaderSource(){
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
      	"   gl_PointSize = 2.0;"
	    "   gl_Position = proj*view*model*vec4(position, 1.0);"
	    "}";
	return vertexSource;

}

const GLchar * visualixer::FragmentShaderSource(){
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

void visualixer::onInit(){

	if (!glfwInit()){
		cout << "glfw Init failure " << endl;
		throw -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // This must be compatible with the installed version of OpenGL
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); THIS CAUSES A FAILURE
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);


	window_ptr = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, window_name.c_str(), NULL, NULL); // Windowed
	//GLFWwindow* window = glfwCreateWindow(400, 300, "OpenGL", glfwGetPrimaryMonitor(), NULL); // Fullscreen
	if ( !window_ptr ) {
		cout << "failed to create window" << endl;
        glfwTerminate();
        exit ( EXIT_FAILURE );
  }
	glfwMakeContextCurrent(window_ptr);

	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
    if ( GLEW_OK != glew_status ) {
        exit ( EXIT_FAILURE );
  }

  // set callbacks
  glfwSetMouseButtonCallback(window_ptr, sMouseClick);
  glfwSetScrollCallback(window_ptr, sMouseWheel);
  glfwSetKeyCallback(window_ptr, sKeyboard);
  glfwSetCursorPosCallback(window_ptr, sCursorPosition);
}


void visualixer::onPrepareData(){


}


void visualixer::onColors(){
	if (_colorby == nullptr) return;
	if (_colorby_max - _colorby_min == 0.0) return;
	// modify the vertex array to incorporate user-defined colors


	rgb ptcolor;
	for (auto i=0; i<num_vertices; i++){
		//ptcolor = color_ramp.get_ramp_color(float((_colorby[i])/(_colorby_max - _colorby_min)));
		ptcolor = color_ramp.get_ramp_color(_colorby[i]);
		vertices[(i+1)*num_per_vertex - 4] = ptcolor.R;
		vertices[(i+1)*num_per_vertex - 3] = ptcolor.G;
		vertices[(i+1)*num_per_vertex - 2] = ptcolor.B;
	}

	return;
}


void visualixer::onAlpha(){
	if (_color_alpha == nullptr) return;

	
	for (auto i=0; i<num_vertices; i++){
		vertices[(i+1)*num_per_vertex - 1] = _color_alpha[i];
	}

}


void visualixer::onRender(){

	// Create Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create VBO and copy data to it
	glGenBuffers (1, &vbo);

	// visualizer specific data definitions
	// this one happens to be XYZRGB
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, num_vertices * num_per_vertex * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	// Create an element array if necessary
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (_num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element + _num_tri_elements*_num_per_tri_element + _num_quad_elements*_num_per_quad_element) * sizeof(GLuint), elements, GL_STATIC_DRAW);

	// enable point size specification
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	// enable alpha channel
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// enable depth 
	glEnable(GL_DEPTH_TEST);

  return;
}

void visualixer::onShaders(){

	// create and compile the vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar * vssource = this->VertexShaderSource();
	glShaderSource(vertexShader, 1, &(vssource), NULL);
	glCompileShader(vertexShader);

	GLint status;
	GLint logLength;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE){
		cout << "vertex shader failed to compile" << endl;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH , &logLength);
		if (logLength > 1) {
		    GLchar* compiler_log = (GLchar*)malloc(logLength);
		    glGetShaderInfoLog(vertexShader, logLength, 0, compiler_log);
		    printf("%s\n", compiler_log);
		    free (compiler_log);
		}
	}


	// create and compile the fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar * fssource = this->FragmentShaderSource();
	glShaderSource(fragmentShader, 1, &(fssource), NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE){
		cout << "fragment shader failed to compile" << endl;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH , &logLength);
		if (logLength > 1) {
		    GLchar* compiler_log = (GLchar*)malloc(logLength);
		    glGetShaderInfoLog(fragmentShader, logLength, 0, compiler_log);
		    printf("%s\n", compiler_log);
		    free (compiler_log);
		}
	}



	// link the vertex and fragment shader into a shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify the layout of the vertex data passed into the shader program
	//cout << "num_vertices: " << num_vertices << " num_vertex_points: " << num_vertex_points << " num_per_vertex: " << num_per_vertex << endl;
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, num_vertex_points, GL_FLOAT, GL_FALSE, num_per_vertex * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 	// attribute
						  4, 			// number of values 
						  GL_FLOAT, 	// data type
						  GL_FALSE, 	// take the value as-is
						  num_per_vertex * sizeof(GLfloat), 	// stride
						  (void*)(num_vertex_points * sizeof(GLfloat))	// offset
						  );

	// specify the uniforms that are passed into the shader program
	rotdeg = 0;
	model = glm::rotate(model, rotdeg, glm::vec3(0.0f, 0.0f, 1.0f)); // angle in radians to suppress some output
	uniModel = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	// Set up view matrix
	zoom_level = 1.0f;
	zoom_scale = 1.0f;
	up_vec = glm::vec3(0.0f, 1.0f, 0.0f);
	// calculate the eye z position so that it can view the whole scene
	if (xmax - xmin > ymax - ymin) eyez_init = (xmax-xmin) + zmax;
	else eyez_init = (ymax - ymin) + zmax;
	//cout << "centroid is: " << model_centroid[0] << ", " << model_centroid[1] << ", " << model_centroid[2] << endl;
	focus_vec = glm::vec3(model_centroid[0], model_centroid[1], model_centroid[2]);
	eye_vec = glm::vec3(model_centroid[0], model_centroid[1], eyez_init);
	view = glm::lookAt(
				eye_vec, // camera position
				focus_vec, // the position to be looking at
				up_vec  // the up vector
				);
	recalcCamera();

	uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	proj = glm::perspective(0.785f, float(DEFAULT_WIDTH)/float(DEFAULT_HEIGHT), 0.000005f, 100000.0f);
	uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));




	update_colorbar();
	//cout << " colorbar is updated" << endl;
	compile_colorbar();
	//cout << " colorbar is compiled" << endl;
}

void visualixer::PreLoopTasks(){
	return;
}


bool visualixer::MainLoop(){
	// cout << "ABOUT TO DRAW: " << endl;
	// cout << "    " << _num_point_elements << " point elements" << endl;
	// cout << "    " << _num_line_elements << " line elements" << endl;
	// cout << "    " << _num_tri_elements << " tri elements" << endl;

	while(!glfwWindowShouldClose(window_ptr)){

		LoopTasks();

	    if (glfwGetKey(window_ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window_ptr, GL_TRUE);

	    glfwSwapBuffers(window_ptr);
	    glfwPollEvents();

	    // Clear the screen to black
	    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	    draw_model();
	    if (m_display_colorbar) draw_colorbar();


	}

	return 0;
}

void visualixer::LoopTasks(){
	return;
}

void visualixer::draw_model(){
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    // Draw points, lines, triangles, and quads in that order
    glDrawElements(GL_POINTS, _num_point_elements*_num_per_point_element , GL_UNSIGNED_INT, (void *)((0) * sizeof(GLuint)));
    glDrawElements(GL_LINES, _num_line_elements*_num_per_line_element , GL_UNSIGNED_INT, (void *)((_num_point_elements*_num_per_point_element) * sizeof(GLuint)));
    glDrawElements(GL_TRIANGLES, _num_tri_elements*_num_per_tri_element , GL_UNSIGNED_INT, (void *)((_num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element) * sizeof(GLuint)));
	
    glBindVertexArray(0);
	return;
}


void visualixer::onExit(){

	free_colorbar();

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);

	glfwDestroyWindow( window_ptr );
	glfwTerminate();
	return;
}


void visualixer::onRefresh(){
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ARRAY_BUFFER, num_vertices * num_per_vertex * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (_num_point_elements*_num_per_point_element + _num_line_elements*_num_per_line_element + _num_tri_elements*_num_per_tri_element + _num_quad_elements*_num_per_quad_element) * sizeof(GLuint), elements, GL_STATIC_DRAW);
	
	if (m_display_colorbar){
		free_colorbar();
		update_colorbar();
		compile_colorbar();
	} 
}







void visualixer::sMouseClick(GLFWwindow * window, int button, int action, int modifiers){
	for (unsigned int i=0; i<_vInstances.size(); i++){
		if (_vInstances.at(i)->window_ptr == window){
			_vInstances.at(i)->onMouseClick(button, action, modifiers);
			return;
		}
	}
	return;
}


void visualixer::sMouseWheel(GLFWwindow * window, double xoffset, double yoffset){
	for (unsigned int i=0; i<_vInstances.size(); i++){
		if (_vInstances.at(i)->window_ptr == window){
			_vInstances.at(i)->onMouseWheel(xoffset, yoffset);
			return;
		}
	}
	return;
}

void visualixer::sKeyboard(GLFWwindow * window, int key, int scancode, int action, int modifiers){
	for (unsigned int i=0; i<_vInstances.size(); i++){
		if (_vInstances.at(i)->window_ptr == window){
			_vInstances.at(i)->onKeyboard(key, scancode, action, modifiers);
			return;
		}
	}
	return;
}

void visualixer::sCursorPosition(GLFWwindow * window, double xpos, double ypos){
	for (unsigned int i=0; i<_vInstances.size(); i++){
		if (_vInstances.at(i)->window_ptr == window){
			_vInstances.at(i)->onCursorPosition(xpos, ypos);
			return;
		}
	}
	return;
}





//*************************************** TEST SECTION *******************************************
//*************************************** TEST SECTION *******************************************
//*************************************** TEST SECTION *******************************************
//*************************************** TEST SECTION *******************************************
//*************************************** TEST SECTION *******************************************

#ifdef _TEST_
// use cmake to compile
#include "Converter.hpp"

int main(int argc, char * argv[]){
	// declare vars

	// test the base class
	visualixer * mywindow = new visualixer();
	//cout << "about to run" << endl;
	mywindow->set_test_case();
	mywindow->run();
	//cout << "about to delete mywindow" << endl;
	delete mywindow;

	return 0;

}

#endif
