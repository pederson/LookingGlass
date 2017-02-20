#include "VisualixerGeometricObject.hpp"

using namespace std;

//#define _TEST_

vector<visualixer*> _vGeometricObjectInstances;


//*************************************** Mesh Model Visualixer Class *******************************************
mesh_model_visualixer::mesh_model_visualixer(){
  _vGeometricObjectInstances.push_back(this);

	visualixer_active = false;
	window_name = "Mesh Model Visualixer";
	rotation_lock = false;
	//color_ramp = NULL;
	_colorby = nullptr;
	_color_alpha = nullptr;
	vertices = NULL;
	elements = NULL;
	normals = NULL;

	num_vertices = 0;
	num_per_vertex = 0;
	num_elements = 0;
	num_normals = 0;

	_num_point_elements = 0;
	_num_line_elements = 0;
	_num_tri_elements = 0;
	_num_quad_elements = 0;

	model_centroid[0] = 0.0;
  model_centroid[1] = 0.0;
  model_centroid[2] = 0.0;
}

mesh_model_visualixer::~mesh_model_visualixer(){
  for (unsigned int i=0; i<_vGeometricObjectInstances.size(); i++){
		if (_vGeometricObjectInstances.at(i) == this){
			_vGeometricObjectInstances.erase(_vGeometricObjectInstances.begin() + i);
			return;
		}
	}

	//delete[] window_name;
	//if (color_ramp != NULL) delete[] color_ramp;
	if (vertices != NULL) delete[] vertices;
	if (elements != NULL) delete[] elements;
	if (normals != NULL) delete[] normals;
}


void mesh_model_visualixer::add_model(const mesh_model & model){
	// bind the model
	_model = &model;

	// assign basic info
	num_vertices = _model->vertex_count;
	num_per_vertex = 7;
	num_vertex_points = 3;
	
  return;
}


void mesh_model_visualixer::set_test_case(){
  num_vertices = 100;
	num_per_vertex = 7;
	num_vertex_points = 3;
	vertices = new GLfloat[num_vertices*num_per_vertex];
	for (unsigned int i=0; i<num_vertices; i++){
			vertices[i*num_per_vertex] = 0.5*GLfloat(i);
			if (i%2==0) vertices[i*num_per_vertex + 1] = 0.5;
      else vertices[i*num_per_vertex +1] = 0.0;
			if (i%3==1) vertices[i*num_per_vertex + 2] = 0.5;
      else vertices[i*num_per_vertex + 2] = 0.0;
			if (i%2==0){
				vertices[i*num_per_vertex + 3] = 1.0f;
				vertices[i*num_per_vertex + 4]  = 0.0f;
				vertices[i*num_per_vertex + 5] = 0.0f;
			}
			else if (i%3==0){
				vertices[i*num_per_vertex + 3] = 0.0f;
				vertices[i*num_per_vertex + 4]  = 1.0f;
				vertices[i*num_per_vertex + 5] = 0.0f;
			}
			else {
				vertices[i*num_per_vertex + 3] = 0.0f;
				vertices[i*num_per_vertex + 4]  = 0.0f;
				vertices[i*num_per_vertex + 5] = 1.0f;
			}
			vertices[i*num_per_vertex + 6] = 1.0f;
	}


	num_elements = num_vertices-2;
	num_per_element = 3;
  elements = new GLuint[num_elements*num_per_element];
	// set the triangle elements
	for (unsigned int i=0; i<num_vertices-2; i++){
		elements[i*num_per_element] = i;
    elements[i*num_per_element + 1] = i+1;
    elements[i*num_per_element + 2] = i+2;
	}


	model_centroid[0] = num_vertices*0.5*0.5;
	model_centroid[1] = 0.5*0.5;
	model_centroid[2] = 0.0;
	xmax = num_vertices*0.5;
	ymax = 0.5;
	zmax = 0.0;
	xmin = 0;
	ymin = 0;
	zmin = 0;

	return;
}

/*
const GLchar * mesh_model_visualixer::VertexShaderSource(){
	// vertex shader sources
	const GLchar* vertexSource =
	    "#version 140\n"
	    "in vec3 position;"
	    "in vec3 color;"
	    "in vec3 normal;"
	    "out vec3 Color;"
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

const GLchar * mesh_model_visualixer::FragmentShaderSource(){
  // fragment shader source
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

void mesh_model_visualixer::onPrepareData(){

	vertices = new GLfloat[num_vertices*num_per_vertex];
	for (unsigned int i=0; i<num_vertices; i++){
		vertices[i*num_per_vertex] = _model->vertices[i*3];
		vertices[i*num_per_vertex+1] = _model->vertices[i*3+1];
		vertices[i*num_per_vertex+2]= _model->vertices[i*3+2];
		if (i%2==0){
			vertices[i*num_per_vertex + 3] = 0.81f;
			vertices[i*num_per_vertex + 4]  = 0.81f;
			vertices[i*num_per_vertex + 5] = 0.81f;
		}
		else {
			vertices[i*num_per_vertex + 3] = 0.41f;
			vertices[i*num_per_vertex + 4]  = 0.41f;
			vertices[i*num_per_vertex + 5] = 0.41f;
		}
		vertices[i*num_per_vertex + 6] = 1.0f;
	}

	num_elements = num_vertices/3;
	num_per_element = 3;
	_num_tri_elements = num_vertices/3;
	elements = new GLuint[num_elements*num_per_element];
	// set the triangle elements
	for (unsigned int i=0; i<num_elements; i++){
		elements[i*num_per_element] = i*num_per_element;
		elements[i*num_per_element+1] = i*num_per_element+1;
		elements[i*num_per_element+2] = i*num_per_element+2;
	}


	model_centroid[0] = 0.0;
	model_centroid[1] = 0.0;
	model_centroid[2] = 0.0;
	xmax = vertices[0];
	ymax = vertices[1];
	zmax = vertices[2];
	xmin = vertices[0];
	ymin = vertices[1];
	zmin = vertices[2];
	for (unsigned int i=0; i<num_vertices; i++){
		if (vertices[i*num_per_vertex] > xmax) xmax = vertices[i*num_per_vertex];
		if (vertices[i*num_per_vertex] < xmin) xmin = vertices[i*num_per_vertex];
		if (vertices[i*num_per_vertex+1] > ymax) ymax = vertices[i*num_per_vertex+1];
		if (vertices[i*num_per_vertex+1] < ymin) ymin = vertices[i*num_per_vertex+1];
		if (vertices[i*num_per_vertex+2] > zmax) zmax = vertices[i*num_per_vertex+2];
		if (vertices[i*num_per_vertex+2] < zmin) zmin = vertices[i*num_per_vertex+2];

		model_centroid[0] += vertices[i*num_per_vertex];
		model_centroid[1] += vertices[i*num_per_vertex+1];
		model_centroid[2] += vertices[i*num_per_vertex+2];
	}
	model_centroid[0] /= num_vertices;
	model_centroid[1] /= num_vertices;
	model_centroid[2] /= num_vertices;

}

void mesh_model_visualixer::onRender(){
  // Create Vertex Array Object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // create VBO and copy data to it
  glGenBuffers (1, &vbo);

  // visualizer specific data definitions
  // this one happens to be XYRGB
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, num_vertices * num_per_vertex * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	// Create an element array if necessary
	if (num_elements > 0){
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, (num_elements * num_per_element + num_line_elements * num_per_line_element) * sizeof(GLuint), elements, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (num_elements * num_per_element) * sizeof(GLuint), elements, GL_STATIC_DRAW);
  }


	// create a normal buffer
  	if (num_normals > 0){
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, num_normals*sizeof(GLfloat)*3,  normals, GL_STATIC_DRAW);
	}
  //cout << "total elements buffered: " << num_elements * num_per_element<< endl;
  // enable point size specification
  //glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

  // enable face culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // enable depth test culling
  glEnable(GL_DEPTH_TEST);

  return;
}

void mesh_model_visualixer::onShaders(){

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

  // Specify the layout of the vertex data
  //cout << "num_vertices: " << num_vertices << " num_vertex_points: " << num_vertex_points << " num_per_vertex: " << num_per_vertex << endl;
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, num_vertex_points, GL_FLOAT, GL_FALSE, num_per_vertex * sizeof(GLfloat), 0);

  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, num_per_vertex * sizeof(GLfloat), (void*)(num_vertex_points * sizeof(GLfloat)));

  GLint normAttrib = glGetAttribLocation(shaderProgram, "normal");
  glEnableVertexAttribArray(normAttrib);
//glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
	    normAttrib, 3, GL_FLOAT, GL_FALSE, num_per_vertex * sizeof(GLfloat),                                // stride
	    (void*)0                          // array buffer offset
	);

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

  // set up projection matrix
  proj = glm::perspective(0.785f, float(DEFAULT_WIDTH)/float(DEFAULT_HEIGHT), 0.05f, 100000.0f);
  uniProj = glGetUniformLocation(shaderProgram, "proj");
  glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

}

/*
bool mesh_model_visualixer::MainLoop(){

  while(!glfwWindowShouldClose(window_ptr)){

		if (glfwGetKey(window_ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window_ptr, GL_TRUE);

    glfwSwapBuffers(window_ptr);
		glfwPollEvents();

    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    // Draw triangles
    //glCullFace(GL_BACK);
    glDrawElements(GL_TRIANGLES, num_elements*num_per_element , GL_UNSIGNED_INT, NULL);

    //cout << "looping \r" << flush;
	}

	return 0;
}
*/

void mesh_model_visualixer::onExit(){
  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  if (num_elements > 0) glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);

	glfwDestroyWindow( window_ptr );
	glfwTerminate();
	return;
}




#ifdef _TEST_
// use cmake to compile

int main(int argc, char * argv[]){
	// declare vars

	// test the mesh model viewer
	mesh_model_visualixer mymmodvis;
	//mymmodvis->set_test_case();
	mesh_model * mesh_test_model = mesh_model::read_STL("../testfiles/brain-gear.stl");
	mymmodvis.add_model(*mesh_test_model);
	mymmodvis.run();

	return 0;

}

#endif