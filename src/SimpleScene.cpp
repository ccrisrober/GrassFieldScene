/*
*	MIT License
*
* Copyright (c) <2016> <Cristian Rodríguez Bernal (maldicion069)>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "SimpleScene.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

#include "SOIL/SOIL.h"

#define MAXPOINTS 500		// Change this to increment num of points
#define RANDOM_POINT 0.02

SimpleScene::SimpleScene(int w, int h) : IScene(w, h) { }
SimpleScene::~SimpleScene() { }

void SimpleScene::initScene() {
	compileAndLinkShader();
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	skybox = new Skybox("skybox");

	srand(time(NULL));
	GLfloat points[MAXPOINTS * 3 / 2];
	GLfloat points2[MAXPOINTS * 3 / 2];
	for (int i = 0; i < MAXPOINTS * 3 / 2; i += 3) {
		points[i] = ((float)(std::rand() % 1000)) * RANDOM_POINT;
		points[i + 1] = ((float)(std::rand() % 1000)) * RANDOM_POINT;
		points[i + 2] = ((float)(std::rand() % 1000)) * RANDOM_POINT;

		points2[i] = ((float)(std::rand() % 1000)) * RANDOM_POINT;
		points2[i + 1] = ((float)(std::rand() % 1000)) * RANDOM_POINT;
		points2[i + 2] = ((float)(std::rand() % 1000)) * RANDOM_POINT;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points2), &points2, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Texture 1
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// Set our texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		int width, height;
		unsigned char* image = SOIL_load_image("../textures/butterfly.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Texture 2
	{
		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		int width, height;
		unsigned char* image = SOIL_load_image("../textures/dragonfly.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}


void SimpleScene::update( float t ) { }

void SimpleScene::draw(Camera* camera) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	view = camera->GetViewMatrix();
	projection = camera->GetProjectionMatrix();

	// draw scene
	drawScene();

	// WARNING: Skybox change glPolygonMode to GL_FILL
	skybox->render(camera);
}

void SimpleScene::updateMatrices() {
	glm::mat4 modelView = view * model;

	prog.send_uniform("modelViewProj", projection * modelView);
}

void SimpleScene::resize(int w, int h) {
	glViewport(0, 0, w, h);
	width = w;
	height = h;
}

void SimpleScene::compileAndLinkShader() {
	prog.load("../shader/billboard.vert", GL_VERTEX_SHADER);
	prog.load("../shader/billboard.geom", GL_GEOMETRY_SHADER);
	prog.load("../shader/billboard.frag", GL_FRAGMENT_SHADER);
	prog.compile_and_link();
	prog.add_uniform("modelViewProj");
	prog.add_uniform("time");
	prog.add_uniform("beta");
	prog.add_uniform("up");
	prog.add_uniform("showQuad");
	prog.add_uniform("image");
}

void SimpleScene::drawScene() {
	prog.use();
	model = glm::mat4(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, _showSolid ? GL_FILL : GL_LINE);
	prog.send_uniform_b("showQuad", _showQuad);
	prog.send_uniform("time", (float)glfwGetTime());

	// Draw butterflies
	updateMatrices();
	prog.send_uniform("up", 0.01f * (float)sin(glfwGetTime()));
	prog.send_uniform("beta", (float)glm::radians(-60.0));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	prog.send_uniform("image", 0);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, MAXPOINTS/2);

	// Draw dragonflies
	updateMatrices();	// Not necessary (we will not change model matrix)
	prog.send_uniform("up", 0.01f * (float)cos(glfwGetTime()));
	prog.send_uniform("beta", (float)glm::radians(-90.0));

	glBindTexture(GL_TEXTURE_2D, texture2);
	prog.send_uniform("image", 0);	// Not necessary (we will not change texture index (0))
	glBindVertexArray(VAO2);
	glDrawArrays(GL_POINTS, 0, MAXPOINTS/2);
	prog.unuse();
}
