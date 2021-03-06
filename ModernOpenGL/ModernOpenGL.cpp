// ModernOpenGL.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.


#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <GL/glew.h> // add glew32.dll, opengl32.dll 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace glm;

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

int main()
{
	if (!glfwInit())
	{
		fprintf(stderr, "GLFW Initialize Failed");
		return -1;
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;
	int width = 800;
	int height = 600;

	window = glfwCreateWindow(width, height, "ModernOpenGL_1", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "GLFW Window Open Failed");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); 

	glewExperimental = true; 
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
		
	//-----------------------------------------------------------------------------------
	//Shader
	GLuint programID = LoadShaders("vertexshader.glsl", "fragmentshader.glsl");

	
	static const GLfloat position_data[] = {
		-0.5, -0.5, 0.0f,
		0.5, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
	};

	static const GLfloat color_data[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	
				
	//-----------------------------------------------------------------------------------
	//VAO : Vertex Array Object
	GLuint triangleVAO;
	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);

	//-----------------------------------------------------------------------------------
	//VBO : Vertex Buffer Object 
	GLuint positionbuffer;
	glGenBuffers(1, &positionbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position_data), position_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color_data), color_data, GL_STATIC_DRAW);
	
	//-----------------------------------------------------------------------------------
	//Link shader 
	GLuint vertexPosition_modelspace = glGetAttribLocation(programID, "vertexPosition_modelspace");
	if (vertexPosition_modelspace == -1)
	{
		cout << "vertex position setting failed" << endl;
	}
	
	GLuint vertexColor = glGetAttribLocation(programID, "vertexColor");
	if (vertexColor == -1)
	{
		cout << "vertex color setting failed" << endl;
	}

	//-----------------------------------------------------------------------------------
	mat4 Model = glm::mat4(1.0f);
	mat4 View = lookAt(
		vec3(0, 0, 3), //eye
		vec3(0, 0, 0), //at
		vec3(0, 1, 0)  //up
	);
	mat4 Projection = perspective(radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	mat4 MVP = Projection * View * Model;

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	//-----------------------------------------------------------------------------------
	
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0)
	{		
		glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//------------------------------------------------------------------------------
		glUseProgram(programID);		
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(vertexPosition_modelspace);
		glEnableVertexAttribArray(vertexColor);
		
		glBindBuffer(GL_ARRAY_BUFFER, positionbuffer);
		glVertexAttribPointer(vertexPosition_modelspace, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(vertexColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		glDrawArrays(GL_TRIANGLES, 0, 3);

		
		glDisableVertexAttribArray(vertexPosition_modelspace);
		glDisableVertexAttribArray(vertexColor);		
					
		//------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteProgram(programID);
	glDeleteBuffers(1, &positionbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteVertexArrays(1, &triangleVAO);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
{

	// Create Shader
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Vertex shader code read from file 
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if (VertexShaderStream.is_open()) {
		stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf(" Can't read file : %s\n ", vertex_file_path);
		getchar();
		return 0;
	}

	// Fragment shader code read from file 
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if (FragmentShaderStream.is_open()) {
		stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Vertex shader code compile
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Vertex shader code check
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Fragment shader code compile
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Fragment shader code check
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// program Link
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// program check
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}