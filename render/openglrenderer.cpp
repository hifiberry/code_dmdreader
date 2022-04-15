#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <boost/log/trivial.hpp>

#include "openglshader.h"
#include "openglrenderer.h"
#include "shader-definitions.h"


OpenGLRenderer::OpenGLRenderer()
{
}

OpenGLRenderer::~OpenGLRenderer()
{
}

void OpenGLRenderer::render_frame(DMDFrame& f)
{

	const vector<uint8_t> data = f.get_data();

	// resolution changed, this usually only happens rendering the first frame 
	if ((f.get_width() != frame_width) || (f.get_height() != frame_height)) {
		frame_width = f.get_width();
		frame_height = f.get_height();
		if ((f.get_width() == 128) && f.get_height() == 32) {
			tx_width = tx_height = 128;
			tx_pixel_count = 128 * 32;
			vertices[9] = vertices[14] = 0.25f;

		}
		else if ((f.get_width() == 192) && f.get_height() == 64) {
			tx_width = tx_height = 192;
			tx_pixel_count = 192 * 64;
			vertices[9] = vertices[14] = 0.33f;
		}
		else {
			BOOST_LOG_TRIVIAL(warning) << "[openglrenderer] resolution " << f.get_width() << "x" << f.get_height() << "not supported";
			return;
		}
	}

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindTexture(GL_TEXTURE_2D, dmd_texture_id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	if (f.get_bitsperpixel() == 24) {
		// copy data into texture buffer
		memcpy_s(texturbuf, tx_buf_len, &data[0], tx_pixel_count * 3);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tx_width, tx_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texturbuf);
	}
	else if (f.get_bitsperpixel() == 24) {
		// copy data into texture buffer
		memcpy_s(texturbuf, tx_buf_len, &data[0], tx_pixel_count * 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tx_width, tx_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,texturbuf);
	}

	// render
	// ------
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind Textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dmd_texture_id);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, overlay_texture_id);

	// Set textures
	glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);

	// render container
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(window);
}

bool OpenGLRenderer::initialize_display()
{
	// glfw: initialize and configure
// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(width, height, "DMD", NULL, NULL);
	if (window == nullptr)
	{
		BOOST_LOG_TRIVIAL(error) << "[openglrenderer] failed to create GLFW window";
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		BOOST_LOG_TRIVIAL(error) << "[openglrenderer] failed to initialize GLAD";
		return false;
	}

	// build and compile our shader zprogram
	// ------------------------------------
	shader = OpenGLShader(vertexShader, fragmentShader192x64);

	BOOST_LOG_TRIVIAL(info) << "[openglrenderer] OpenGL version: " << glGetString(GL_VERSION);

	// texture 1  - the DMD dislay
	// ---------------------------
	glGenTextures(1, &dmd_texture_id);
	glBindTexture(GL_TEXTURE_2D, dmd_texture_id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// No need for wrapping
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// texture scaling
	if (scale_linear) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}


	// texture 2 - the circle overlay
	// ------------------------------
	glGenTextures(1, &overlay_texture_id);
	glBindTexture(GL_TEXTURE_2D, overlay_texture_id);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(overlay_texture_file.c_str(), &width, &height, &nrChannels, 4);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		BOOST_LOG_TRIVIAL(debug) << "[openglrenderer] loaded overlay_texture " << overlay_texture_file;
		stbi_image_free(data);
	}
	else
	{
		BOOST_LOG_TRIVIAL(warning) << "[openglrenderer] Failed to load overlay_texture " << overlay_texture_file << ", will use no overlay";
		data = new unsigned char [4](255); // create a one-pixel texture that's completely transparent
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		delete[] data;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	shader.use();

	return true;
}

bool OpenGLRenderer::configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_renderer)
{
	width = pt_renderer.get("width", 1280);
	height = pt_renderer.get("height", 320);

	scale_linear = pt_renderer.get("scale_linear", false);

	dmd_x = pt_renderer.get("dmd_x", 0 );
	dmd_y = pt_renderer.get("dmd_y", 0);
	dmd_width = pt_renderer.get("dmd_width", 0);
	dmd_height = pt_renderer.get("dmd_height", 0);

	if (dmd_width == 0) {
		dmd_width = width;
	}

	if (dmd_height == 0) {
		dmd_height = height;
	}

	overlay_texture_file = pt_renderer.get("overlay_texture", "img/circle_blurred.png");

	initialize_display();
	return true;
}
