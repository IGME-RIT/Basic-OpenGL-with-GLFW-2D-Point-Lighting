/*
Title: 2D Normal Mapping
File Name: main.cpp
Copyright � 2016
Author: David Erbelding
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "FreeImage.h"
#include <vector>
#include "../header/shader.h"
#include "../header/spriteBatcher.h"
#include <iostream>

SpriteBatcher* spriteBatcher;
glm::vec2 mousePosition;
glm::vec2 viewportDimensions = glm::vec2(800, 600);

// Window resize callback
void resizeCallback(GLFWwindow* window, int width, int height)
{
    viewportDimensions = glm::vec2(width, height);
	glViewport(0, 0, width, height);
    spriteBatcher->SetScreenSize(viewportDimensions);
}

// This will get called when the mouse moves.
void mouseMoveCallback(GLFWwindow *window, GLdouble mouseX, GLdouble mouseY)
{
    mousePosition = glm::vec2(mouseX, mouseY);
}

int main(int argc, char **argv)
{
	// Initializes the GLFW library
	glfwInit();

	// Initialize window
	GLFWwindow* window = glfwCreateWindow(800, 600, "2D lighting", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	//set resize callback
	glfwSetFramebufferSizeCallback(window, resizeCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);

	// Initializes the glew library
	glewInit();

    ShaderProgram* program = new ShaderProgram();
    program->AttachShader(new Shader("../shaders/vertex.glsl", GL_VERTEX_SHADER));
    program->AttachShader(new Shader("../shaders/fragment.glsl", GL_FRAGMENT_SHADER));
    Material* spriteRender = new Material(program);
    spriteRender->IncRefCount();
    spriteRender->SetTexture("tex", new Texture("../assets/texture.png"));
    spriteRender->SetTexture("norm", new Texture("../assets/normal.png"));


    // Create a spriteBatcher (This one will need the size of the screen)
    spriteBatcher = new SpriteBatcher(glm::vec2(800, 600));


    std::cout << "Move the mouse to change the direction of the light!" << std::endl;


    float frames = 0;
    float secCounter = 0;

	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
        // Calculate delta time and frame rate
        float dt = glfwGetTime();
        frames++;
        secCounter += dt;
        if (secCounter > 1.f)
        {
            std::string title = "2D lighting FPS: " + std::to_string(frames);
            glfwSetWindowTitle(window, title.c_str());
            secCounter = 0;
            frames = 0;
        }
        glfwSetTime(0);

        // Convert the mouse position on screen to world coordinates
        glm::vec2 direction = mousePosition;
        direction.y = viewportDimensions.y - direction.y;
        glm::vec3 direction3d = glm::vec3(direction, 5.f);

        // send the mouse position as the position for the point light
        spriteRender->SetVec3("lightPosition", direction3d);

		// Clear the screen.
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0, 0.0, 0.0, 0.0);


        // Loop and draw 100 sprites to the screen
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                spriteBatcher->Draw(glm::vec4(i * 60, j * 60, 50, 50), glm::vec4(0, 0, 512, 512), glm::vec4(i / 10.f,  j / 10.f, 1, 1), spriteRender);
            }
        }

        // Now that we have a collection of all the draws we want to make, send it all to the gpu to be drawn!
        spriteBatcher->Flush();

		// Swap the backbuffer to the front.
		glfwSwapBuffers(window);

		// Poll input and window events.
		glfwPollEvents();

	}

    delete spriteBatcher;
    spriteRender->DecRefCount();

	// Free GLFW memory.
	glfwTerminate();


	// End of Program.
	return 0;
}
