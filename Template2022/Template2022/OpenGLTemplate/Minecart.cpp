#include "Common.h"
#include "Minecart.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


CMinecart::CMinecart()
{}

CMinecart::~CMinecart()
{}


// Create the Minecart, including its geometry, texture mapping, normal, and colour
void CMinecart::CreateBody() {
	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Create a VBO
	m_vbo.Create();
	m_vbo.Bind();

	//Triangle Verticies
	glm::vec3 g0 = glm::vec3(-2.5f, 1.5f, 3.5f);
	glm::vec3 g1 = glm::vec3(2.5f, 1.5f, 3.5f);
	glm::vec3 g2 = glm::vec3(-1.5f, -1.5f, 2.7f);
	glm::vec3 g3 = glm::vec3(1.5f, -1.5f, 2.7f);
	glm::vec3 g4 = glm::vec3(1.5f, -1.5f, -2.7f);
	glm::vec3 g5 = glm::vec3(2.5f, 1.5f, -3.5f);
	glm::vec3 g6 = glm::vec3(-1.5f, -1.5f, -2.7f);
	glm::vec3 g7 = glm::vec3(-2.5f, 1.5f, -3.5f);

	glm::vec3 J = glm::vec3(-2.1f, 1.5f, 3.1f);
	glm::vec3 K = glm::vec3(2.1f, 1.5f, 3.1f);
	glm::vec3 L = glm::vec3(2.1f, 1.5f, -3.1f);
	glm::vec3 M = glm::vec3(-2.1f, 1.5f, -3.1f);

	glm::vec3 u = glm::vec3(-1.5f, -1.501f, 2.7f);
	glm::vec3 i = glm::vec3(1.5f, -1.501f, -2.7f);
	glm::vec3 o = glm::vec3(-1.5f, -1.501f, -2.7f);
	glm::vec3 w = glm::vec3(1.5f, -1.501f, 2.7f);

	//Texture Coordinates
	glm::vec2 r0 = glm::vec2(0, 0);
	glm::vec2 r1 = glm::vec2(0, 1);
	glm::vec2 r2 = glm::vec2(1, 0);
	glm::vec2 r3 = glm::vec2(1, 1);

	//Normal Coordinates
	glm::vec3 n0 = glm::vec3(0, 0.25766, -0.96623);
	glm::vec3 n1 = glm::vec3(-0.94868, 0.31623, 0);
	glm::vec3 n2 = glm::vec3(0, -0.25766, -0.96623);
	glm::vec3 n3 = glm::vec3(-0.94868, -0.31623, 0);
	glm::vec3 n4 = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 n5 = glm::vec3(0.0f, -1.0f, 0.0f);

	//Front Triangle 1
	CreateTriangle(g0, g2, g1, n0, r0, r2, r1);

	//Front Triangle 2
	CreateTriangle(g1, g3, g2, n0, r1, r3, r2);

	//Right Triangle 1
	CreateTriangle(g3, g4, g1, n1, r0, r1, r2);

	//Right Triangle 2
	CreateTriangle(g1, g5, g4, n1, r2, r3, r1);

	//Back Triangle 1
	CreateTriangle(g4, g6, g5, n2, r0, r1, r2);

	//Back Triangle 2
	CreateTriangle(g5, g7, g6, n2, r2, r3, r1);

	//Left Triangle 1
	CreateTriangle(g6, g2, g7, n3, r0, r1, r2);

	//Left Triangle 2
	CreateTriangle(g7, g0, g2, n3, r2, r3, r1);


	//Top Front Triangle 1
	CreateTriangle(g0, g1, J, n4, r2, r3, r1);

	//Top Front Triangle 2
	CreateTriangle(J, K, g1, n4, r2, r3, r1);

	//Top Back Triangle 1
	CreateTriangle(g5, g7, L, n4, r2, r3, r1);

	//Top Back Triangle 2
	CreateTriangle(L, M, g7, n4, r2, r3, r1);

	//Top Left Triangle 1
	CreateTriangle(g7, g0, M, n4, r2, r3, r1);

	//Top Left Triangle 2
	CreateTriangle(J, g0, M, n4, r1, r3, r2);

	//Top Right Triangle 1
	CreateTriangle(g1, g5, K, n4, r2, r3, r1);

	//Top Right Triangle 2
	CreateTriangle(g5, K, L, n4, r2, r3, r1);

	//Bottom Triangle 1
	CreateTriangle(o, i, u, n5, r2, r3, r1);

	//Bottom Triangle 2
	CreateTriangle(u, w, i, n5, r1, r0, r3);

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

void CMinecart::CreateGold() {
	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vao2);
	glBindVertexArray(m_vao2);

	// Create a VBO
	m_vbo2.Create();
	m_vbo2.Bind();

	//Triangle Verticies
	glm::vec3 J = glm::vec3(-2.1f, 1.5f, 3.1f);
	glm::vec3 K = glm::vec3(2.1f, 1.5f, 3.1f);
	glm::vec3 L = glm::vec3(2.1f, 1.5f, -3.1f);
	glm::vec3 M = glm::vec3(-2.1f, 1.5f, -3.1f);

	//Texture Coordinates
	glm::vec2 r0 = glm::vec2(0, 0);
	glm::vec2 r1 = glm::vec2(0, 1);
	glm::vec2 r2 = glm::vec2(1, 0);
	glm::vec2 r3 = glm::vec2(1, 1);

	//Normal Coordinates
	glm::vec3 n0 = glm::vec3(0, 1, 0);

	//Triangle 1
	m_vbo2.AddData(&J, sizeof(glm::vec3));
	m_vbo2.AddData(&r3, sizeof(glm::vec2));
	m_vbo2.AddData(&n0, sizeof(glm::vec3));
	m_vbo2.AddData(&K, sizeof(glm::vec3));
	m_vbo2.AddData(&r2, sizeof(glm::vec2));
	m_vbo2.AddData(&n0, sizeof(glm::vec3));
	m_vbo2.AddData(&L, sizeof(glm::vec3));
	m_vbo2.AddData(&r0, sizeof(glm::vec2));
	m_vbo2.AddData(&n0, sizeof(glm::vec3));

	//Triangle 1
	m_vbo2.AddData(&L, sizeof(glm::vec3));
	m_vbo2.AddData(&r0, sizeof(glm::vec2));
	m_vbo2.AddData(&n0, sizeof(glm::vec3));
	m_vbo2.AddData(&J, sizeof(glm::vec3));
	m_vbo2.AddData(&r3, sizeof(glm::vec2));
	m_vbo2.AddData(&n0, sizeof(glm::vec3));
	m_vbo2.AddData(&M, sizeof(glm::vec3));
	m_vbo2.AddData(&r1, sizeof(glm::vec2));
	m_vbo2.AddData(&n0, sizeof(glm::vec3));

	// Upload the VBO to the GPU
	m_vbo2.UploadDataToGPU(GL_STATIC_DRAW);

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

void CMinecart::CreateWheel(glm::vec3 pos, glm::vec3 n0, bool side) {

	// Use VAO to store state associated with vertices
	glGenVertexArrays(1, &m_vao3);
	glBindVertexArray(m_vao3);

	// Create a VBO
	m_vbo3.Create();
	m_vbo3.Bind();

	//Vector to hold verticies
	vector<float> yVertices;
	vector<float> zVertices;

	//Center position

	for (int i = 0; i < triangles; i++) {
		zVertices.push_back(pos.z + (radius * cos(i * ((M_PI * 2) / triangles))));
		yVertices.push_back(pos.y + (radius * sin(i * ((M_PI * 2) / triangles))));
	}

	//Center texture coordinate
	glm::vec2 r0 = glm::vec2(0.5, 0.5);

	glm::vec3 pos1 = pos;
	glm::vec3 pos2 = pos;

	m_vbo3.AddData(&pos, sizeof(glm::vec3));
	m_vbo3.AddData(&r0, sizeof(glm::vec2));
	m_vbo3.AddData(&n0, sizeof(glm::vec3));

	if (side == false) {
		for (int i = triangles - 1; i >= 0; i--) {
			pos1.y = yVertices[i];
			pos1.z = zVertices[i];

			glm::vec2 texture = glm::vec2(0, 0);
			texture.x = (cos(i * (2 * M_PI / triangles)) * 0.5) + 0.5;
			texture.y = (sin(i * (2 * M_PI / triangles)) * 0.5) + 0.5;

			m_vbo3.AddData(&pos1, sizeof(glm::vec3));
			m_vbo3.AddData(&texture, sizeof(glm::vec2));
			m_vbo3.AddData(&n0, sizeof(glm::vec3));
		}

		pos1.y = yVertices[triangles - 1];
		pos1.z = zVertices[triangles - 1];

		glm::vec2 texture = glm::vec2(0, 0);
		texture.x = (cos(0 * (2 * M_PI / triangles)) * 0.5f) + 0.5;
		texture.y = (sin(0 * (2 * M_PI / triangles)) * 0.5f) + 0.5;

		m_vbo3.AddData(&pos1, sizeof(glm::vec3));
		m_vbo3.AddData(&texture, sizeof(glm::vec2));
		m_vbo3.AddData(&n0, sizeof(glm::vec3));
	}
	else {
		for (int i = 0; i < triangles; i++) {
			pos1.y = yVertices[i];
			pos1.z = zVertices[i];

			glm::vec2 texture = glm::vec2(0, 0);
			texture.x = (cos(i * (2 * M_PI / triangles)) * 0.5) + 0.5;
			texture.y = (sin(i * (2 * M_PI / triangles)) * 0.5) + 0.5;

			m_vbo3.AddData(&pos1, sizeof(glm::vec3));
			m_vbo3.AddData(&texture, sizeof(glm::vec2));
			m_vbo3.AddData(&n0, sizeof(glm::vec3));
		}

		pos1.y = yVertices[0];
		pos1.z = zVertices[0];

		glm::vec2 texture = glm::vec2(0, 0);
		texture.x = (cos(0 * (2 * M_PI / triangles)) * 0.5f) + 0.5;
		texture.y = (sin(0 * (2 * M_PI / triangles)) * 0.5f) + 0.5;

		m_vbo3.AddData(&pos1, sizeof(glm::vec3));
		m_vbo3.AddData(&texture, sizeof(glm::vec2));
		m_vbo3.AddData(&n0, sizeof(glm::vec3));
	}

	// Upload the VBO to the GPU
	m_vbo3.UploadDataToGPU(GL_STATIC_DRAW);

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

// Render the Minecart as a triangle strip
void CMinecart::Render()
{
	RenderBody();

	CreateGold();
	RenderGold();

	//True represents left wheels and false are right wheels
	CreateWheel(glm::vec3(-1.9, -1.2, 2.1), glm::vec3(-1, 0, 0), true);
	RenderWheels();
	CreateWheel(glm::vec3(-1.9, -1.2, -2.1), glm::vec3(-1, 0, 0), true);
	RenderWheels();

	CreateWheel(glm::vec3(1.9, -1.2, 2.1), glm::vec3(-1, 0, 0), false);
	RenderWheels();
	CreateWheel(glm::vec3(1.9, -1.2, -2.1), glm::vec3(-1, 0, 0), false);
	RenderWheels();
}

void CMinecart::RenderBody() {
	// Load the texture
	m_texture.Load("resources\\textures\\WoodPanel.png", true);

	// Set parameters for texturing using sampler object
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindVertexArray(m_vao);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 54);
}

void CMinecart::RenderGold() {
	// Load the texture
	m_texture.Load("resources\\textures\\gold.jpg", true);

	// Set parameters for texturing using sampler object
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindVertexArray(m_vao2);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
}

void CMinecart::RenderWheels() {
	// Load the texture
	m_texture.Load("resources\\textures\\wheel.png", true);

	// Set parameters for texturing using sampler object
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindVertexArray(m_vao3);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_FAN, 0, triangles + 2);
}
// Release resources
void CMinecart::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
	glDeleteVertexArrays(1, &m_vao2);
	m_vbo2.Release();
}

void CMinecart::CreateTriangle(glm::vec3 coordinate1, glm::vec3 coordinate2, glm::vec3 coordinate3, glm::vec3 normal, glm::vec2 texture1, glm::vec2 texture2, glm::vec2 texture3) {
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