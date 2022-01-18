#include <iostream>
#include "graphics/Graphics.hpp"
#include "GLFW/glfw3.h"

#include "graphics/Shader.hpp"
#include "graphics/IndexBuffer.hpp"
#include "graphics/VertexBuffer.hpp"
#include "graphics/VertexBufferLayout.hpp"
#include "graphics/VertexArray.hpp"
#include "graphics/Texture2D.hpp"

void error_callback(int error, const char* description)
{
	std::cerr<<"Error: "<<description<<std::endl;
}

int main()
{
    
	glfwSetErrorCallback(error_callback);
    GLFWwindow* window;

    //Start GLFW Library
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create window and ensure creation
	window = glfwCreateWindow(640, 480, "2DGameEngine", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		std::cerr<<"Window not made!"<<std::endl;
		return -1;
	}
	//Make window current
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	//Test GLEW is working and output GL Version
	if (glewInit() != GLEW_OK)
		std::cout << "Error" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	//Set clear color
	glClearColor(0.5f,1.0f,1.0f,1.0f);

	//Test code
	float testVerArray[]=
	{
		-0.5f, -0.5f, 1.0f,0.0f,0.0f, 0.0f,0.0f,
    	0.5f, -0.5f, 0.0f,1.0f,0.0f, 1.0f,0.0f,
     	0.5f,  0.5f, 0.0f,0.0f,1.0f, 1.0f,1.0f,
		-0.5f,  0.5f, 0.0f,0.0f,1.0f, 0.0f,1.0f
	};

	unsigned int testIndArray[]=
	{
		0,1,2,
		0,2,3
	};
	
	VertexBuffer vb(testVerArray,sizeof(testVerArray));
	VertexBufferLayout vbl;
	vbl.push(GL_FLOAT,2,false);
	vbl.push(GL_FLOAT,3,false);
	vbl.push(GL_FLOAT,2,false);
	
	VertexArray va;
	va.addBuffer(vb,vbl);
	IndexBuffer ib(testIndArray,sizeof(testIndArray)/sizeof(testIndArray[0]));
	Shader shad("shaders/basic.vert","shaders/basic.frag");
	shad.bind();
	shad.setUniform1i("texture1",0);
	Texture2D tex("assets/test.png");
	//Wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(window))
    {
		glClear(GL_COLOR_BUFFER_BIT);
		tex.bind();
		shad.bind();
		va.bind();
		ib.bind();
		glCheck(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
		glfwSwapBuffers(window);
		glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}