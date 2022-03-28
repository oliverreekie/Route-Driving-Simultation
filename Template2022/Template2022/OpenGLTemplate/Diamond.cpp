#include "Common.h"
#include "Diamond.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


CDiamond::CDiamond()
{}

CDiamond::~CDiamond()
{}


// Create the Diamond, including its geometry, texture mapping, normal, and colour
void CDiamond::Create(string directory, string filename)
{
	// Load the texture
	m_texture.Load(directory + filename, true);

	m_directory = directory;
	m_filename = filename;

	// Set parameters for texturing using sampler object
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);


	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Create a VBO
	m_vbo.Create();
	m_vbo.Bind();

	//Triangle Verticies
	glm::vec3 g0 = glm::vec3(0.0, 1.5f, 0.0);
	glm::vec3 g1 = glm::vec3(0.0, -1.5f, 0.0);
	glm::vec3 g2 = glm::vec3(1.0, 0.0f, 1.0);
	glm::vec3 g3 = glm::vec3(-1.0, 0.0f, -1.0);
	glm::vec3 g4 = glm::vec3(-1.0, 0.0f, 1.0);
	glm::vec3 g5 = glm::vec3(1.0, 0.0f, -1.0);

	//Texture Coordinates
	glm::vec2 r0 = glm::vec2(0, 0);
	glm::vec2 r1 = glm::vec2(0, 1);
	glm::vec2 r2 = glm::vec2(1, 0);

	//Top Quarter 1
	CreateTriangle(g0, g2, g4, glm::vec3(0, 0.5547, 0.83205), r0, r1, r2);

	//Bottom Quarter 1
	CreateTriangle(g1, g4, g2, glm::vec3(0, 0.5547, -0.83205), r0, r1, r2);

	//Top Quarter 2
	CreateTriangle(g0, g4, g3, glm::vec3(-0.83205, 0.5547, 0), r0, r1, r2);

	//Bottom Quarter 2
	CreateTriangle(g1, g3, g4, glm::vec3(0.83205, 0.5547, 0), r0, r1, r2);

	//Top Quarter 3
	CreateTriangle(g0, g3, g5, glm::vec3(0, 0.5547, -0.83205), r0, r1, r2);

	//Bottom Quarter 3
	CreateTriangle(g1, g2, g5, glm::vec3(-0.83205, 0.5547, 0), r0, r1, r2);

	//Top Quarter 4
	CreateTriangle(g0, g3, g5, glm::vec3(0, 0.5547, -0.83205), r0, r1, r2);

	//Bottom Quarter 4
	CreateTriangle(g1, g3, g5, glm::vec3(0, -0.5547, -0.83205), r0, r1, r2);

	// Upload the VBO to the GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	// Set the vertex attribute locations
	GLsizei stride = sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal  
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}

// Render the Diamond as a triangle strip
void CDiamond::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 38);

}

// Release resources
void CDiamond::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}

void CDiamond::CreateTriangle(glm::vec3 coordinate1, glm::vec3 coordinate2, glm::vec3 coordinate3, glm::vec3 normal, glm::vec2 texture1, glm::vec2 texture2, glm::vec2 texture3) {
	m_vbo.AddData(&coordinate1, sizeof(glm::vec3));
	m_vbo.AddData(&texture1, sizeof(glm::vec2));
	m_vbo.AddData(&normal, sizeof(glm::vec3));
	m_vbo.AddData(&coordinate2, sizeof(glm::vec3));
	m_vbo.AddData(&texture2, sizeof(glm::vec2));
	m_vbo.AddData(&normal, sizeof(glm::vec3));
	m_vbo.AddData(&coordinate3, sizeof(glm::vec3));
	m_vbo.AddData(&texture3, sizeof(glm::vec2));
	m_vbo.AddData(&normal, sizeof(glm::vec3));
}