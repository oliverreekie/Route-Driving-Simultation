#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"

// Class for generating a xz Minecart of a given size
class CMinecart
{
public:
	CMinecart();
	~CMinecart();
	void Create();
	void CreateBody();
	void CreateGold();
	void CreateWheel(glm::vec3, glm::vec3, bool);
	void Render();
	void RenderBody();
	void RenderGold();
	void RenderWheels();
	void Release();
	void CreateTriangle(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec2, glm::vec2, glm::vec2);
private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	UINT m_vao2;
	CVertexBufferObject m_vbo2;
	UINT m_vao3;
	CVertexBufferObject m_vbo3;
	CTexture m_texture;
	CTexture m_texture2;
	string m_directory;
	string m_filename;
	float m_width;
	float m_height;
	int triangles = 18;
	float radius = 0.8f;
};